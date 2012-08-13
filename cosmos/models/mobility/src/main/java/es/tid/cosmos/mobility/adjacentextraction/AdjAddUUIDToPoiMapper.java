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

package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.UUID;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Replaces previous key by the LSB of a UUID.
 *
 * Input:  <TwoInt, Poi>
 * Output: <LongWritable, Poi>
 *
 * @author sortega
 */
class AdjAddUUIDToPoiMapper extends Mapper<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>,
        LongWritable, TypedProtobufWritable<Poi>> {

    private LongWritable id;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.id = new LongWritable();
    }

    @Override
    protected void map(ProtobufWritable<TwoInt> ignoredKey,
            TypedProtobufWritable<Poi> value, Context context)
            throws IOException, InterruptedException {
        this.id.set(UUID.randomUUID().getLeastSignificantBits());
        context.write(this.id, value);
    }
}
