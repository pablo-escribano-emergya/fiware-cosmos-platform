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
#ifndef _H_SAMSON_system_EXTRACT_SINGLE_FIELDS_PROCESS
#define _H_SAMSON_system_EXTRACT_SINGLE_FIELDS_PROCESS

#include "samson_system/ProcessComponent.h"

namespace samson{ namespace system{

    class ExtractSingleFieldsProcess : public ProcessComponent
    {
        std::vector < std::string> concepts_;
        std::string out_app_name_;
        std::string out_def_name_;

    public:

        ExtractSingleFieldsProcess( ): ProcessComponent("default")
        {
            out_app_name_ = "def_out";
            out_def_name_ = "def_def";
        };

        ExtractSingleFieldsProcess( std::string name, std::vector <std::string> concepts, std::string out_app, std::string out_def ): ProcessComponent(name)
        {
            concepts_ = concepts;
            out_app_name_ = out_app;
            out_def_name_ = out_def;
        }

        std::string out_app_name() { return out_app_name_; }
        std::string out_def_name() { return out_def_name_; }

        // Update this state based on input values ( return true if this state has been updated with this component )
        bool update( Value* key ,
                            Value * state ,
                            Value ** values ,
                            size_t num_values ,
                            samson::KVWriter *writer );
    };

} } // End of namespace

#endif
