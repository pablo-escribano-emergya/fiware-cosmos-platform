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

package es.tid.cosmos.kpicalculation;

import java.io.IOException;

import org.apache.hadoop.io.DataInputBuffer;
import org.apache.hadoop.io.RawComparator;

import es.tid.cosmos.base.mapreduce.CompositeKey;
import es.tid.cosmos.kpicalculation.data.KpiCalculationComparationException;
import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;

/**
 * Class used in the compare and sort phases of process for simple aggregation
 * operations. This class is used to compare items of type WebLogCounter.
 *
 * Note: Ideally this class should be used in a general way to use subclasses of
 * WebLog through casting operations. At this moment, and due to the flow of
 * hadoop map & reduce process, it is not possible to set a dynamic type to the
 * comparation objects so we have used to different comparator classes
 *
 * @author javierb
 */
public class PageViewKpiCounterComparator implements
        RawComparator<CompositeKey> {
    private final DataInputBuffer buffer;

    public PageViewKpiCounterComparator() {
        this.buffer = new DataInputBuffer();
    }

    /*
     * (non-Javadoc)
     *
     * @see org.apache.hadoop.io.RawComparator#compare(byte[], int, int, byte[],
     * int, int)
     */
    @Override
    public int compare(byte[] b1, int s1, int l1, byte[] b2, int s2, int l2) {
        try {
            CompositeKey o1 = new CompositeKey(1);
            CompositeKey o2 = new CompositeKey(1);
            this.buffer.reset(b1, s1, l1);
            o1.readFields(buffer);
            this.buffer.reset(b2, s2, l2);
            o2.readFields(buffer);
            return compare(o1, o2);
        } catch (IOException e) {
            throw new KpiCalculationComparationException(e.getMessage(), e,
                    KpiCalculationCounter.COMPARATION_ERROR);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see java.util.Comparator#compare(java.lang.Object, java.lang.Object)
     */
    @Override
    public int compare(CompositeKey o1, CompositeKey o2) {
        return o1.compareTo(o2);
    }
}