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

#ifndef _H_SAMSON_iwebp_process
#define _H_SAMSON_iwebp_process

#include <string>
#include <vector>

#include "samson_system/EmitFieldsProcess.h"
#include "samson_system/HitCountByConceptProcess.h"
#include "samson_system/HitCountProcess.h"
#include "samson_system/ProcessComponentManager.h"
#include "samson_system/SpreadMapProcess.h"
#include "samson_system/Value.h"
#include "samson/module/samson.h"

namespace samson {
namespace iwebp {

class process : public Reduce {
  public:
    //  INFO_MODULE
    // If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
    // Please, do not remove this comments, as it will be used to check consistency on module declaration
    //
    //  input: system.Value system.Value
    //  input: system.Value system.Value
    //  output: system.Value system.Value
    //  output: system.Value system.Value
    //  output: system.Value system.Value
    //
    //  helpLine: Bulk process operation for instant individual web profiling. Instances added at init()
    //  extendedHelp: Bulk process operation for instant individual web profiling. Instances added at init()
    //  As all the system.Value process(), one of the outputs is fedback to the stream input
    //
    //  END_INFO_MODULE

    void init(KVWriter *writer) {
      // TODO(@jges): Remove log messages
      LM_M(("iwebp.process: init()"));
      {
        system::SpreadMapProcess *p_operation = new system::SpreadMapProcess("agregatedKey", "individualConceptHits",
                                                                             "agregatedKey2", system::Value::kTimestampField);
        process_components_manager_.Add(p_operation);
      }
      {
        system::HitCountProcess *p_operation = new system::HitCountProcess("individualConceptHits",
                                                                           "individualConceptHits_counts",
                                                                           system::HitCountProcess::kNullDest);
        p_operation->AddUpdateCountFunction("short", 300, 100);
        p_operation->AddUpdateCountFunction("medium", 3600, 100);
        p_operation->AddUpdateCountFunction("long", 24 * 3600, 100);
        process_components_manager_.Add(p_operation);
      }
      {
        std::vector<std::string> dependent_fields;
        dependent_fields.push_back(system::Value::kUrlField);
        dependent_fields.push_back(system::Value::kDomainField);
        dependent_fields.push_back(system::Value::kUserField);
        system::EmitFieldsProcess *p_operation = new system::EmitFieldsProcess("agregatedKey2", "hitsByCategory",
                                                                               "agregatedKey3", dependent_fields,
                                                                               system::Value::kCategoriesField, system::Value::kTimestampField);
        process_components_manager_.Add(p_operation);
      }
      {
        system::HitCountByConceptProcess *p_operation =
            new system::HitCountByConceptProcess("hitsByCategory", system::Value::kCategoriesField, "hitsByCategory_counts",
                                                 system::HitCountByConceptProcess::kNullDest);
        p_operation->AddUpdateCountFunction("short", 300, 100);
        p_operation->AddUpdateCountFunction("medium", 3600, 100);
        p_operation->AddUpdateCountFunction("long", 24 * 3600, 100);
        process_components_manager_.Add(p_operation);
      }
      {
        std::vector<std::string> dependent_fields;
        dependent_fields.push_back(system::Value::kUrlField);
        dependent_fields.push_back(system::Value::kDomainField);
        dependent_fields.push_back(system::Value::kCategoriesField);
        dependent_fields.push_back(system::Value::kQueryWordsField);
        system::EmitFieldsProcess *p_operation = new system::EmitFieldsProcess("agregatedKey3", "hitsByUser",
                                                                               system::EmitFieldsProcess::kNullField,
                                                                               dependent_fields, "user", system::Value::kTimestampField);
        process_components_manager_.Add(p_operation);
      }
      {
        system::HitCountByConceptProcess *p_operation =
            new system::HitCountByConceptProcess("hitsByUser", system::Value::kUserField, "hitsByUser_counts",
                                                 system::HitCountByConceptProcess::kNullDest);
        p_operation->AddUpdateCountFunction("short", 300, 100);
        p_operation->AddUpdateCountFunction("medium", 3600, 100);
        p_operation->AddUpdateCountFunction("long", 24 * 3600, 100);
        process_components_manager_.Add(p_operation);
      }
    }

    void run(KVSetStruct* inputs, KVWriter *writer) {
      // TODO(@jges): Remove log messages
      LM_M(("iwebp.process: run()"));
      process_components_manager_.Process(inputs, writer);
    }

    void finish(KVWriter *writer) {
      // TODO(@jges): Remove log messages
      LM_M(("iwebp.process: finish()"));
    }

  private:
    system::ProcessComponentsManager process_components_manager_;
};
}   // end of namespace iwebp
}   // end of namespace samson

#endif
