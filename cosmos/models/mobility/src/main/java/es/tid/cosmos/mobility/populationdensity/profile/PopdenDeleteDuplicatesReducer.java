package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;

/**
 *
 * @author ximo
 */
public class PopdenDeleteDuplicatesReducer extends Reducer<
        ProtobufWritable<MobData>, NullWritable, LongWritable,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<MobData> key,
            Iterable<NullWritable> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(MobData.class);
        final MobData mobData = key.get();
        final NodeBtsDate nodeBtsDate = mobData.getNodeBtsDate();
        context.write(
                new LongWritable(nodeBtsDate.getUserId()),
                MobDataUtil.createAndWrap(NodeBtsUtil.create(
                        nodeBtsDate.getUserId(),
                        nodeBtsDate.getBts(),
                        nodeBtsDate.getDate().getWeekday(),
                        nodeBtsDate.getHour())));
    }
}
