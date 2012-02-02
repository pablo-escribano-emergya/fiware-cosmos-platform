
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_passive_location_update_tmsi_imsi_table
#define _H_SAMSON_passive_location_update_tmsi_imsi_table


#include <samson/module/samson.h>
#include <samson/modules/passive_location/CompleteTMSI.h>
#include <samson/modules/system/UInt.h>
#include <stdint.h>


namespace samson{
namespace passive_location{


class update_tmsi_imsi_table : public samson::Reduce
{

    samson::passive_location::CompleteTMSI completeTMSI;    // Input key
    samson::passive_location::CompleteTMSI newTMSI;    // Used as value at the output
    samson::passive_location::IMSIbyTime newIMSI;                           // Input value of the records
    samson::passive_location::IMSIbyTime stateIMSI;                         // Input value of the state

    uint64_t  tmps_timespan;
    time_t now;

public:


    //  INFO_MODULE
    // If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
    // Please, do not remove this comments, as it will be used to check consistency on module declaration
    //
    //  input: passive_location.CompleteTMSI passive_location.IMSIbyTime
    //  input: passive_location.CompleteTMSI passive_location.CompleteTMSI
    //  input: passive_location.CompleteTMSI passive_location.IMSIbyTime
    //  output: passive_location.CompleteTMSI passive_location.IMSIbyTime
    //
    // helpLine: Update the table to translate to recover imsi from tmsi
    //  END_INFO_MODULE

    void init( samson::KVWriter *writer )
    {
        tmps_timespan = environment->getSizeT("pl.tmps_age.timespan", 0);
        if ( tmps_timespan != 0)
        {
            now = time(NULL);
        }
    }

    void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
    {

        if ((inputs[0].num_kvs == 0) && (inputs[1].num_kvs > 0))
        {
            // If we don't receive new tmsi, just resend the state

            // If we are in 'live mode' (timespan != 0), only reemit recent
            // associations
            stateIMSI.parse(inputs[1].kvs[0]->value);
            if ((tmps_timespan == 0) || ((now - stateIMSI.timestamp.value) < (int64_t)tmps_timespan))
            {
                completeTMSI.parse(inputs[1].kvs[0]->key);
                writer->emit( 0 , &completeTMSI , &stateIMSI );
            }
            return;
        }

        completeTMSI.parse(inputs[0].kvs[0]->key);

        // We should keep only the newest record

        newIMSI.parse(inputs[0].kvs[inputs[0].num_kvs-1]->value);
        //LM_M(("TMSI[inputs[0].num_kvs-1]:'%s'", newIMSI.str().c_str()));

        writer->emit( 0 , &completeTMSI , &newIMSI );

    }

    void finish( samson::KVWriter *writer )
    {
    }



};


} // end of namespace passive_location
} // end of namespace samson

#endif
