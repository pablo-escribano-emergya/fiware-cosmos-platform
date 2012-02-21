package es.tid.ps.mobility.mapreduce;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.mobility.data.BtsCounterUtil;
import es.tid.ps.mobility.data.MxProtocol.BtsCounter;
import es.tid.ps.mobility.data.MxProtocol.TwoInt;
import es.tid.ps.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class MobmxRepbtsGetRepresentativeBtsReducer extends Reducer<IntWritable,
        ProtobufWritable<BtsCounter>, ProtobufWritable<TwoInt>,
        ProtobufWritable<BtsCounter>> {
    private static final int MOB_CONF_MIN_PERC_REP_BTS = 5;
    private static final int MOB_CONF_MIN_NUMBER_CALLS_BTS = 14;
    
    @Override
    public void reduce(IntWritable key,
            Iterable<ProtobufWritable<BtsCounter>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<BtsCounter> value : values) {
            final BtsCounter counter = value.get();
            if (counter.getCount() >= MOB_CONF_MIN_PERC_REP_BTS
                    && counter.getRange() >= MOB_CONF_MIN_NUMBER_CALLS_BTS) {
                ProtobufWritable<TwoInt> nodeBtsWrapper =
                        TwoIntUtil.createAndWrap(key.get(), counter.getBts());
                ProtobufWritable<BtsCounter> output =
                        BtsCounterUtil.createAndWrap(counter.getBts(), 0, 0,
                                                     counter.getCount());
                context.write(nodeBtsWrapper, output);
            }
        }
    }
}
