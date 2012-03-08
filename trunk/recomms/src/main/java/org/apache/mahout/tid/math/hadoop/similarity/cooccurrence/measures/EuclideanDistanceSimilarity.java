/**
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.apache.mahout.tid.math.hadoop.similarity.cooccurrence.measures;

import org.apache.mahout.math.Vector;
import org.apache.mahout.tid.math.hadoop.similarity.cooccurrence.measures.VectorSimilarityMeasure;

import java.util.Iterator;

public class EuclideanDistanceSimilarity implements VectorSimilarityMeasure {
    @Override
    public Vector normalize(Vector vector) {
        return vector;
    }

    @Override
    public double norm(Vector vector) {
        double norm = 0;
        Iterator<Vector.Element> nonZeroElements = vector.iterateNonZero();
        while (nonZeroElements.hasNext()) {
            double value = nonZeroElements.next().get();
            norm += value * value;
        }
        return norm;
    }

    @Override
    public double aggregate(double valueA, double nonZeroValueB) {
        return valueA * nonZeroValueB;
    }

    @Override
    public double similarity(double dots, double normA, double normB,
            int numberOfColumns) {
        double euclideanDistance = Math.sqrt(normA - 2 * dots + normB);
        return 1.0 / (1.0 + euclideanDistance);
    }

    @Override
    public boolean consider(int numNonZeroEntriesA, int numNonZeroEntriesB,
            double maxValueA, double maxValueB, double threshold) {
        return true;
    }
}
