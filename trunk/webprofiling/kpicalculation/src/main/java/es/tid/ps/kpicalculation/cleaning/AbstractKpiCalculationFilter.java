package es.tid.ps.kpicalculation.cleaning;

import java.text.MessageFormat;
import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.hadoop.conf.Configuration;

/**
 * @author javierb
 */
public abstract class AbstractKpiCalculationFilter implements
        IKpiCalculationFilter {
    private static final String FILTER_PARAMETER_DELIMITER = "\n";

    protected Pattern pattern;
    protected Matcher matcher;
            
    protected void init(Configuration conf, String configParameter,
            String regularExpression) {
        String forbiddenPattern = setPattern(regularExpression,
                conf.get(configParameter));
        this.pattern = Pattern.compile(forbiddenPattern,
                Pattern.CASE_INSENSITIVE);
    }

    @Override
    public abstract void filter(String url);

    /**
     * Builds the regExp pattern that will filter input URL's using a predefined
     * regExp template specific of each filter and the values to be filtered
     * that will be in a config file
     * 
     * @param regExp
     *            Basic template of the regExp
     * @param values
     *            String of values to be filtered
     * @return Final regExp to be applied
     */
    protected String setPattern(String regExp, String values) {
        StringTokenizer stt = new StringTokenizer(values.trim(),
                FILTER_PARAMETER_DELIMITER);
        StringBuilder filters = new StringBuilder("(");
        while (stt.hasMoreTokens()) {
            filters.append(stt.nextToken().trim().replace(".", "\\.")).append(
                    "|");
        }
        filters.setCharAt(filters.length() - 1, ')');
        return MessageFormat.format(regExp, filters.toString());
    }
}
