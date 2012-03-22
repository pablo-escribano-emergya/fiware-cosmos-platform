"""Job configuration urls. """

from django.conf.urls.defaults import patterns, include, url

import views

urlpatterns = patterns('',
    url(r'^$', views.list_jobs, name="list_jobs"),
    url(r'new/$', views.new_job, name="new_job"),
    url(r'job/(\d+)/results/$', views.results, name="view_results"),
    url(r'job/(\d+)/config/$', views.config_job, name="config_job"),
)
