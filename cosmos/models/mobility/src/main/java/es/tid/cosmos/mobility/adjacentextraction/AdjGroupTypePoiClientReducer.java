package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjGroupTypePoiClientReducer extends Reducer<LongWritable,
        ProtobufWritable<PoiNew>, ProtobufWritable<TwoInt>,
        ProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<PoiNew>> values, Context context)
            throws IOException, InterruptedException {
        List<PoiNew> poiNewList = new LinkedList<PoiNew>();
        for (ProtobufWritable<PoiNew> value : values) {
            value.setConverter(PoiNew.class);
            poiNewList.add(value.get());
        }
        
        for (PoiNew curPoi : poiNewList) {
            for (PoiNew tempPoi : poiNewList) {
                if (curPoi.getId() < tempPoi.getId()) {
                    context.write(TwoIntUtil.createAndWrap(curPoi.getBts(),
                                                           tempPoi.getBts()),
                                  TwoIntUtil.createAndWrap(curPoi.getId(),
                                                           tempPoi.getId()));
                }
            }
        }
    }
}
