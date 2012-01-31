
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_passive_location_parse_xml_cdrs
#define _H_SAMSON_passive_location_parse_xml_cdrs


#include <samson/module/samson.h>
#include <samson/modules/passive_location/Record.h>
#include <samson/modules/passive_location/CompleteTMSI.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace passive_location{


class parse_xml_cdrs : public samson::Parser
{
    samson::system::UInt user;                              // Used as key at the output
    samson::passive_location::Record record;                // Used as value at the output
    samson::passive_location::CompleteTMSI completeTMSI;    // Used as value at the output
    samson::passive_location::IMSIbyTime imsiTime;    // Used as value at the output


public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
    // If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

    //out system.UInt passive_location.Record                     # Emitted with key = imsi
    //out passive_location.CompleteTMSI passive_location.Record   # Whem imsi = 0, emitted with key = tmsi
    //out system.UInt passive_location.Record                     # If no cellId or LAC is parsed in the record
    //out passive_location.CompleteTMSI passive_location.IMSIbyTime               # Queue to recover imsi from tmsi


    helpLine: Parse input cdrs from Arkanum platform. Note that output key is imsi at the output
#endif // de INFO_COMMENT

    void init( samson::KVWriter *writer )
    {
    }

    ///Function to translate a char
    ///into a integer.
    ///
    ///@param line character to traslate.
    inline int GST_char_to_int(char character)
    {
        return (character -48);
    }

    bool xmlStringToTime(char *strTimeStamp, struct tm *time)
    {
        // TimeStamp format in ARCANUM xmls
        // 2011-07-21T15:44:57
        time->tm_year = 100 + 10*GST_char_to_int(strTimeStamp[2]) + GST_char_to_int(strTimeStamp[3]);
        time->tm_mon = 10*GST_char_to_int(strTimeStamp[5]) + GST_char_to_int(strTimeStamp[6]) - 1;
        time->tm_mday = 10*GST_char_to_int(strTimeStamp[8]) + GST_char_to_int(strTimeStamp[9]);

        time->tm_hour = GST_char_to_int(strTimeStamp[11])*10 + GST_char_to_int(strTimeStamp[12]);
        time->tm_min = GST_char_to_int(strTimeStamp[14])*10 + GST_char_to_int(strTimeStamp[15]);
        time->tm_sec = GST_char_to_int(strTimeStamp[17])*10 + GST_char_to_int(strTimeStamp[18]);

        return(true);
    }

    void parseXML (char *text, samson::KVWriter *writer )
    {
        char *endptr;

        char *p_tag_begin;
        char *p_sep;
        char *p_xml = text;

        struct tm timeExpanded;

        //OLM_T(LMT_User06, ("parseXML called"));

        // Init everything
        record.imsi.value = 0;
        record.imei.value = 0;
        record.msisdn.value = 0;
        record.timestamp.value = 0;
        record.DRType.value = 0;
        record.callType.value = 0;

        uint32_t LAC = 0;
        uint32_t cellIdTmp = 0;
        uint32_t cellID = 0;

        uint64_t tmsi = 0;

        int output_queue = 0;


#define XML_TAG_USERID "<IMSI>"
        if ((p_tag_begin = strstr(p_xml, XML_TAG_USERID)) != NULL)
        {
            p_tag_begin += strlen(XML_TAG_USERID);
            if ((p_sep = strchr(p_tag_begin, '<')) == NULL)
            {
                OLM_E(("xml without IMSI end"));
                return;
            }
            *p_sep = '\0';
            record.imsi.value = strtoul(p_tag_begin, &endptr, 10 );
            if (*endptr != '\0')
            {
                OLM_E(("xml with wrong userId:'%s'", p_tag_begin));
                return;
            }
            p_xml = p_sep+1;
        }
        else
        {
            //OLM_E(("xml without userId"));
        }

#define XML_TAG_PTMSI "<PTMSI>"
        if ((p_tag_begin = strstr(p_xml, XML_TAG_PTMSI)) != NULL)
        {
            p_tag_begin += strlen(XML_TAG_PTMSI);
            if ((p_sep = strchr(p_tag_begin, '<')) == NULL)
            {
                OLM_E(("xml without PTMSI end"));
                return;
            }
            *p_sep = '\0';
            tmsi = strtoul(p_tag_begin, &endptr, 10 );
            if (*endptr != '\0')
            {
                OLM_E(("xml with wrong userId:'%s'", p_tag_begin));
                return;
            }
            p_xml = p_sep+1;
        }
        else
        {
            //OLM_E(("xml without IMEI"));
        }

#define XML_TAG_IMEI "<IMEI>"
        if ((p_tag_begin = strstr(p_xml, XML_TAG_IMEI)) != NULL)
        {
            p_tag_begin += strlen(XML_TAG_IMEI);
            if ((p_sep = strchr(p_tag_begin, '<')) == NULL)
            {
                OLM_E(("xml without IMEI end"));
                return;
            }
            *p_sep = '\0';
            record.imei.value = strtoul(p_tag_begin, &endptr, 10 );
            if (*endptr != '\0')
            {
                OLM_E(("xml with wrong userId:'%s'", p_tag_begin));
                return;
            }
            p_xml = p_sep+1;
        }
        else
        {
            //OLM_E(("xml without IMEI"));
        }

#define XML_TAG_LACID "<LocationArea>"
        if ((p_tag_begin = strstr(p_xml, XML_TAG_LACID)) != NULL)
        {

            p_tag_begin += strlen(XML_TAG_LACID);
            if ((p_sep = strchr(p_tag_begin, '<')) == NULL)
            {
                OLM_E(("xml without LAC end"));
                return;
            }
            *p_sep = '\0';
            LAC = strtoul(p_tag_begin, &endptr, 10 );
            cellIdTmp = (LAC << 16) & 0xffff0000;
            if (*endptr != '\0')
            {
                OLM_E(("xml with wrong LAC:'%s'", p_tag_begin));
                return;
            }
            p_xml = p_sep+1;
        }
        else
        {
            //OLM_E(("xml without XML_TAG_LACID"));
        }

#define XML_TAG_CELLID "<CellID>"
        if ((p_tag_begin = strstr(p_xml, XML_TAG_CELLID)) != NULL)
        {
            p_tag_begin += strlen(XML_TAG_CELLID);
            if ((p_sep = strchr(p_tag_begin, '<')) == NULL)
            {
                OLM_E(("xml without cellId end"));
                return;
            }
            *p_sep = '\0';
            cellID = strtoul(p_tag_begin, &endptr, 10 );
            record.cellId.value = cellIdTmp | cellID;
            if (*endptr != '\0')
            {
                OLM_E(("xml with wrong cellId:'%s'", p_tag_begin));
                return;
            }
            p_xml = p_sep+1;
        }
        else
        {
            //OLM_E(("xml without cellId"));
        }

#define XML_TAG_TIMESTAMP "<Timestamp>"
        if ((p_tag_begin = strstr(p_xml, XML_TAG_TIMESTAMP)) != NULL)
        {


            p_tag_begin += strlen(XML_TAG_TIMESTAMP);
            if ((p_sep = strchr(p_tag_begin, '<')) == NULL)
            {
                OLM_E(("xml without timeStamp end"));
                return;
            }
            *p_sep = '\0';
            if (xmlStringToTime(p_tag_begin, &timeExpanded) == false)
            {
                OLM_E(("xml with wrong timeStamp:'%s'", p_tag_begin));
                return;
            }
            record.timestamp.getTimeUTCFromCalendar(&timeExpanded);
        }
        else
        {
            OLM_E(("xml without timeStamp"));
        }

        completeTMSI.tmsi.value = tmsi;
        completeTMSI.LAC.value = LAC;

        if ((cellIdTmp == 0) || (cellID == 0))
        {
            writer->emit(2, &user, &record);
        }
        else if (record.imei.value == 0)
        {
            writer->emit(1, &completeTMSI, &record);
        }
        else
        {
            writer->emit(0, &user, &record);
        }

        if ((user.value != 0) && (LAC != 0))
        {
            imsiTime.imsi.value = user.value;
            imsiTime.timestamp.value = record.timestamp.value;
            writer->emit(3, &completeTMSI, &imsiTime);
        }


        // Emit the record at the output
        writer->emit(output_queue, &user, &record);

        return;

    }

    void run( char *data , size_t length , samson::KVWriter *writer )
    {
        size_t offset = 0;

        char *p_xml_begin = data;
        char *p_xml_end;

        char *p_xml_doc;

#define XML_BEGIN "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
#define XML_END "</ns0:AMRReport>"

        while( offset < length )
        {
            if (strncmp(p_xml_begin, XML_BEGIN, strlen(XML_BEGIN)))
            {
                OLM_E(("Error, bad formed XML (begin) starting at pos offset:%lu, (%s)", offset, p_xml_begin));
                return;
            }
            if ((p_xml_end = strstr(p_xml_begin, XML_END)) == NULL)
            {
                OLM_E(("Error, bad formed XML (end) starting at pos offset:%d, (%s)", offset, p_xml_begin));
                return;
            }

            p_xml_doc = strndup(p_xml_begin, (p_xml_end - p_xml_begin));
            p_xml_end[strlen(XML_END)] = '\0';

            parseXML (p_xml_begin, writer);

            p_xml_begin = p_xml_end + strlen(XML_END) + 1;
            offset = p_xml_begin - data;
        }
    }

    void finish( samson::KVWriter *writer )
    {
    }

        

};


} // end of namespace passive_location
} // end of namespace samson

#endif
