package es.tid.ps.mobility;

import java.io.IOException;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.ps.mobility.jobs.mapreduce.GetSample10000Job;
import es.tid.ps.mobility.jobs.parsing.ParseCdrsJob;
import es.tid.ps.mobility.jobs.parsing.ParseCellsJob;

/**
 *
 * @author dmicol
 */
public class MobilityMain extends Configured implements Tool {
    // TODO: don't use hard-coded paths.
    private static final String WORKING_DIRECTORY = "/home/hdfs/mobility";
    private static final String CDRS_MOB_PATH =
            WORKING_DIRECTORY + "/cdrs_mob";
    private static final String CELLS_MOB_PATH =
            WORKING_DIRECTORY + "/cells_mob";
    private static final String CDRS_SAMPLE_PATH =
            WORKING_DIRECTORY + "/cdrs_sample";
    
    @Override
    public int run(String[] args)
            throws IOException, ClassNotFoundException, InterruptedException {
        if (args.length != 2) {
            throw new IllegalArgumentException("Mandatory parameters: "
                    + "cdrs_path cells_path");
        }

        Path cdrsPath = new Path(args[0]);
        Path cellsPath = new Path(args[1]);
        
        if (!this.runParsingJobs(cdrsPath, cellsPath)) {
            return 1;
        }
        
        if (!this.runGetVectorsToClusterJobs()) {
            return 1;
        }

        return 0;
    }
    
    private boolean runParsingJobs(Path cdrsPath, Path cellsPath)
            throws IOException, ClassNotFoundException, InterruptedException {
        ParseCdrsJob parseCdrsJob = new ParseCdrsJob(this.getConf());
        parseCdrsJob.configure(cdrsPath, new Path(CDRS_MOB_PATH));
        if (!parseCdrsJob.waitForCompletion(true)) {
            return false;
        }
        
        ParseCellsJob parseCellsJob = new ParseCellsJob(this.getConf());
        parseCellsJob.configure(cellsPath, new Path(CELLS_MOB_PATH));
        if (!parseCellsJob.waitForCompletion(true)) {
            return false;
        }
        
        return true;
    }
    
    private boolean runGetVectorsToClusterJobs()
            throws IOException, ClassNotFoundException, InterruptedException {
        GetSample10000Job getSample10000Job = new GetSample10000Job(
                this.getConf());
        getSample10000Job.configure(new Path(CDRS_MOB_PATH),
                                    new Path(CDRS_SAMPLE_PATH));
        if (!getSample10000Job.waitForCompletion(true)) {
            return false;
        }
       
        return true;
    }
    
    public static void main(String[] args) {
        try {
            int res = ToolRunner.run(
                    new Configuration(), new MobilityMain(), args);
            System.exit(res);
        } catch (Exception ex) {
            ex.printStackTrace(System.err);
            System.exit(1);
        }
    }
}
