package es.tid.cosmos.mobility.preparing;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.cosmos.base.mapreduce.MapJob;
import es.tid.cosmos.base.mapreduce.ReduceJob;
import es.tid.cosmos.mobility.util.ConvertCdrToMobDataReducer;
import es.tid.cosmos.mobility.util.ConvertCellToMobDataReducer;

/**
 *
 * @author dmicol
 */
public final class PreparingRunner {
    private PreparingRunner() {
    }

    public static void run(Path tmpPath, Path cdrsMobPath, Path cdrsInfoPath,
                           Path cdrsNoinfoPath, Path cellsMobPath,
                           Path clientsBtsPath, Path btsCommsPath,
                           Path cdrsNoBtsPath, Path viTelmonthBtsPath,
                           Configuration conf) throws Exception {
        FileSystem fs = FileSystem.get(conf);
        
        {
            MapJob job = MapJob.create(conf, "FilterCellnoinfoMapper",
                    SequenceFileInputFormat.class,
                    FilterCellnoinfoMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsMobPath);
            FileOutputFormat.setOutputPath(job, cdrsInfoPath);
            job.waitForCompletion(true);
        }
        
        {
            MapJob job = MapJob.create(conf, "FilterCellnoinfoMapper",
                    SequenceFileInputFormat.class,
                    FilterCellnoinfoMapper.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsMobPath);
            FileOutputFormat.setOutputPath(job, cdrsNoinfoPath);
            job.waitForCompletion(true);
        }

        Path cdrsInfoMobDataPath = tmpPath.suffix("/cdrs_info_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf, "ConvertCdrToMobData",
                    SequenceFileInputFormat.class,
                    ConvertCdrToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cdrsInfoPath);
            FileOutputFormat.setOutputPath(job, cdrsInfoMobDataPath);
            job.waitForCompletion(true);
        }
        
        Path cellsMobDataPath = tmpPath.suffix("/cells_mob_data");
        {
            ReduceJob job = ReduceJob.create(conf, "ConvertCellToMobData",
                    SequenceFileInputFormat.class,
                    ConvertCellToMobDataReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, cellsMobPath);
            FileOutputFormat.setOutputPath(job, cellsMobDataPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf, "JoinBtsNodeToNodeBts",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToNodeBtsReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] { cdrsInfoMobDataPath,
                                                            cellsMobDataPath });
            FileOutputFormat.setOutputPath(job, clientsBtsPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf, "JoinBtsNodeToBtsDayRange",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToBtsDayRangeReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] { cdrsInfoMobDataPath,
                                                            cellsMobDataPath });
            FileOutputFormat.setOutputPath(job, btsCommsPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf, "JoinBtsNodeToCdr",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToCdrReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] { cdrsInfoMobDataPath,
                                                            cellsMobDataPath });
            FileOutputFormat.setOutputPath(job, cdrsNoBtsPath);
            job.waitForCompletion(true);
        }

        {
            ReduceJob job = ReduceJob.create(conf,
                    "JoinBtsNodeToTelMonthAndCell",
                    SequenceFileInputFormat.class,
                    JoinBtsNodeToTelMonthAndCellReducer.class,
                    SequenceFileOutputFormat.class);
            FileInputFormat.setInputPaths(job, new Path[] { cdrsInfoMobDataPath,
                                                            cellsMobDataPath });
            FileOutputFormat.setOutputPath(job, viTelmonthBtsPath);
            job.waitForCompletion(true);
        }
        
        fs.delete(cdrsInfoMobDataPath, true);
        fs.delete(cellsMobDataPath, true);
    }
}
