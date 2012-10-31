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

#ifndef _H_SAMSON_mob2_MobVar_calcvariables
#define _H_SAMSON_mob2_MobVar_calcvariables


#include <samson/module/samson.h>
#include <samson/modules/cdr/CellPos.h>
#include <samson/modules/mob2/MobVars.h>
#include <samson/modules/mob2/TelMonth.h>
#include <samson/modules/system/UInt.h>



namespace samson{
namespace mob2{


class MobVar_calcvariables : public samson::Reduce
{
	//Inputs
	TelMonth telMonth;
	samson::cdr::CellPos cellInfo;

	//Outputs
	samson::system::UInt phone;
	MobVars mobVars;

	// Temp
	TempMob tempMob;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: mob2.TelMonth cdr.CellPos
	output: system.UInt mob2.MobVars

	extendedHelp: 		Calculate individual variables by period

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	// Calculate variables for a month //
	/////////////////////////////////////

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		uint64_t j;
		float sum_pow2distCM = 0; // acumulador cuadrados de distancias al centro de masas (en km)
		float distanceact = 0;
		float distancemax = 0;

		// Initializations
		mobVars.dif_btss.value = mobVars.dif_muns.value = mobVars.dif_states.value = 0;
		mobVars.masscenter_utmx.value = mobVars.masscenter_utmy.value = 0;
		mobVars.radius.value = mobVars.diam_areainf.value = 0;

		telMonth.parse(inputs[0].kvs[0]->key);

		// Number of positions with bts
		mobVars.num_pos.value = inputs[0].num_kvs;

		// Temporal vector for compile btss, lacs and states
		tempMob.btssSetLength(mobVars.num_pos.value);
		tempMob.munsSetLength(mobVars.num_pos.value);
		tempMob.statesSetLength(mobVars.num_pos.value);
		tempMob.coord_utmxSetLength(mobVars.num_pos.value);
		tempMob.coord_utmySetLength(mobVars.num_pos.value);
		// Vectors are initialized to 0
		for ( j=0; j<mobVars.num_pos.value; j++)
		{
			tempMob.btss[j].value = 0;
			tempMob.muns[j].value = 0;
			tempMob.states[j].value = 0;
			tempMob.coord_utmx[j].value = 0;
			tempMob.coord_utmy[j].value = 0;
		}

		// Calls in a period (Month and working/weekend day)
		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			cellInfo.parse(inputs[0].kvs[i]->value);

			// BTS
			for ( j=0; j<mobVars.dif_btss.value; j++)
			{
				if (tempMob.btss[j].value == cellInfo.bts.value)
					break;
			}
			if (j==mobVars.dif_btss.value)  // Bts is not in the vector
			{
				mobVars.dif_btss.value++;
				tempMob.btss[j].value = cellInfo.bts.value;
				tempMob.coord_utmx[j].value = cellInfo.posx.value;
				tempMob.coord_utmy[j].value = cellInfo.posy.value;
			}
			// LAC
			for ( j=0; j<mobVars.dif_muns.value; j++)
			{
				if (tempMob.muns[j].value == cellInfo.mun.value)
					break;
			}
			if (j==mobVars.dif_muns.value)  // Lac is not in the vector
			{
				mobVars.dif_muns.value++;
				tempMob.muns[j].value = cellInfo.mun.value;
			}
			// STATE
			for (j=0; j<mobVars.dif_states.value; j++)
			{
				if (tempMob.states[j].value == cellInfo.sta.value)
					break;
			}
			if (j==mobVars.dif_states.value) // State is not in the vector
			{
				mobVars.dif_states.value++;
				tempMob.states[j].value = cellInfo.sta.value;
			}
			// Mass center accumulation
			mobVars.masscenter_utmx.value += cellInfo.posx.value;
			mobVars.masscenter_utmy.value += cellInfo.posy.value;
		}

		// Mass center final calculation
		mobVars.masscenter_utmx.value /= mobVars.num_pos.value;
		mobVars.masscenter_utmy.value /= mobVars.num_pos.value;

		// Spin radius: weigthed mean of all distances to the mass center
		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			cellInfo.parse(inputs[0].kvs[i]->value);
			sum_pow2distCM += pow((cellInfo.posx.value-mobVars.masscenter_utmx.value)/1000.0,2)+pow((cellInfo.posy.value-mobVars.masscenter_utmy.value)/1000.0,2);
		}
		mobVars.radius.value = sqrt(sum_pow2distCM/mobVars.num_pos.value );

		// Influence area diameter (max distances between all BTSs)
		// Only different bts are considered
		for (unsigned int a=0; a<mobVars.dif_btss.value; a++) {
			for (unsigned int b=a+1; b<mobVars.dif_btss.value; b++) {
				distanceact = sqrt(pow((tempMob.coord_utmx[a].value-tempMob.coord_utmx[b].value)/1000.0,2)+pow((tempMob.coord_utmy[a].value-tempMob.coord_utmy[b].value)/1000.0,2));
				if (distanceact > distancemax)
					distancemax = distanceact;
			}
		}
		mobVars.diam_areainf.value = distancemax;

		phone.value = telMonth.phone.value;
		mobVars.month.value = telMonth.month.value;
		mobVars.workingday.value = telMonth.workingday.value;
		writer->emit(0, &phone,&mobVars);

	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif
