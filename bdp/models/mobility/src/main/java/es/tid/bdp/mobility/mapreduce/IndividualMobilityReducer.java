package es.tid.bdp.mobility.mapreduce;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.bdp.mobility.data.Client;
import es.tid.bdp.mobility.data.MobProtocol.GLEvent;

public class IndividualMobilityReducer extends Reducer<LongWritable,
        ProtobufWritable<GLEvent>, LongWritable, Client> {
    @Override
    protected void reduce(LongWritable key,
                          Iterable<ProtobufWritable<GLEvent>> values,
                          Context context) throws IOException,
                                                  InterruptedException {
        Client client = new Client();
        client.setUserId(key.get());
        List<GLEvent> glEvents = new ArrayList<GLEvent>();
        for (ProtobufWritable<GLEvent> wrapper : values) {
            wrapper.setConverter(GLEvent.class);
            glEvents.add(wrapper.get());
        }
        client.calculateGeoLocations(glEvents.iterator());
        client.calculateNodeCommVector();
        client.calculatePoiCommVector();

        context.write(key, client);
    }
}
