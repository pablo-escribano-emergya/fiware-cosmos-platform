
#include "samson_system/ValueContainer.h"

#include "HubMapTimeProcess.h" // Own interface


namespace samson{ namespace system{

bool HubMapTimeProcess::update( Value* key ,Value * state ,Value ** values ,size_t num_values ,samson::KVWriter *writer )
{
    Value new_key;
    Value new_value;

    if (key->checkMapValue("app", name().c_str()))
    {
        std::vector<std::string> keys = key->get_keys_from_map();
        for (size_t i = 0; (i < keys.size()); i++)
        {
            if ((keys[i] == "app") || (keys[i] == "timestamp"))
            {
                continue;
            }
            //new_key.set_as_void();
            new_key.set_as_map();
            new_key.add_value_to_map("app")->set_string(out_app_name().c_str());
            new_key.add_value_to_map("concept")->set_string(keys[i].c_str());

            Value *p_value = key->get_value_from_map(keys[i].c_str());
            if (p_value->isVector())
            {
                for (size_t k = 0; (k < p_value->get_vector_size()); k++)
                {
                    //new_value.set_as_void();
                    new_value.set_as_map();
                    new_value.add_value_to_map("item")->set_string( p_value->get_value_from_vector(k)->get_string());
                    new_value.add_value_to_map("time")->set_double( key->get_double_from_map("timestamp"));
                    for (size_t j = 0; (j < num_values); j++)
                    {
                        //LM_M(("Emit vector item:'%s'(time:%lf) for concept:'%s'", p_value->get_value_from_vector(k)->get_string().c_str(), key->get_double_from_map("timestamp"), keys[i].c_str()));
                        emit_feedback(&new_key, &new_value, writer);
                    }
                }
            }
            else if (p_value->isMap())
            {
                LM_E(("Error, don't know how to distribute concept '%s' being a map", keys[i].c_str()));
                continue;
            }
            else
            {
                new_value.set_as_void();
                new_value.set_as_map();
                new_value.add_value_to_map("item")->set_string( p_value->get_string());
                new_value.add_value_to_map("time")->set_double( key->get_double_from_map("timestamp"));
                for (size_t j = 0; (j < num_values); j++)
                {
                    //LM_M(("Emit vector item:'%s'(time:%lf) for concept:'%s'", p_value->get_string().c_str(), key->get_double_from_map("timestamp"), keys[i].c_str()));
                    emit_feedback(&new_key, &new_value, writer);
                }
            }
        }
        key->set_string_for_map("app", out_def_name().c_str());
        for (size_t j = 0; (j < num_values); j++)
        {
            emit_feedback(key, values[j], writer);
        }
        return true;
    }
    else
    {
        return false;
    }
}



}} // End of namespace