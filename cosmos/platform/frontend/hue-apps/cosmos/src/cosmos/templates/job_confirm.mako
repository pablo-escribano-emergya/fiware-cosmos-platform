<%namespace name="shared" file="shared_components.mako" />
<%def name="render_error(err)">
  <div class="jframe-error">
    ${unicode(err) | n}
  </div>
</%def>

${shared.header("Run new job", section="define_job")}

<div id="run_job" class="jframe_padded view">
    <h4 class="jframe-hidden">Run new job</h4>

    <p>Please, review job settings.</p>

    <table class="cos-job_summary">
        <thead>
            <tr>
                <th colspan="2">Configuration</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <th>Name</th>
                <td>${ job['name'] }</td>
            </tr>
            <tr>
                <th>Description</th>
                <td>${ job['description'] }</td>
            </tr>
            <tr>
                <th>JAR file</th>
                <td>${ job['jar_path'] }</td>
            </tr>
            % if not parameters:
            <tr>
                <th>Dataset path</th>
                <td>${ job['dataset_path'] }</td>
            </tr>
            % endif
        </tbody>
        % if parameters:
        <thead>
            <tr>
                <th colspan="2">Custom parameters</th>
            </tr>
        </thead>
        <tbody>
            % for param in parameters.parameters:
            <tr>
                <th>${ param.name }</th>
                <td>${ param.get_value() }</td>
            </tr>
            % endfor
        </tbody>
        % endif
    </table>

    <form action="${ url('confirm_job') }" method="POST" class="cos-run_job_form"
          enctype="multipart/form-data">
        ${shared.wizard_navigation()}
    </form>
</div>

${shared.footer()}
## vim:set syntax=mako: