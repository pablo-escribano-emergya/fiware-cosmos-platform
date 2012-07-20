
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
 */

#ifndef _H_SAMSON_level_GraphLine
#define _H_SAMSON_level_GraphLine


#include <samson/modules/level/GraphLine_base.h>

#include "logMsg/logMsg.h"

namespace samson{
namespace level{


class GraphLine : public GraphLine_base
{
    public:

    void Init()
    {
        prev_timestamp.value = 0;
        nsecs_since_shift.value = 0;
        nmins_since_shift.value = 0;

        NUMBER_OF_SECONDS.value=60;
        NUMBER_OF_MINUTES.value=59;
        NUMBER_OF_HOURS.value=23;
        NUMBER_OF_POINTS.value=(NUMBER_OF_SECONDS.value + NUMBER_OF_MINUTES.value + NUMBER_OF_HOURS.value);

        coordSetLength(NUMBER_OF_POINTS.value);
        for (int i = 0; (i < NUMBER_OF_POINTS.value); i++)
        {
            coord[i].xSetLength(2);
            coord[i].x[0].value = -1 * i;
            coord[i].x[1].value = 0;
        }

//        p_coord_seconds.value = reinterpret_cast<uint64_t> (coord);
//        p_coord_minutes.value = reinterpret_cast<uint64_t> (coord + NUMBER_OF_SECONDS.value);
//        p_coord_hours.value = reinterpret_cast<uint64_t> (p_coord_minutes + NUMBER_OF_MINUTES.value);
    }

    void shift_hours(int num_items_shifted)
    {
        int end_hours = NUMBER_OF_POINTS.value;
        int init_hours = NUMBER_OF_SECONDS.value + NUMBER_OF_MINUTES.value;

        if (num_items_shifted > NUMBER_OF_HOURS.value)
        {
            LM_E(("Error. Not possible to shift more than %d hours, trying %d", NUMBER_OF_HOURS.value, num_items_shifted));
            num_items_shifted = NUMBER_OF_HOURS.value - 1;
        }
        for (int i = (end_hours - 1); (i >= (init_hours + num_items_shifted)); i--)
        {
            //LM_M(("Shift last hours from index %d to index:%d(val:%d)", i - num_items_shifted, i, coord[i - num_items_shifted].x[1].value));
            coord[i].x[1].value = coord[i - num_items_shifted].x[1].value;
        }
        for (int i = (init_hours + num_items_shifted-1); (i > init_hours); i--)
        {
            //LM_M(("Shift first hours from index %d to index:%d(val:%d)", init_hours, i, coord[init_hours].x[1].value));
            coord[i].x[1].value = coord[init_hours].x[1].value;
        }
    }
    void shift_minutes(int num_items_shifted)
    {
        int end_minutes = NUMBER_OF_SECONDS.value + NUMBER_OF_MINUTES.value;
        int init_minutes = NUMBER_OF_SECONDS.value;

        if (num_items_shifted > NUMBER_OF_MINUTES.value)
        {
            LM_E(("Error. Not possible to shift more than %d minutes, trying %d", NUMBER_OF_MINUTES.value, num_items_shifted));
            num_items_shifted = NUMBER_OF_MINUTES.value - 1;
        }
        for (int i = (end_minutes - 1); (i >= (init_minutes + num_items_shifted)); i--)
        {
            //LM_M(("Shift last minutes from index %d to index:%d(val:%d)", i - num_items_shifted, i, coord[i - num_items_shifted].x[1].value));
            coord[i].x[1].value = coord[i - num_items_shifted].x[1].value;
        }
        for (int i = (init_minutes + num_items_shifted-1); (i > init_minutes); i--)
        {
            //LM_M(("Shift first minutes from index %d to index:%d(val:%d)", init_minutes, i, coord[init_minutes].x[1].value));
            coord[i].x[1].value = coord[init_minutes].x[1].value;
        }
    }
    void shift_seconds(int num_items_shifted)
    {
        int end_seconds = NUMBER_OF_SECONDS.value;
        int init_seconds = 0;

        if (num_items_shifted > NUMBER_OF_SECONDS.value)
        {
            LM_E(("Error. Not possible to shift more than %d seconds, trying %d", NUMBER_OF_SECONDS.value, num_items_shifted));
            num_items_shifted = NUMBER_OF_SECONDS.value - 1;
        }
        for (int i = (end_seconds - 1); (i >= (init_seconds + num_items_shifted)); i--)
        {
            //LM_M(("Shift last seconds from index %d to index:%d(val:%d)", i - num_items_shifted, i, coord[i - num_items_shifted].x[1].value));
            coord[i].x[1].value = coord[i - num_items_shifted].x[1].value;
        }
        for (int i = (init_seconds + num_items_shifted-1); (i > init_seconds); i--)
        {
            //LM_M(("Shift first seconds from index %d to index:%d(val:%d)", init_seconds, i, coord[init_seconds].x[1].value));
            coord[i].x[1].value = coord[init_seconds].x[1].value;
        }
    }
    int average(samson::level::Coordinate *p_init_window, const int window_length)
    {
        uint64_t acum_value = 0;
        for (int i = 0; (i < window_length); i++)
        {
            acum_value += p_init_window[i].x[1].value;
        }
        //LM_M(("Return average:%d (acum(%d)/window_length(%d))", (acum_value / window_length),  acum_value, window_length));
        return (acum_value / window_length);
    }
};


} // end of namespace samson
} // end of namespace level

#endif