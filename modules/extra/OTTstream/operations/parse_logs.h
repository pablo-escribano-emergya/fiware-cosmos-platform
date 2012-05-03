/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_OTTstream_parse_logs
#define _H_SAMSON_OTTstream_parse_logs

#include <samson/module/samson.h>
#include <samson/modules/OTTstream/ServiceHit.h>
#include <samson/modules/OTTstream/UserHit.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/SimpleParser.h>

#include "OTTService.h"

#include "logMsg/logMsg.h"

namespace samson
{
namespace OTTstream
{

class parse_logs: public samson::system::SimpleParser
{

  bool (parse_logs::*_parseFunction)(char*line, samson::system::UInt *userId, ServiceHit *hit, samson::system::String *url);

  std::vector<char*> fields;
  char sep;

  std::string source;
  std::vector<OTTService*> services;

  samson::system::UInt userId;
  samson::system::UInt serviceId;
  samson::system::TimeUnix timestamp;
  samson::system::String url;
  samson::system::UInt count;
  samson::OTTstream::ServiceHit service_hit;
  samson::OTTstream::UserHit user_hit;
  samson::system::String command;

public:

  //  INFO_MODULE
  // If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
  // Please, do not remove this comments, as it will be used to check consistency on module declaration
  //
  //  output: system.UInt OTTstream.ServiceHit
  //  output: system.UInt OTTstream.UserHit
  //  output: system.UInt system.String
  // output: system.String system.UInt
  //
  // helpLine: parse input logs from http, and emit service use with userId as key
  //  END_INFO_MODULE

  uint64_t phone2number(char *field, int max_client)
  {
    int _length_client = 0;
    int _pos = 0;
    char *_endptr;
    uint64_t phone = 0;

    _length_client = strlen(field);

    if (_length_client > max_client)
    {
      _pos = _length_client - max_client;
    }
    else
    {
      _pos = 0;
    }

    phone = strtoul(&(field[_pos]), &_endptr, 10);

    if (strcmp("", _endptr) != 0)
    {
      phone = 0L;
    }

    return phone;
  }

  uint64_t classify_http(const char *url, const char *domain)
  {
    for (unsigned int i = 0; (i < services.size()); i++)
    {
      if (services[i]->checkHTTP(url, domain))
      {
        //LM_M(("url:'%s', domain:'%s' identified as service:'%s' by category:%d", url, domain, services[i]->name.c_str(), services[i]->category));
        return services[i]->serviceId;
      }
    }
    //LM_M(("unknow url:'%s', domain:'%s'", url, domain));
    return 0;
  }

  uint64_t classify_dns(const char *url)
  {
    for (unsigned int i = 0; (i < services.size()); i++)
    {
      if (services[i]->checkDNS(url))
      {
        //LM_M(("url:'%s' identified as service:'%s'", url, services[i]->name.c_str()));
        return services[i]->serviceId;
      }
    }
    //LM_M(("unknow url:'%s'", url));
    return 0;
  }

  uint64_t classify_name(const char *name)
  {
    for (unsigned int i = 0; (i < services.size()); i++)
    {
      if (services[i]->checkName(name))
      {
        return services[i]->serviceId;
      }
    }
    //LM_M(("unknow name:'%s'", name));
    return 0;
  }

  bool parseLine_HTTP(char * line, samson::system::UInt *userId, ServiceHit *hit, samson::system::String *url)
  {
    int maxDigits = 9;

    /*
    --- HTTP ---
    MOBILE_IP_ADDRESS       88.31.48.46
    REFERER                 http://www.facebook.com/messages/?action=read&tid=
    HOST                    www.facebook.com
    CAUSE_CODE              200
    REPORT_REASON           71
    SERVICE_IP_ADDRESS      69.171.242.39
    MSISDN                  34682934158
    MESSAGING_END_TIME      03-NOV-11 10.59.37.680000 PM
    FIRST_URL               http://www.facebook.com/ajax/chat/send.php?__a=1
    MESSAGING_START_TIME    03-NOV-11 10.59.12.390000 PM
    GET_MESSAGES            1
    IMEI                    8698780025156101
    IMSI                    214075500570676
    SERVICE_PORT            80
    REPORT_TIME             2011-11_03 23:00:82800
    APPLICATION_ID          80
     */

    userId->value = 0;
    hit->serviceId.value = 0;

     char *p_url;
    const char *p_domain;

    split_in_words(line, fields, sep);

    if (fields.size() != 16)
    {
      return false;
    }
    userId->value = phone2number(fields[6], maxDigits);

    if (userId->value == 0)
    {
      return false;
    }

    p_domain = fields[2];
    p_url = fields[8];

    hit->serviceId.value = classify_http(p_url, p_domain);

    if (hit->serviceId.value == 0)
    {
      //return false;
    }

    char *p_blank;
    if ((p_blank = strchr(p_url, ' ')) != NULL)
    {
      *p_blank = '\0';
    }
    url->value = p_url;
    hit->timestamp.setFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[9]);
    //LM_M(("input_time:'%s', timeUnix:%lu, '%s'", fields[9], hit->timestamp.value, hit->timestamp.str().c_str()));

    return true;

  }

  bool parseLine_DNS(char * line, samson::system::UInt *userId, ServiceHit *hit, samson::system::String *url)
  {
    int maxDigits = 9;

    /*
  REPORT_TIME
  IMEI
  IMSI
  MSISDN
  MESSAGING_START_TIME
  MESSAGING_END_TIME
  MOBILE_IP_ADDRESS
  SERVICE_IP_ADDRESS
  SERVICE_PORT
  DNS_QUESTION_NAME
  DNS_CAUSE_CODE
     */

    userId->value = 0;
    hit->serviceId.value = 0;

    const char *p_domain;

    split_in_words(line, fields, sep);

    if (fields.size() != 11)
    {
      return false;
    }
    userId->value = phone2number(fields[3], maxDigits);

    if (userId->value == 0)
    {
      return false;
    }

    p_domain = fields[9];

    hit->serviceId.value = classify_dns(p_domain);

    if (hit->serviceId.value == 0)
    {
      //return false;
    }

    hit->timestamp.setFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[4]);

    return true;

  }

  bool parseLine_SMS(char * line, samson::system::UInt *userId, ServiceHit *hit, samson::system::String *url)
  {
    /*
     --- SMS Type 1 (RECIVE SMS?) ----
     SMS_REPORT_TIME           2011-11_01 03:00:10847
     REPORT_REASON             1
     SMS_IMEI                  3534770414641000
     BSSMAP_CAUSE              9
     RANAP_CAUSE
     SCCP_RELEASE_CAUSE
     SMS_CALLED_NUMBER         34685295917
     SMS_CALLING_NUMBER        NULL
     CP_CAUSE
     RP_CAUSE
     TP_CAUSE
     SMS_LENGTH                60
     SMS_INCOMING_TIME         01-NOV-11 03.01.27.000000 AM
     MO_MSG_ACK_TIME           01-NOV-11 03.01.29.270000 AM
     SERVICE_CENTRE_TIMESTAMP  NULL
     SMS_DELIVERY_TIME         NULL
     SMS_MSISDN_NUMBER         34626206656
     SMS_IMSI                  214075519117257

     --- SMS Type 2 (SEND SMS?) ----
     SMS_REPORT_TIME           2011-11_01 03:01:10894
     REPORT_REASON             2
     SMS_IMEI                  3543160453991420
     BSSMAP_CAUSE
     RANAP_CAUSE               83
     SCCP_RELEASE_CAUSE        3
     SMS_CALLED_NUMBER         NULL
     SMS_CALLING_NUMBER        34691701155
     CP_CAUSE
     RP_CAUSE
     TP_CAUSE
     SMS_LENGTH                92
     SMS_INCOMING_TIME         NULL
     MO_MSG_ACK_TIME           NULL
     SERVICE_CENTRE_TIMESTAMP  01-NOV-11 03.01.27.000000 AM
     SMS_DELIVERY_TIME         01-NOV-11 03.01.29.270000 AM
     SMS_MSISDN_NUMBER         34646746738
     SMS_IMSI                  214071613854137
     */

    int maxDigits = 9;
    char *_endptr = NULL;
    int type = 0;

    split_in_words(line, fields, sep);

    if (fields.size() != 18)
    {
      return false;
    }
    type = strtol(fields[1], &_endptr, 10);

    if (type == 1)
    {
      userId->value = phone2number(fields[16], maxDigits);
      hit->timestamp.setFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[12]);
    }
    else if (type == 2)
    {
      userId->value = phone2number(fields[7], maxDigits);
      hit->timestamp.setFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[14]);
    }
    else
    {
      return false;
    }

    hit->serviceId.value = 1100; //SMS
    return true;

  }

  bool parseLine_voice(char * line, samson::system::UInt *userId, ServiceHit *hit, samson::system::String *url)
  {
    /*
     REPORT_REASON
     REPORT_TIME
     A_CATEGORY
     B_CATEGORY
     ROAMING_NUMBER
     A_CALL_LENGTH
     A_TOTAL_CALL_LENGTH
     B_CALL_LENGTH
     B_TSL
     A_TSL
     CALLING_NUMBEER
     A_MSISDN
     A_DIRECTION_NUMBER
     A_IMEI
     A_IMSI
     DIALLED_DIGITS
     B_MSISDN
     B_DIRECTION_NUMBER
     B_IMEI
     A_CC_CAUSE
     A_ISUP_CAUSE_CODE
     A_BSSMAP_CAUSE
     A_RANAP_CAUSE_TYPE
     A_RAB_CAUSE
     A_HANDOVER_REASON
     B_MM_CAUSE
     B_CC_CAUSE
     B_ISUP_CAUSE_CODE
     B_BSSMAP_CAUSE
     B_RANAP_CAUSE_TYPE
     B_RAB_CAUSE
     B_RAB_CAUSE_TYPE
     B_HANDOVER_REASON
     VOICE_CALL_DURATION_SIDE_A
     VOICE_CALL_DURATION_SIDE_B
     */

    int maxDigits = 9;

    split_in_words(line, fields, sep);

    if (fields.size() != 35)
    {
      return false;
    }

    userId->value = phone2number(fields[10], maxDigits);
    hit->timestamp.setFromStrTimeDate_YYYY_mm_dd_24H(fields[1]);
    //LM_M(("input_time:'%s', timeUnix:%lu, '%s'", fields[1], hit->timestamp.value, hit->timestamp.str().c_str()));

    hit->serviceId.value = 1000; //voice
    return true;
  }

  bool parseLine_commands(char * line, samson::system::UInt *serviceId, samson::system::String *command)
  {
    char service_name[256];
    char command_str[256];

    if (sscanf(line, "%s %[^\n]s", service_name, command_str) != 2)
    {
      LM_E(("Error parsing command:'%s'", line));
      return false;
    }

    serviceId->value = classify_name(service_name);
    LM_M(("Parsing command, service:%s(%lu), command:'%s'", service_name, serviceId->value, command_str));

    if (serviceId->value == 0)
    {
      LM_M(("command, not emitted with service:%s(%lu), command:'%s'", service_name, serviceId->value, command_str));
      return false;
    }
    command->value = command_str;

    return true;
  }

  void init(samson::KVWriter *writer)
  {
    source = environment->get("OTTstream.source", "HTTP"); // values: HTTP or DNS

    if (source == "HTTP")
    {
      _parseFunction = &parse_logs::parseLine_HTTP;
    }
    else if (source == "DNS")
    {
      _parseFunction = &parse_logs::parseLine_DNS;
    }
    else if (source == "SMS")
    {
      _parseFunction = &parse_logs::parseLine_SMS;
    }
    else if (source == "voice")
    {
      _parseFunction = &parse_logs::parseLine_voice;
    }
    else if ((source != "commands") && (source != "command")) // command have different output, so have to be treated in a different function
    {
      LM_W(("Source:%s not supported", source.c_str()));
      _parseFunction = &parse_logs::parseLine_voice;
    }

    sep = '\t';

    {
      OTTService *newService = new OTTService("Skype", 10, false);
      newService->addHTTPPattern("http://ui.skype.com/%/getlatestversion?%");
      newService->addHTTPPattern("http://apps.skype.com/countrycode");
      newService->addHTTPPattern("http://conn.skype.com");
      services.push_back(newService);
    }
    {
      OTTService *newService = new OTTService("Viber", 20, false);
      newService->addHTTPPattern("http://www.cdn.viber.com/ok.txt");
      newService->addHTTPPattern("http://www.cdn.viber.com/android_version.txt");
      services.push_back(newService);
    }
    {
      OTTService *newService = new OTTService("Facebook", 30, false);
      newService->addHTTPPattern("http://www.facebook.com/ajax/messaging/typ.php?__a=1");
      newService->addHTTPPattern("http://apps.facebook.com/ajax/messaging/typ.php?__a=1");
      newService->addHTTPPattern("http://es-es.facebook.com/ajax/messaging/typ.php?__a=1");
      newService->addHTTPPattern("http://www.facebook.com/ajax/messaging/async.php?__a=1");
      newService->addHTTPPattern("http://apps.facebook.com/ajax/messaging/async.php?__a=1");
      newService->addHTTPPattern("http://es-es.facebook.com/ajax/messaging/async.php?__a=1");
      newService->addHTTPPattern("http://www.facebook.com/ajax/messaging/send.php?__a=1");
      newService->addHTTPPattern("http://apps.facebook.com/ajax/chat/send.php?__a=1");
      newService->addHTTPPattern("http://es-es.facebook.com/ajax/chat/send.php?__a=1");
      newService->addHTTPPattern("http://www.facebook.com/ajax/chat/buddy_list.php?__a=1");
      newService->addHTTPPattern("http://apps.facebook.com/ajax/chat/buddy_list.php?__a=1");
      newService->addHTTPPattern("http://es-es.facebook.com/ajax/chat/buddy_list.php?__a=1");
      newService->addHTTPPattern("http://www.facebook.com/ajax/chat/tabs.php?__a=1");
      newService->addHTTPPattern("http://apps.facebook.com/ajax/chat/tabs.php?__a=1");
      newService->addHTTPPattern("http://touch.facebook.com/touch/chathistory.php");
      newService->addHTTPPattern("http://iphone.facebook.com/touch/chathistory.php");
      newService->addHTTPPattern("http://www.facebook.com/ajax/presence/update.php");
      newService->addHTTPPattern("http://%.channel.facebook.com%");
      services.push_back(newService);
    }
    {
      OTTService *newService = new OTTService("GTalk", 40, true);
      newService->addHTTPPattern("talkgadget.google.com");
      newService->addHTTPPattern("talkgadget.google");
      newService->addHTTPPattern("hostedtalkgadget.google.com");
      newService->addHTTPPattern("dl.google.com");
      newService->addHTTPPattern("tc.v%.cache%.c.pack.google.com");
      newService->addDNSPattern("talkgadget.google.com");
      newService->addDNSPattern("talkgadget.l.google.com");
      newService->addDNSPattern("hostedtalkgadget.google.com");
      newService->addDNSPattern("%.talkgadget.google.com");
      newService->addDNSPattern("mtalk.google.com");
      newService->addDNSPattern("talk.google.com");
      newService->addDNSPattern("mtalk.google.com.%");
      newService->addDNSPattern("talk.google.com.%");
      newService->addDNSPattern("talk.%.google.com");
      services.push_back(newService);
    }
    {
      OTTService *newService = new OTTService("WhatsApp", 50, true);
      newService->addDNSPattern("sro.whatsapp.net");
      newService->addDNSPattern("bin-short.whatsapp.net");
      newService->addDNSPattern("bin-nokia.whatsapp.net");
      newService->addDNSPattern("mms.whatsapp.net");
      newService->addDNSPattern("mms40%.whatsapp.net");
      newService->addDNSPattern("mms30%.whatsapp.net");
      newService->addDNSPattern("mms20%.whatsapp.net");
      services.push_back(newService);
    }
    {
      OTTService *newService = new OTTService("faceTime", 60, true);
      services.push_back(newService);
    }
    {
      OTTService *newService = new OTTService("iphoneMessage", 70, true);
      services.push_back(newService);
    }
    {
      OTTService *newService = new OTTService("Tango", 80, true);
      newService->addDNSPattern("%.cm.tango.me");
      services.push_back(newService);
    }
    {
      OTTService *newService = new OTTService("Tuenti", 90, true);
      newService->addHTTPPattern("m.tuenti.com");
      newService->addHTTPPattern("xmpp%.tuenti.com");
      newService->addHTTPPattern("api.tuenti.com");
      newService->addHTTPPattern("fotos.api.tuenti.com");
      newService->addHTTPPattern("api.pl.tuenti.com");
      services.push_back(newService);
    }
    {
      OTTService *newService = new OTTService("Google+", 100, true);
      newService->addDNSPattern("plus.google.com");
      services.push_back(newService);
    }
//    {
//      OTTService *newService = new OTTService("TEST", 200, true);
//      newService->addHTTPPattern("www.c%n.v%ber.com");
//      newService->addDNSPattern("www.applesfera.com");
//      newService->addDNSPattern("www.applesfera.com");
//      services.push_back(newService);
//    }

    count.value = 1;
  }

  void parseLine(char * line, samson::KVWriter *writer)
  {

    if ((source != "commands") && (source != "command"))
    {
      if ((this->*_parseFunction)(line, &userId, &service_hit, &url))
      {
        //LM_M(("User:%lu detected serviceId:%lu at time:%s", userId.value, hit.serviceId.value, hit.timestamp.str().c_str()));
        writer->emit(0, &userId, &service_hit);

        if ((service_hit.serviceId.value != 0) && (service_hit.serviceId.value != 1000) && (service_hit.serviceId.value != 1100))
        {
          // Now emit the second queue, with serviceId as key
          serviceId.value = service_hit.serviceId.value;
          user_hit.userId.value = userId.value;
          user_hit.timestamp.value = service_hit.timestamp.value;
          writer->emit(1, &serviceId, &user_hit);
        }

        if (source == "HTTP")
        {
          if ((url.value != " ") && (url.value != ""))
          {
          writer->emit(3, &url, &count);
          }
        }
      }
    }
    else
    {
      if (parseLine_commands(line, &serviceId, &command))
      {
        LM_M(("Emit command:'%s' on serviceId:%lu to the command queue", command.value.c_str(), serviceId.value));
        writer->emit(2, &serviceId, &command);
      }
    }

  }

  void finish(samson::KVWriter *writer)
  {
  }

};

} // end of namespace OTTstream
} // end of namespace samson

#endif