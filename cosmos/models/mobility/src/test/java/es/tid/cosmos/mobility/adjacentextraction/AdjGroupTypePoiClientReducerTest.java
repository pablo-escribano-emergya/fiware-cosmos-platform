package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjGroupTypePoiClientReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>>(new AdjGroupTypePoiClientReducer());
    }
    
    @Test
    public void testSomeMethod() throws IOException {
        PoiNew pn1 = PoiNewUtil.create(1, 2L, 3L, 4, 1);
        PoiNew pn2 = PoiNewUtil.create(5, 6L, 7L, 8, 0);
        List<Pair<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>>> res =
                this.driver
                        .withInput(TwoIntUtil.createAndWrap(1, 2),
                                   asList(MobDataUtil.createAndWrap(pn1),
                                          MobDataUtil.createAndWrap(pn2)))
                .run();
        assertEquals(1, res.size());
        final ProtobufWritable<TwoInt> keyWrapper = res.get(0).getFirst();
        final ProtobufWritable<MobData> valueWrapper = res.get(0).getSecond();
        keyWrapper.setConverter(TwoInt.class);
        valueWrapper.setConverter(MobData.class);
        final TwoInt key = keyWrapper.get();
        final TwoInt value = valueWrapper.get().getTwoInt();
        assertEquals(3L, key.getNum1());
        assertEquals(7L, key.getNum2());
        assertEquals(1L, value.getNum1());
        assertEquals(5L, value.getNum2());
    }
}
