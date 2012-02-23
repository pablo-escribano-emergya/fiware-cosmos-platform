package es.tid.ps.mobility.jobs.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.ps.mobility.MobilityMain;
import es.tid.ps.mobility.mapreduce.FilterCellnoinfoMapper;

/**
 *
 * @author dmicol
 */
public class FilterCellnoinfoJob extends Job {
    private static final String JOB_NAME = "FilterCellnoinfo";

    public FilterCellnoinfoJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MobilityMain.class);
        this.setInputFormatClass(SequenceFileInputFormat.class);
        this.setMapOutputKeyClass(LongWritable.class);
        this.setMapOutputValueClass(ProtobufWritable.class);
        this.setOutputKeyClass(LongWritable.class);
        this.setOutputValueClass(ProtobufWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        this.setMapperClass(FilterCellnoinfoMapper.class);
    }

    public void configure(Path cdrsSamplePath, Path cdrsSmpPath)
            throws IOException {
        FileInputFormat.addInputPath(this, cdrsSamplePath);
        FileOutputFormat.setOutputPath(this, cdrsSmpPath);
    }
}
