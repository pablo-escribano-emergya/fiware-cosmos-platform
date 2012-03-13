package es.tid.bdp.profile.categoryextraction;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.HashMap;
import java.util.Map;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.bdp.base.mapreduce.BinaryKey;
import es.tid.bdp.profile.data.ProfileProtocol.CategoryInformation;
import es.tid.bdp.profile.data.ProfileProtocol.UserNavigation;
import es.tid.bdp.profile.dictionary.Categorization;
import es.tid.bdp.profile.dictionary.Dictionary;
import es.tid.bdp.profile.dictionary.comscore.DistributedCacheDictionary;

/*
 * Enum with the list of counters to use in the CategoryExtraction mapreduces.
 *
 * @author dmicol, sortega
 */
public class CategoryExtractionReducer extends Reducer<BinaryKey,
        ProtobufWritable<UserNavigation>, BinaryKey,
        ProtobufWritable<CategoryInformation>> {

    private static Dictionary sharedDictionary = null;
    private ProtobufWritable<CategoryInformation> catWrapper;

    @Override
    public void setup(Context context) throws IOException {
        this.setupDictionary(context);
        this.catWrapper = new ProtobufWritable<CategoryInformation>();
        this.catWrapper.setConverter(CategoryInformation.class);
    }

    protected void setupDictionary(Context context) throws IOException {
        if (sharedDictionary == null) {
            sharedDictionary = DistributedCacheDictionary
                    .loadFromCache(context);
        }
    }

    @Override
    protected void reduce(BinaryKey key,
            Iterable<ProtobufWritable<UserNavigation>> values, Context context)
            throws IOException, InterruptedException {
        Map<String, Long> uniqueUrlCounts = this.getUniqueUrlCounts(values);
        for (String url : uniqueUrlCounts.keySet()) {
            long urlInstances = uniqueUrlCounts.get(url);
            Categorization dictionaryResponse = this.categorize(url);

            switch (dictionaryResponse.getResult()) {
                case KNOWN_URL:
                    context.getCounter(CategoryExtractionCounter.KNOWN_VISITS).
                            increment(urlInstances);

                    this.catWrapper.set(CategoryInformation.newBuilder()
                            .setUserId(key.getPrimaryKey())
                            .setUrl(url)
                            .setDate(key.getSecondaryKey())
                            .setCount(urlInstances)
                            .addAllCategories(
                                asList(dictionaryResponse.getCategories()))
                            .build());
                    context.write(key, this.catWrapper);
                    break;

                case IRRELEVANT_URL:
                    context.getCounter(
                            CategoryExtractionCounter.IRRELEVANT_VISITS).
                            increment(urlInstances);
                    break;

                case UNKNOWN_URL:
                    // TODO: do something smart for URL category extraction.
                    context.getCounter(
                            CategoryExtractionCounter.UNKNOWN_VISITS).
                            increment(urlInstances);
                    break;

                case GENERIC_FAILURE:
                    context.getCounter(
                            CategoryExtractionCounter.UNPROCESSED_VISITS).
                            increment(urlInstances);
                    break;

                default:
                    throw new IllegalStateException(
                            "Invalid dictionary response.");
            }
        }
    }

    protected Categorization categorize(String url) {
        return sharedDictionary.categorize(url);
    }

    private Map<String, Long> getUniqueUrlCounts(
            Iterable<ProtobufWritable<UserNavigation>> wrappedValues) {
        Map<String, Long> uniqueUrlCounts = new HashMap<String, Long>();
        for (ProtobufWritable<UserNavigation> wrapper : wrappedValues) {
            wrapper.setConverter(UserNavigation.class);
            UserNavigation nav = wrapper.get();
            Long count;
            if (uniqueUrlCounts.containsKey(nav.getUrl())) {
                count = uniqueUrlCounts.get(nav.getUrl());
            } else {
                count = new Long(0L);
            }
            uniqueUrlCounts.put(nav.getUrl(), count + 1L);
        }
        return uniqueUrlCounts;
    }
}