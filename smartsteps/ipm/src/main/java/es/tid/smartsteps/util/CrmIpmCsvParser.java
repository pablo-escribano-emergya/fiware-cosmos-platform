package es.tid.smartsteps.util;

import java.io.InputStream;
import java.nio.charset.Charset;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm;

/**
*/
public class CrmIpmCsvParser extends CsvParserSupport<CrmIpm, CrmIpm.Builder> {

    private static final FieldParser[] FIELD_PARSERS = {
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAnonymisedMsisdn(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAnonymisedBillingPostCode(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAcornCode(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setBillingPostCodePrefix(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setGender(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setBillingSystem(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setMtrcPlSegment(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setMpnStatus(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setSpid(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setActiveStatus(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setNeedsSegmentation(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAge(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAgeBand(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAnonymisedImsi(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setAnonymisedImei(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setImeiTac(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setDeviceType(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setDeviceManufacturer(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setDeviceModelName(fieldValue);
                }
            },
            new FieldParser<CrmIpm.Builder>() {
                @Override
                public void parseField(String fieldValue,
                                       CrmIpm.Builder builder) {
                    builder.setEffectiveFromDate(fieldValue);
                }
            },
    };

    public CrmIpmCsvParser(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    public CrmIpm parse(InputStream input) throws ParseException {
        CrmIpm.Builder builder = CrmIpm.newBuilder();
        this.parse(input, builder, FIELD_PARSERS);
        return builder.build();
    }
}
