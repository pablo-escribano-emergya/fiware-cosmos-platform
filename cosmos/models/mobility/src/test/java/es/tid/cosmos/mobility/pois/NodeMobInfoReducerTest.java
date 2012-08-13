/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.mobility.pois;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class NodeMobInfoReducerTest {

    private ReduceDriver<LongWritable, TypedProtobufWritable<BtsCounter>,
            LongWritable, TypedProtobufWritable<NodeMxCounter>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<BtsCounter>,
                LongWritable, TypedProtobufWritable<NodeMxCounter>>(
                        new NodeMobInfoReducer());
    }

    @Test
    public void testReduce() throws Exception {
        LongWritable key = new LongWritable(57L);
        TypedProtobufWritable<BtsCounter> btsCounter1 =
                new TypedProtobufWritable<BtsCounter>(BtsCounterUtil
                .create(1L, 2, 3, 4));
        TypedProtobufWritable<BtsCounter> btsCounter2 =
                new TypedProtobufWritable<BtsCounter>(BtsCounterUtil
                .create(5L, 6, 7, 8));
        TypedProtobufWritable<NodeMxCounter> output =
                new TypedProtobufWritable<NodeMxCounter>(NodeMxCounterUtil
                .create(btsCounter1.get(), btsCounter2.get()));
        this.driver
                .withInput(key, asList(btsCounter1, btsCounter2))
                .withOutput(key, output)
                .runTest();
    }
}
