# -*- coding: utf-8 -*-
import logging

from desktop.lib.django_util import PopupException, render
from django.core.paginator import EmptyPage, PageNotAnInteger
from django.core.urlresolvers import reverse
from django.forms.util import ErrorList
from django.shortcuts import redirect
import jobsub.views as jobsub
from jobsub.models import Submission
from jobsubd.ttypes import (BinHadoopStep, LocalizedFile, LocalizeFilesStep,
                            State, SubmissionPlan, SubmissionPlanStep)

from cosmos import mongo, paths
from cosmos.expansion import ExpansionContext
from cosmos.jar import InvalidJarFile, JarFile
from cosmos.hdfs_util import CachedHDFSFile
from cosmos.models import JobRun
from cosmos.forms import BasicConfigurationForm, DefineJobForm

LOGGER = logging.getLogger(__name__)
TEMP_JAR = "tmp.jar"
DEFAULT_PAGE = 1


def index(request):
    """List job runs."""

    job_runs = JobRun.objects.filter(user=request.user).order_by('-start_date')
    for job_run in job_runs:
        job_run.refresh_state()
    return render('job_list.mako', request, dict(
        job_runs=[job for job in job_runs if job.state() != 'unsubmitted']
    ))


def hadoop_args(job, jar_name, expansion):
    """Generate command line arguments for a job execution."""
    args = ['jar', jar_name]
    if job.is_parameterized():
        args.extend(job.parameters.as_hadoop_args(job, expansion))
    else:
        input_path = expansion.expand(job.dataset_path)
        output_path = paths.tmp_path(job.user, job.id)
        args.extend([input_path, output_path,
                     mongo.user_coll_url(job.user.id, job.mongo_collection())])
    return args


def submit(job, expansion):
    """Submit a job through the jobsubd infrastructure."""

    LOGGER.info("Submitting job %s (%d) for %s with JAR=%s on dataset %s" %
                (job.name, job.id, job.user.username, job.jar_path,
                 job.dataset_path))

    lf = LocalizedFile(target_name=TEMP_JAR, path_on_hdfs=job.jar_path)
    lfs = LocalizeFilesStep(localize_files=[lf])
    bhs = BinHadoopStep(arguments=hadoop_args(job, TEMP_JAR, expansion))
    plan = SubmissionPlan(name=job.name,
                          user=job.user.username,
                          groups=job.user.get_groups(),
                          steps=[SubmissionPlanStep(localize_files_step=lfs),
                                 SubmissionPlanStep(bin_hadoop_step=bhs)])
    submission = Submission(owner=job.user,
                            name=job.name,
                            last_seen_state=State.SUBMITTED,
                            submission_plan=plan)
    submission.save()
    job.submission = submission
    job.save()
    try:
        try:
            submission.submission_handle = jobsub.get_client().submit(plan)
        except Exception:
            submission.last_seen_state = State.ERROR
            raise
    finally:
        submission.save()


def job_wizard(request):
    """Failsafe access to the wizard state."""
    return request.session.get('job_wizard', {})


def clear_job_wizard(request):
    """Forget wizard state."""
    request.session.pop('job_wizard', '')


def update_job_wizard(request, wizard):
    """Make sure wizard state is made persistent."""
    request.session['job_wizard'] = wizard


def define_job(request):
    """Defines a new job in user session."""

    wizard = job_wizard(request)
    job = wizard.get('job', {})

    if request.method != 'POST':
        form = DefineJobForm(initial=job)

    else:
        form = DefineJobForm(request.POST, request.FILES)
        if form.is_valid(request.fs):
            job.update(form.cleaned_data)
            wizard['job'] = job
            update_job_wizard(request, wizard)

            cached_file = None
            jar = None
            try:
                cached_file = CachedHDFSFile(request.fs, job['jar_path'])
                try:
                    jar = JarFile(cached_file.local_path())
                    wizard['parameterized'] = jar.is_parameterized()
                    if wizard['parameterized']:
                        wizard['parameters'] = jar.parameters()
                    else:
                        wizard['parameters'] = None
                    return redirect(reverse('configure_job'))
                except InvalidJarFile, ex:
                    errors = form._errors.setdefault('jar_path', ErrorList())
                    errors.append('Invalid JAR: %s' % ex.message)
            finally:
                if jar:
                    jar.close()
                if cached_file:
                    cached_file.close()

    return render('job_define.mako', request, dict(
        form=form,
        wizard_nav={'next': True, 'back': False}
    ))


def configure_job(request):
    """Job configuration step."""

    wizard = job_wizard(request)
    if not wizard.has_key('parameterized'):
        return redirect(reverse('define_job'))

    if wizard['parameterized']:
        return configure_parameterized_job(request)
    else:
        return configure_basic_job(request)


def configure_basic_job(request):
    """
    Configure non-parameterized job.

    Only input path is requested.
    """
    wizard = job_wizard(request)
    if request.method != 'POST':
        form = BasicConfigurationForm(data=wizard.get('job'))
    elif request.POST.has_key('back'):
        return redirect(reverse('define_job'))
    else:
        form = BasicConfigurationForm(request.POST)
        if form.is_valid(request.fs):
            wizard['job'].update(form.cleaned_data)
            update_job_wizard(request, wizard)
            return redirect(reverse('confirm_job'))

    return render('job_configure.mako', request, dict(
        form=form,
        wizard_nav={'next': True, 'back': True}
    ))


def configure_parameterized_job(request):
    """
    Configure parameterized job.

    A form is dynamically generated from the JAR metadata.
    """
    wizard = job_wizard(request)
    parameters = wizard['parameters']
    expansion = ExpansionContext(user=request.user)

    if request.method != 'POST':
        form = parameters.as_form(expansion)
    elif request.POST.has_key('back'):
        return redirect(reverse('define_job'))
    else:
        parameters.update_from_form(request.POST)
        update_job_wizard(request, wizard)
        form = parameters.as_form(expansion)
        if form.is_valid():
            return redirect(reverse('confirm_job'))

    return render('job_configure.mako', request, dict(
        form=form,
        wizard_nav={'next': True, 'back': True}
    ))


def confirm_job(request):
    """Confirm and run."""

    wizard = job_wizard(request)
    job_data = wizard.get('job', {})
    job = JobRun(user=request.user)
    try:
        job.name = job_data['name']
        job.description = job_data['description']
        job.jar_path = job_data['jar_path']
    except KeyError:
        return redirect(reverse('define_job'))

    if wizard['parameterized']:
        job.parameters = wizard['parameters']
    else:
        job.dataset_path = job_data['dataset_path']

    if request.method != 'POST':
        return render('job_confirm.mako', request, dict(wizard,
            wizard_nav={'next': False, 'back': True, 'finish': 'Run job'}
        ))
    elif request.POST.has_key('back'):
        return redirect(reverse('configure_job'))
    else:
        job.save()
        submit(job, ExpansionContext(job=job, user=request.user))
        clear_job_wizard(request)
        return redirect(reverse('list_jobs'))


def cancel_job(request):
    """Cancels and restarts the new job wizard."""
    clear_job_wizard(request)
    return redirect(reverse('define_job'))


def ensure_dir(fs, path):
    """Makes sure a directory with a given path exists or it is created.
       Raises a popup if a file is in the way"""
    if fs.isdir(path):
        return
    if fs.exists(path):
        raise PopupException(('A file named "%s" is preventing the creation ' +
                              'of the upload directory. Please, rename it to ' +
                              'proceed.') % path)
    else:
        fs.mkdir(path)


def upload_index(request):
    """Allows to upload datasets and jars."""

    datasets_base = paths.datasets_base(request.user)
    ensure_dir(request.fs, datasets_base)
    jars_base = paths.jars_base(request.user)
    ensure_dir(request.fs, jars_base)
    return render('upload_index.mako', request, dict(
        datasets_base=datasets_base,
        jars_base=jars_base
    ))


def list_results(request):
    """List result collections."""

    try:
        collections = mongo.list_collections(request.user.id)
    except mongo.NoConnectionError:
        return database_not_available(request)

    return render('results_list.mako', request, dict(
        collections=collections
    ))


def show_results(request, collection_name):
    """
    Render a page of a collection documents.
    """
    try:
        primary_key = request.GET.get('primary_key')
        paginator = mongo.retrieve_results(request.user.id, collection_name,
                                           primary_key)

        try:
            page_num = request.GET.get('page', DEFAULT_PAGE)
        except ValueError:
            page_num = DEFAULT_PAGE

        try:
            page = paginator.page(page_num)
        except PageNotAnInteger:
            page = paginator.page(DEFAULT_PAGE)
        except EmptyPage:
            page = paginator.page(paginator.num_pages)

        prototype_result = page.object_list[0]
        if primary_key is None:
            primary_key = prototype_result.pk

    except mongo.NoConnectionError:
        return database_not_available(request)

    except mongo.NoResultsError:
        page = None

    return render('results_detail.mako', request, dict(
        title='Results collection %s' % collection_name,
        page=page,
        hidden_keys=mongo.HIDDEN_KEYS,
        primary_key=primary_key
    ))

def database_not_available(request):
    """Render error page when database is not available."""
    return render('error.mako', request, dict(
        error_title='Cannot contact database',
        error_details='Database backend is unreachable or not responding'))
