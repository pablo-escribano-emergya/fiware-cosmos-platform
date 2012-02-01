package es.tid.ps.profile.userprofile;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.*;
import org.apache.hadoop.io.Writable;

/**
 * User profile: frequencies by category for a given user.
 *
 * @author sortega@tid.es
 */
public class UserProfile implements Writable {
    private String userId;
    private Map<String, Long> counts;

    public void setUserId(String userId) {
        this.userId = userId;
        this.counts = new HashMap<String, Long>();
    }

    public void add(CategoryCount categoryCount) {
        Long count = this.counts.get(categoryCount.getCategory());
        if (count == null) {
            count = 0l;
        }
        this.counts.put(categoryCount.getCategory(),
                count + categoryCount.getCount());
    }

    public long getCount(CategoryCount categoryCount) {
        Long count = this.counts.get(categoryCount.getCategory());
        if (count == null) {
            return 0l;
        }
        return count;
    }

    public long getTotalCount() {
        long total = 0l;
        for (Long count : this.counts.values()) {
            total += count;
        }
        return total;
    }

    public long getCount(String categoryName) {
        Long count = this.counts.get(categoryName);
        return (count == null) ? 0l : count;
    }

    @Override
    public void write(DataOutput output) throws IOException {
        ArrayList<Map.Entry<String, Long>> entries =
                new ArrayList<Map.Entry<String, Long>>(this.counts.entrySet());

        Collections.sort(entries, new Comparator<Map.Entry<String, Long>>() {
            @Override
            public int compare(Map.Entry<String, Long> left,
                    Map.Entry<String, Long> right) {
                return left.getValue().compareTo(right.getValue());
            }
        });

        output.writeUTF(this.userId);
        output.writeInt(entries.size());
        for (Map.Entry<String, Long> entry : entries) {
            output.writeUTF(entry.getKey());
            output.writeLong(entry.getValue().longValue());
        }
    }

    @Override
    public void readFields(DataInput input) throws IOException {
        this.userId = input.readUTF();
        this.counts = new HashMap<String, Long>();
        int entries = input.readInt();
        for (int i = 0; i < entries; i++) {
            String category = input.readUTF();
            long count = input.readLong();
            this.counts.put(category, count);
        }
    }
}