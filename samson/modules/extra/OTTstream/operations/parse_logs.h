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
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_OTTstream_parse_logs
#define _H_SAMSON_OTTstream_parse_logs

#include <samson/module/samson.h>
#include <samson/modules/OTTstream/ServiceHit.h>
#include <samson/modules/OTTstream/WebCategoryHit.h>
#include <samson/modules/OTTstream/UserHit.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/SimpleParser.h>

#include "comscore/SamsonComscoreDictionary.h"

#include "OTTService.h"

#include "logMsg/logMsg.h"

namespace samson
{
namespace OTTstream
{

class parse_logs: public samson::system::SimpleParser
{

    bool (parse_logs::*_parseFunction)(char*line, samson::system::UInt *userId, ServiceHit *hit, ServiceHit *group_hit, samson::system::String *url);

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
    samson::OTTstream::ServiceHit group_service_hit;
    samson::OTTstream::WebCategoryHit web_hit;
    samson::system::String command;

    samson::comscore::SamsonComscoreDictionary samson_comscore_dictionary;
    samson::system::String output_key;

    public:

    //  INFO_MODULE
    // If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
    // Please, do not remove this comments, as it will be used to check consistency on module declaration
    //
    //  output: system.UInt OTTstream.ServiceHit
    //  output: system.UInt OTTstream.WebCategoryHit
    //  output: system.UInt system.String
    //  output: system.String system.UInt
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

    OTTService *classify_http(const char *url, const char *host)
    {
        for (unsigned int i = 0; (i < services.size()); i++)
        {
            if (services[i]->checkHTTP(url, host))
            {
                //LM_M(("url:'%s', host:'%s' identified as service:'%s' by category:%d", url, host, services[i]->name.c_str(), services[i]->category));
                return services[i];
            }
        }
        //LM_M(("unknown url:'%s', host:'%s'", url, host));
        return services[0];
    }

    OTTService *classify_dns(const char *url)
    {
        for (unsigned int i = 0; (i < services.size()); i++)
        {
            if (services[i]->checkDNS(url))
            {
                //LM_M(("url:'%s' identified as service:'%s'", url, services[i]->name.c_str()));
                return services[i];
            }
        }
        //LM_M(("unknow url:'%s'", url));
        return services[0];
    }

    OTTService *classify_name(const char *name)
    {
        for (unsigned int i = 0; (i < services.size()); i++)
        {
            if (services[i]->checkName(name))
            {
                return services[i];
            }
        }
        //LM_M(("unknow name:'%s'", name));
        return services[0];
    }

    bool parseLine_HTTP(char * line, samson::system::UInt *userId, ServiceHit *hit, ServiceHit *group_hit, samson::system::String *url)
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
        group_hit->serviceId.value = 0;


        char *p_url;
        const char *p_host;

        split_in_words(line, fields, sep);

        if (fields.size() != 16)
        {
            return false;
        }
        userId->value = phone2number(fields[6], maxDigits);

        // To be sure we work only with 68... and 69... msisdn
        if ((userId->value == 0) || (userId->value < 680000000) || (userId->value > 699999999))
        {
            return false;
        }

        p_host = fields[2];
        p_url = fields[8];

        OTTService *detected_service = classify_http(p_url, p_host);
        if (detected_service == NULL)
        {
            return false;
        }
        hit->serviceId.value = detected_service->serviceId;
        group_hit->serviceId.value = detected_service->group_serviceId;

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
        group_hit->timestamp.value = hit->timestamp.value;
        //LM_M(("input_time:'%s', timeUnix:%lu, '%s'", fields[9], hit->timestamp.value, hit->timestamp.str().c_str()));

        return true;

    }

    bool parseLine_DNS(char * line, samson::system::UInt *userId, ServiceHit *hit, ServiceHit *group_hit, samson::system::String *url)
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
        group_hit->serviceId.value = 0;

        const char *p_domain;

        split_in_words(line, fields, sep);

        if (fields.size() != 11)
        {
            return false;
        }
        userId->value = phone2number(fields[3], maxDigits);

        // To be sure we work only with 68... and 69... msisdn
        if ((userId->value == 0) || (userId->value < 680000000) || (userId->value > 699999999))
        {
            return false;
        }


        p_domain = fields[9];

        OTTService *detected_service = classify_dns(p_domain);
        if (detected_service == NULL)
        {
            return false;
        }
        hit->serviceId.value = detected_service->serviceId;
        group_hit->serviceId.value = detected_service->group_serviceId;


        if (hit->serviceId.value == 0)
        {
            //return false;
        }

        hit->timestamp.setFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[4]);
        group_hit->timestamp.value = hit->timestamp.value;

        return true;

    }

    bool parseLine_SMS(char * line, samson::system::UInt *userId, ServiceHit *hit, ServiceHit *group_hit, samson::system::String *url)
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

            // To be sure we work only with 68... and 69... msisdn
            if ((userId->value == 0) || (userId->value < 680000000) || (userId->value > 699999999))
            {
                return false;
            }

            hit->timestamp.setFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[12]);
            group_hit->timestamp.value = hit->timestamp.value;
        }
        else if (type == 2)
        {
            userId->value = phone2number(fields[7], maxDigits);

            // To be sure we work only with 68... and 69... msisdn
            if ((userId->value == 0) || (userId->value < 680000000) || (userId->value > 699999999))
            {
                return false;
            }

            hit->timestamp.setFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[14]);
            group_hit->timestamp.value = hit->timestamp.value;
        }
        else
        {
            return false;
        }

        hit->serviceId.value = 1002; //SMS
        group_hit->serviceId.value = 1000;  // CORE
        return true;

    }

    bool parseLine_voice(char * line, samson::system::UInt *userId, ServiceHit *hit, ServiceHit *group_hit, samson::system::String *url)
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

        // To be sure we work only with 68... and 69... msisdn
        if ((userId->value == 0) || (userId->value < 680000000) || (userId->value > 699999999))
        {
            return false;
        }


        hit->timestamp.setFromStrTimeDate_YYYY_mm_dd_24H(fields[1]);
        group_hit->timestamp.value = hit->timestamp.value;
        //LM_M(("input_time:'%s', timeUnix:%lu, '%s'", fields[1], hit->timestamp.value, hit->timestamp.str().c_str()));

        hit->serviceId.value = 1001; //voice
        group_hit->serviceId.value = 1000;  // CORE
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

        OTTService *detected_service = classify_name(service_name);
        if (detected_service == NULL)
        {
            return false;
        }
        serviceId->value = detected_service->serviceId;

        LM_M(("Parsing command, service:%s(%lu), command:'%s'", service_name, serviceId->value, command_str));

        if (serviceId->value == 0)
        {
            LM_M(("command not found in services list, will try as a sequence. service:%s(%lu), command:'%s'", service_name, serviceId->value, command_str));
            serviceId->value = strtoul(service_name, NULL, 10);
            if (serviceId->value == 0)
            {
                LM_M(("service:%s(%lu), not recognized as a number. Ignoring command:'%s'", service_name, serviceId->value, command_str));
                return false;
            }
        }
        command->value = command_str;
        LM_M(("Emit command, service:%s(%lu), command:'%s'", service_name, serviceId->value, command_str));
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
            OTTService *newService = new OTTService("web", 0, 0, false);
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("Skype", 11, 10, false);
            newService->addHTTPPattern("http://ui.skype.com/%/getlatestversion?%");
            newService->addHTTPPattern("http://apps.skype.com/countrycode");
            newService->addHTTPPattern("http://conn.skype.com");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("Viber", 12, 10, false);
            newService->addHTTPPattern("http://www.cdn.viber.com/ok.txt");
            newService->addHTTPPattern("http://www.cdn.viber.com/android_version.txt");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("GTalk", 13, 10, true);
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
            OTTService *newService = new OTTService("WhatsApp", 14, 10, true);
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
            OTTService *newService = new OTTService("faceTime", 15, 10, true);
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("iphoneMessage", 16, 10, true);
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("Tango", 17, 10, true);
            newService->addDNSPattern("%.cm.tango.me");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("Facebook", 21, 20, false);
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
            OTTService *newService = new OTTService("Tuenti", 22, 20, true);
            newService->addHTTPPattern("m.tuenti.com");
            newService->addHTTPPattern("xmpp%.tuenti.com");
            newService->addHTTPPattern("api.tuenti.com");
            newService->addHTTPPattern("fotos.api.tuenti.com");
            newService->addHTTPPattern("api.pl.tuenti.com");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("Google+", 23, 20, true);
            newService->addDNSPattern("plus.google.com");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("VodafoneADSL", 101, 100, false);
            newService->addHTTPPattern("%vodafone%/%internet-y-tv/adsl%");
            newService->addHTTPPattern("%adsl-vodafone%");
            newService->addHTTPPattern("%internetvodafone%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("VodafoneClientes", 102, 100, false);
            newService->addHTTPPattern("%club2020.mi.vodafone%");
            newService->addHTTPPattern("%mivodafone%");
            newService->addHTTPPattern("%vodafone.es/%amigo%");
            newService->addHTTPPattern("%mi-vodafone%");
            newService->addHTTPPattern("%vodafone.es/miregalo%");
            newService->addHTTPPattern("%vodafone.es/%programa%");
            newService->addHTTPPattern("%vodafone.es/%trae-un-amigo%");
            newService->addHTTPPattern("%vodafone.es/%promociones%");
            newService->addHTTPPattern("%vodafone.es/%puntos%");
            newService->addHTTPPattern("%areaclientes.vodafone%");
            newService->addHTTPPattern("%zonaclientes.vodafone%");

            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("VodafoneOnline", 103, 100, false);
            newService->addHTTPPattern("%atencionalcliente.vodafone%");
            newService->addHTTPPattern("%ayudacliente.vodafone%");
            newService->addHTTPPattern("%canalonline.vodafone%");
            newService->addHTTPPattern("%foro.vodafone%");
            newService->addHTTPPattern("%manuales.vodafone%");
            newService->addHTTPPattern("%vodafone%/%atencion-cliente%");
            newService->addHTTPPattern("%vodafone%/%ayuda%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("VodafoneContacto", 104, 100, false);
            newService->addHTTPPattern("%vodafone%/conocenos%");
            newService->addHTTPPattern("%vodafone%/%app%-y-descargas%");
            newService->addHTTPPattern("%vodafone%/%zonadescargas%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("VodafoneEmpresas", 105, 100, false);
            newService->addHTTPPattern("%mensaredempresas.vodafone%");
            newService->addHTTPPattern("%vodafone%/autonomos%");
            newService->addHTTPPattern("%vodafone%/empresas%");
            newService->addHTTPPattern("%vodafone%/puntosempresa%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("VodafoneInternetMovil", 106, 100, false);
            newService->addHTTPPattern("%vodafone%/%internet-y-correo%");
            newService->addHTTPPattern("%vodafone%/%internet-movil%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("VodafoneMovilFijo", 107, 100, false);
            newService->addHTTPPattern("%vodafone%/%/moviles-%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("VodafoneTV", 108, 100, false);
            newService->addHTTPPattern("%vodafone%/%internet-y-tv/television%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("VodafoneTienda", 109, 100, false);
            newService->addHTTPPattern("%tienda%.vodafone%");
            newService->addHTTPPattern("%tiendaempresasvodafone%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("OrangeClientes", 201, 200, false);
            newService->addHTTPPattern("%area%clientes.orange.es%");
            newService->addHTTPPattern("%areaprivada.orange.es%");
            newService->addHTTPPattern("%clientes%.orange.es%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("OrangeOnline", 202, 200, false);
            newService->addHTTPPattern("%ayuda.orange.es%");
            newService->addHTTPPattern("%configuratumovil.orange.es%");
            newService->addHTTPPattern("%foros.%orange.es%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("OrangeEmpresas", 203, 200, false);
            newService->addHTTPPattern("%orange.es/empresas%");
            newService->addHTTPPattern("%orange.es%/autonomos%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("OrangeADSL", 204, 200, false);
            newService->addHTTPPattern("%internet.orange.es%");
            newService->addHTTPPattern("%orange.es/internet%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("OrangeInfoMovil", 205, 200, false);
            newService->addHTTPPattern("%movil.orange.es%");
            newService->addHTTPPattern("%orange.es/movil%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("OrangePortabilidad", 206, 200, false);
            newService->addHTTPPattern("%portabilidad%orange.es%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("OrangeTienda", 207, 200, false);
            newService->addHTTPPattern("%tienda%orange.es%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("OrangeTonos", 208, 200, false);
            newService->addHTTPPattern("%tonosdeespera%orange%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoClientes", 301, 300, false);
            newService->addHTTPPattern("%miyoigo.yoigo%");
            newService->addHTTPPattern("%yoigo.com/mequedo%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoBlogs", 302, 300, false);
            newService->addHTTPPattern("%yoigo.blogspot%");
            newService->addHTTPPattern("%foroyoigo%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoOnline", 303, 300, false);
            newService->addHTTPPattern("%yoigo.com/ayuda-al-cliente%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoCobertura", 304, 300, false);
            newService->addHTTPPattern("%cobertura.yoigo.com%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoComparadorMoviles", 305, 300, false);
            newService->addHTTPPattern("%yoigo.com/comparador%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoCondiciones", 306, 300, false);
            newService->addHTTPPattern("%yoigo.com/%condiciones%");
            newService->addHTTPPattern("%yoigo.com/preguntas%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoInternet", 307, 300, false);

            newService->addHTTPPattern("%yoigo.com/internet%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoRecarga", 308, 300, false);
            newService->addHTTPPattern("%yoigo.com/recarga%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoRevista", 309, 300, false);
            newService->addHTTPPattern("%yoigo.com/revista%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoServicios", 310, 300, false);
            newService->addHTTPPattern("%yoigo.com/servicios%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoTarifas", 311, 300, false);
            newService->addHTTPPattern("%yoigo.com/tarifas%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("YoigoTiendasFisicas", 312, 300, false);
            newService->addHTTPPattern("%yoigo.com/tiendasyoigo%");
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("voice", 1001, 1000, false);
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("SMS", 1002, 1000, false);
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("seqLargaGTalk", 2001, 2000, false);
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("seqCortaGTalk", 2002, 2000, false);
            services.push_back(newService);
        }
        {
            OTTService *newService = new OTTService("seqWhatsApp", 2003, 2000, false);
            services.push_back(newService);
        }

        count.value = 1;

        samson_comscore_dictionary.read( "/var/comscore/samson_comscore_dictionary.bin" );
    }

    void parseLine(char * line, samson::KVWriter *writer)
    {
        if ((source != "commands") && (source != "command"))
        {
            if ((this->*_parseFunction)(line, &userId, &service_hit, &group_service_hit, &url))
            {
                //LM_M(("User:%lu detected serviceId:%lu at time:%s", userId.value, hit.serviceId.value, hit.timestamp.str().c_str()));
                writer->emit(0, &userId, &service_hit);

                if (group_service_hit.serviceId.value > 0)
                {
                    writer->emit(0, &userId, &group_service_hit);
                }

                if (source == "HTTP")
                {
                    //LM_M(("Ready to process an URL"));
                    if ((url.value != " ") && (url.value != ""))
                    {
                        // Remove protocol information ( if any )
                        size_t pos = url.value.find("://");
                        if( pos != std::string::npos )
                            url.value = url.value.substr( pos+3 );

                        // Emit an output to track popular urls
                        output_key.value = au::str("url %s" , url.value.c_str() );
                        //LM_M(("Emit topic:'%s'", output_key.value.c_str()));
                        writer->emit( 3 , &output_key , &count );

                        // Extract domain to get the most popular domains
                        {
                            size_t pos = url.value.find("/");

                            if( pos == std::string::npos )
                                output_key.value = au::str("domain %s" , url.value.c_str() );
                            else
                                output_key.value = au::str("domain %s" , url.value.substr(0,pos).c_str() );
                            //LM_M(("Emit topic:'%s'", output_key.value.c_str()));
                            writer->emit( 3 , &output_key , &count );
                        }

                        // Get all categories for this url
                        std::vector<uint> categories_ids = samson_comscore_dictionary.getCategories( url.value.c_str() );
                        for ( size_t i = 0 ; i < categories_ids.size() ; i++ )
                        {
                            std::string name = samson_comscore_dictionary.getCategoryName( categories_ids[i] );

                            // Replace spaces by "_"s
                            for ( size_t s = 0 ; s < name.length() ; s++ )
                                if( name[s] == ' ')
                                    name[s] = '_';

                            // Emit an output for each category that has received a hit
			    output_key.value = au::str("category %s" , name.c_str() );
                            //LM_M(("Emit topic:'%s'", output_key.value.c_str()));
                            writer->emit( 3 , &output_key , &count );

                            // We also emit the categories per user, for the web profiling
                            web_hit.category.value = name;
                            web_hit.timestamp.value = service_hit.timestamp.value;
                            //LM_M(("Emit hit for user:%lu, category:'%s'", userId.value, output_key.value.c_str()));
                            writer->emit(1, &userId, &web_hit);
                        }
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
        while (services.size() > 0)
        {
            delete services.back();
            services.pop_back();
        }
    }

};

} // end of namespace OTTstream
} // end of namespace samson

#endif
