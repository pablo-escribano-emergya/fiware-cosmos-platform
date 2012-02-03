
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_passive_location_parse_tek_drs
#define _H_SAMSON_passive_location_parse_tek_drs


#include <arpa/inet.h> // ntohl(), ntohs()
#include <samson/module/samson.h>
#include <samson/modules/passive_location/Record.h>
#include <samson/modules/passive_location/CompleteTMSI.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>

#include "logMsg/logMsg.h"

#include "tektronix_data.h"

#include <iostream>
#include <sstream>
#include <string>


namespace samson{
namespace passive_location{


class parse_tek_drs : public samson::Parser
{

    samson::system::UInt user;                              // Used as key at the output
    samson::passive_location::Record record;                // Used as value at the output
    samson::passive_location::CompleteTMSI completeTMSI;    // Used as value at the output
    samson::passive_location::IMSIbyTime imsiTime;    // Used as value at the output
    samson::passive_location::CompleteTMSI oldTMSI;    // Used as value at the output
    samson::passive_location::CompleteTMSI newTMSI;    // Used as value at the output

    samson::system::String infoKey;   // Info key
    samson::system::UInt infoValue;   // Info value

public:


    //  INFO_MODULE
    // If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
    // Please, do not remove this comments, as it will be used to check consistency on module declaration
    //
    // out system.UInt passive_location.Record                     # Emitted with key = imsi
    // out passive_location.CompleteTMSI passive_location.Record   # Whem imsi = 0, emitted with key = tmsi
    // out system.UInt passive_location.Record                     # If no cellId or LAC is parsed in the record
    // out passive_location.CompleteTMSI passive_location.IMSIbyTime               # Queue to recover imsi from tmsi
    // out passive_location.CompleteTMSI passive_location.CompleteTMSI              # Queue to update tmsi
    // out system.String system.UInt                               # Queue for general info

    //
    // helpLine: Parse input binary OHDRs from Tektroniks platform (probes). Note that output key is cellid
    //  END_INFO_MODULE

    void init( samson::KVWriter *writer )
    {
    }

    void run( char *data , size_t length , samson::KVWriter *writer )
    {
        //size_t offset = 0;

        LM_M(("Parsing length:%lu", length));

        unsigned char *p_blob = (unsigned char *)data;
        unsigned char *p_end_blob = (unsigned char *)data + length;

        unsigned char *p_init_ohdr;
        unsigned char *p_end_ohdr;

        unsigned int sizeOHDR = 0;
        int typeMsg = 0;
        int numDRs = 0;

        unsigned int sizeDR = 0;
        struct struct_tek_record tek_record;



        //        for (int i = 0; (i < 32); i++)
        //        {
        //            LM_M(("data[%d](0x%0x) = 0x%0x", i, p_blob+i, int(p_blob[i])));
        //        }

        while( p_blob < p_end_blob )
        {
            //            for (int i = 0; (i < 16); i++)
            //            {
            //                LM_M(("p_blob[%d](0x%0x) = 0x%0x", i, p_blob+i, int(p_blob[i])));
            //            }

            p_init_ohdr = p_blob;
            if (parse_OHDR_header(&p_blob, &sizeOHDR, &numDRs, &typeMsg))
            {
                p_end_ohdr = p_init_ohdr + sizeOHDR;
                //LM_M(("Parsing DRs in OHDR of size:%d", sizeOHDR));
                for (int i = 0; ((i < numDRs) && (p_blob < p_end_ohdr)); i++)
                {
                    init_tek_record(&tek_record);
                    //LM_M(("Parsing DR:%d of numDRs:%d", i, numDRs));
                    if (parse_DR(&p_blob, &sizeDR, &tek_record))
                    {

                        if (tek_record.last_tmsi != 0)
                        {
                            completeTMSI.tmsi.value = tek_record.last_tmsi;
                        }
                        else
                        {
                            completeTMSI.tmsi.value = tek_record.tmsi;
                        }
                        completeTMSI.LAC.value = tek_record.LAC;

                        user.value = tek_record.imsi;

                        record.imsi.value = tek_record.imsi;
                        record.imei.value = tek_record.imei;
                        record.timestamp.value = tek_record.timestamp;
                        // We compose location id with ((LAC << 16) | cell_id), in a uint32_t field
                        uint32_t cellIdTmp = tek_record.LAC;
                        record.cellId.value = (cellIdTmp << 16) | tek_record.cellID;
                        record.callType.value = tek_record.callType;
                        record.DRType.value = tek_record.typeDR;

                        //LM_M(("Ready to emit typeDR:%d for callNumber:%d callType:0x%0x imsi:%lu tmsi:%lu imei:%lu at cellId:%d in LAC:%d (compose:%lu 0x%0x) at %lu(%s)", tek_record.typeDR, tek_record.callNumber, tek_record.callType, tek_record.imsi, tek_record.tmsi, tek_record.imei, tek_record.cellID, tek_record.LAC, record.cellId.value, record.cellId.value, record.timestamp.value, record.timestamp.str().c_str()));

                        // Emitting infos
                        {
                            std::stringstream ss;//create a stringstream

                            infoValue.value = 1;
                            ss << tek_record.callType;
                            infoKey.value = "callType " + ss.str();
                            writer->emit(5, &infoKey, &infoValue);

                            ss << tek_record.typeDR;
                            infoKey.value = "typeDR " + ss.str();
                            writer->emit(5, &infoKey, &infoValue);

                            ss << tek_record.LAC;
                            infoKey.value = "LAC " + ss.str();
                            writer->emit(5, &infoKey, &infoValue);

                            ss << record.cellId.value;
                            infoKey.value = "cellId " + ss.str();
                            writer->emit(5, &infoKey, &infoValue);

                            if (tek_record.imsi != 0)
                            {
                                ss << tek_record.imsi;
                                infoKey.value = "imsi " + ss.str();
                                writer->emit(5, &infoKey, &infoValue);
                            }


                        }


                        // Emit the record at the corresponding output
                        if ((tek_record.cellID == 0) || (tek_record.LAC == 0))
                        {
                            LM_W(("Emit records without cellId: typeDR:%d for callNumber:%d callType:0x%0x imsi:%lu imei:%lu at cellId:%d in LAC:%d (compose:%lu 0x%0x) at %lu(%s)", tek_record.typeDR, tek_record.callNumber, tek_record.callType, tek_record.imsi, tek_record.imei, tek_record.cellID, tek_record.LAC, record.cellId.value, record.cellId.value, record.timestamp.value, record.timestamp.str().c_str()));
                            writer->emit(2, &user, &record);
                        }
                        else if (tek_record.imsi == 0)
                        {
                            //LM_W(("Emit records without imsi: typeDR:%d for callNumber:%d callType:0x%0x imsi:%lu tmsi:%lu imei:%lu at cellId:%d in LAC:%d (compose:%lu 0x%0x) at %lu(%s)", tek_record.typeDR, tek_record.callNumber, tek_record.callType, tek_record.imsi, tek_record.tmsi, tek_record.imei, tek_record.cellID, tek_record.LAC, record.cellId.value, record.cellId.value, record.timestamp.value, record.timestamp.str().c_str()));

                            writer->emit(1, &completeTMSI, &record);
                        }
                        else
                        {
                            //LM_W(("Emit complete records: typeDR:%d for callNumber:%d callType:0x%0x imsi:%lu imei:%lu at cellId:%d in LAC:%d (compose:%lu 0x%0x) at %lu(%s)", tek_record.typeDR, tek_record.callNumber, tek_record.callType, tek_record.imsi, tek_record.imei, tek_record.cellID, tek_record.LAC, record.cellId.value, record.cellId.value, record.timestamp.value, record.timestamp.str().c_str()));
                            writer->emit(0, &user, &record);
                        }

                        if ((tek_record.imsi != 0) && (tek_record.LAC != 0))
                        {
                            imsiTime.imsi.value = user.value;
                            imsiTime.timestamp.value = tek_record.timestamp;
                            //LM_M(("Emit new TMSI-IMSI association"));

                            writer->emit(3, &completeTMSI, &imsiTime);
                        }
                        else if ((tek_record.last_tmsi != 0) && (tek_record.tmsi != 0))
                        {
                            //LM_M(("Ready to emit TMSI update: old:%lu, new:%lu", tek_record.tmsi, tek_record.last_tmsi));
                            oldTMSI.tmsi.value = tek_record.tmsi;
                            oldTMSI.LAC.value = tek_record.LAC;
                            newTMSI.tmsi.value = tek_record.last_tmsi;
                            newTMSI.LAC.value = tek_record.LAC;

                            writer->emit(4, &oldTMSI, &newTMSI);
                        }
                    }
                    else
                    {
                        LM_W(("Alignment failed in DR %d of %d DRs", i, numDRs));
                    }
                }
                if (p_blob != p_end_ohdr)
                {
                    LM_W(("Alignment failed in a OHDR of size:%d, numDRs:%d", sizeOHDR, numDRs));
                }
            }
            else
            {
                LM_W(("OHDR ignored because not valid header, with typeMsg=%d, at pos:%lu of length:%lu", typeMsg, p_init_ohdr - (unsigned char *)data, length));
            }
            //OLM_M(("p_end_blob - p_blob=%lu (length(%lu))", p_end_blob - p_blob, length));
        }
    }

    void finish( samson::KVWriter *writer )
    {
    }



};


} // end of namespace passive_location
} // end of namespace samson

#endif
