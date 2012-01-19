package es.tid.ps.dynamicprofile.categoryextraction;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import org.apache.hadoop.io.NullWritable;

public class CategoryExtractionMapper extends
        Mapper<LongWritable, Text, CompositeKey, NullWritable> {
    @Override
    public void map(LongWritable key, Text value, Context context) {
        try {
            WebLog webLog = new WebLog();
            webLog.set(value.toString());
            CompositeKey cKey = new CompositeKey(webLog.visitorId,
                    webLog.fullUrl);
            context.write(cKey, NullWritable.get());
        } catch (Exception ex) {
            System.err.println(ex.getMessage());
            context.getCounter(CategoryExtractionCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
            ex.printStackTrace(System.err);
        }
    }
}