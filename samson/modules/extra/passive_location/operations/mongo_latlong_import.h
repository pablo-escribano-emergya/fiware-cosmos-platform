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

#ifndef _H_SAMSON_passive_location_mongo_latlong_import
#define _H_SAMSON_passive_location_mongo_latlong_import

#include <samson/module/samson.h>
#include <samson/modules/mobility/Record.h>
#include <samson/modules/system/UInt.h>

namespace samson {
namespace passive_location {

class mongo_latlong_import : public samson::Generator {

  public:

//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  output: system.UInt mobility.Record
//  
// helpLine: Import passive location 'latlong' data from a mongoDb server at 'mongo.ip' from table 'mongo.table'
//  END_INFO_MODULE

    void init(samson::KVWriter *writer) {
    }

    void setup(int worker, int num_workers, int process, int num_processes) {
    }

    void run(samson::KVWriter *writer) {
    }

    void finish(samson::KVWriter *writer) {
    }

};

} // end of namespace passive_location
} // end of namespace samson

#endif
