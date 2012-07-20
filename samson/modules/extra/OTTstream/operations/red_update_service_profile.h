
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_OTTstream_red_update_service_profile
#define _H_SAMSON_OTTstream_red_update_service_profile


#include <samson/module/samson.h>
#include <samson/modules/OTTstream/ActivityByService.h>
#include <samson/modules/OTTstream/UserHit.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace OTTstream{


class red_update_service_profile : public samson::Reduce
{

  samson::OTTstream::ActivityByService activity;
  samson::system::UInt serviceId;
  samson::OTTstream::UserHit hit;

  int danger_threshold;

public:


  //  INFO_MODULE
  // If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
  // Please, do not remove this comments, as it will be used to check consistency on module declaration
  //
  //  input: system.UInt OTTstream.UserHit
  //  input: system.UInt OTTstream.ActivityByService
  //  output: system.UInt OTTstream.UserActivity
  //  output: system.UInt OTTstream.ActivityByService
  //
  // helpLine: Update the latest service profile
  //  END_INFO_MODULE

  void init( samson::KVWriter *writer )
  {
    //LM_M(("Update service init"));
    danger_threshold = environment->getInt("OTTstream.danger_threshold", 1); // Threshold on web activity on the competence to detect flight danger

  }

  void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
  {
    if ( inputs[1].num_kvs == 0 )
    {
      // New service state
      activity.Init();
      serviceId.parse( inputs[0].kvs[0]->key );
    }
    else
    {
      // Previous state
      activity.parse( inputs[1].kvs[0]->value ); // We only consider one state
      serviceId.parse( inputs[1].kvs[0]->key );
      if (inputs[1].num_kvs > 1)
      {
        LM_W(("Multiple states(%lu) for serviceId:%lu", inputs[1].num_kvs, serviceId.value));
      }
    }

    LM_M(("For service:%lu, detected:%lu hits", serviceId.value,  inputs[0].num_kvs ));
    for (uint64_t i = 0; (i <  inputs[0].num_kvs) ; i++ )
    {
      hit.parse(inputs[0].kvs[i]->value);
      bool user_found = false;

      if (i%10000 == 0)
      {
        LM_M(("Check %lu of %lu, for service:%lu userId:%lu", i, inputs[0].num_kvs, serviceId.value, hit.user.userId.value));
      }

     // LM_M(("Check for service:%lu userId:%lu", serviceId.value, hit.user.userId.value));
      // First inefficient try with vectors (we would like having lists or maps)
      // As we insert new users at the end, will look first at the end of the vector

      for (int j= (activity.user_activity_length - 1); ((j >= 0) && (user_found == false)); j--)
      {
        if (hit.user.userId.value == activity.user_activity[j].user.userId.value)
        {
          if (activity.user_activity[j].last_timestamp.value < hit.timestamp.value)
          {
            activity.user_activity[j].last_timestamp.value = hit.timestamp.value;
          }
          activity.user_activity[j].count.value++;
          user_found = true;

          // Patch to emit alerts when user in danger of portability
          if ((serviceId.value >= 100) && (serviceId.value <1000))
          {
              LM_M(("Checking alert for userActivity:%s", activity.user_activity[j].str().c_str()));
              if (((hit.user.clusterId.value == 1) || (hit.user.clusterId.value == 3) || (hit.user.clusterId.value == 13)) && (activity.user_activity[j].user.sensitive.value == 0) && (activity.user_activity[j].count.value > static_cast<unsigned int>(danger_threshold)))
              {
                  activity.user_activity[j].user.sensitive.value = 1;
                  writer->emit(0, &serviceId, &(activity.user_activity[j]));
              }
              if ((activity.user_activity[j].user.sensitive.value == 0) && ((hit.user.userId.value == 696295295) || (hit.user.userId.value == 683232478) || (hit.user.userId.value == 682891197) || (hit.user.userId.value == 699705375) || (hit.user.userId.value == 689691211) || (hit.user.userId.value == 689947547) || (hit.user.userId.value == 686400583) || (hit.user.userId.value == 689706996) || (hit.user.userId.value == 685625133)))
              {
                  activity.user_activity[j].user.sensitive.value = 1;
                  writer->emit(0, &serviceId, &(activity.user_activity[j]));
              }
          }
        }


      }

      if (user_found == false)
      {
        //LM_M(("For service:%d new userId:%lu at time:%lu", serviceId.value, hit.user.userId.value, static_cast<unsigned long>(hit.timestamp.value)));
        activity.AddUser(hit.user, hit.timestamp, 1);
      }
    }
    //LM_M(("Update service:%lu state with %d users", serviceId.value, activity.user_activity_length));
    writer->emit(1, &serviceId, &activity);
    //LM_M(("Update service:%lu updated", serviceId.value));
  }

  void finish( samson::KVWriter *writer )
  {
    //LM_M(("Update service finished"));
  }



};


} // end of namespace OTTstream
} // end of namespace samson

#endif