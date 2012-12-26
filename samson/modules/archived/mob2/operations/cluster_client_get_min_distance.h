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

#ifndef _H_SAMSON_mob2_cluster_client_get_min_distance
#define _H_SAMSON_mob2_cluster_client_get_min_distance


#include <samson/module/samson.h>
#include <samson/modules/mob2/Cluster.h>
#include <samson/modules/mob2/ClusterVector.h>
#include <samson/modules/mob2/Node_Bts.h>
#include <samson/modules/system/UInt.h>
#include "mongo/client/dbclient.h"
#include "mongo/client/dbclientcursor.h"
#include "mob2_environment_parameters.h"



namespace samson{
namespace mob2{


class cluster_client_get_min_distance : public samson::Map
{
	// Inputs
	Node_Bts nodbts;
	ClusterVector clusVector;
	// Ouputs
	samson::system::UInt node;
	Cluster cluster;

	//reference clusters
	ClusterSet clientClusters;

	// MongoDB variables
	std::string          mongo_ip;
	std::string          mongo_db;
	std::string          mongo_collection;
	std::string          mongo_db_path;
	DBClientConnection*  mdbConnection;

	// Environment variable
	int coordsLength;

	// Temporal variables
	samson::system::Double coord;
	Cluster clusterInfo;


public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: mob2.Node_Bts mob2.ClusterVector
	output: system.UInt mob2.Cluster

	extendedHelp: 		Calculate the euclidean distance between a vector and a reference cluster, read from mongoDB

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
		OLM_T(LMT_User06, ("Starting init()"));

		coordsLength = environment->getInt(MOB2_PARAMETER_COORDS_LENGTH, MOB2_PARAMETER_COORDS_LENGTH_DEFAULT);

		clientClusters.clusterSetLength(0);
		mongo_ip           = environment->get("mongo.ip",    "no-mongo-ip");
		mongo_db           = environment->get("mongo.db",    "no-mongo-db");
		mongo_collection   = environment->get("mongo.collection", "no-mongo-collection");

		if (mongo_ip == "no-mongo-ip")
		{
			tracer->setUserError("No mongo ip is specified. Please specify mongo ip with 'mongo.ip' environment variable");
			OLM_E(("No mongo ip specified. Please specify mongo ip with 'mongo.ip' environment variable"));
			return;
		}

		if (mongo_db == "no-mongo-db")
		{
			tracer->setUserError("No db specified. Please specify mongo database db name with 'mongo.db' environment variable");
			OLM_E(("No db specified. Please specify mongo database db name with 'mongo.db' environment variable"));
			return;
		}

		if (mongo_collection == "no-mongo-collection")
		{
			tracer->setUserError("No collection is specified. Please specify mongo collection with 'mongo.collection' environment variable");
			OLM_E(("No collection specified. Please specify mongo database collection name with 'mongo.collection' environment variable"));
			return;
		}

		mongo_db_path = mongo_db + "." + mongo_collection;

		mdbConnection = new DBClientConnection();
		if (mdbConnection == NULL)
		{
			tracer->setUserError("Error creating client mongoDB connection");
			OLM_E(("Error creating client mongoDB connection"));
		}

		try
		{
			mdbConnection->connect(mongo_ip);
		}
		catch ( ... )
		{
			tracer->setUserError("Error connecting to mongo_ip");
			OLM_E(("Error connecting to mongo_ip:'%s'", mongo_ip.c_str()));
		}

		BSONObj                 bo;

		auto_ptr<DBClientCursor> cursor = mdbConnection->query(mongo_db_path);

		int clusId = 0;

		while (cursor->more())
		{
			bo       = cursor->next();
			clusterInfo.label.value = bo.getField("clusterInfo.label").Long();
			//OLM_T(LMT_User06, ("cluster[%d].clusterInfo.label:%lu", clusId, clusterInfo.label.value));
			clusterInfo.labelgroup.value = bo.getField("clusterInfo.labelgroup").Long();
			//OLM_T(LMT_User06, ("cluster[%d].clusterInfo.labelgroup:%lu", clusId, clusterInfo.labelgroup.value));
			clusterInfo.mean.value = bo.getField("clusterInfo.mean").Double();
			//OLM_T(LMT_User06, ("cluster[%d].clusterInfo.mean:%lf", clusId, clusterInfo.mean.value));
			clusterInfo.distance.value = bo.getField("clusterInfo.distance").Double();
			//OLM_T(LMT_User06, ("cluster[%d].clusterInfo.distance:%lf", clusId, clusterInfo.distance.value));
			for(int j=0; j<coordsLength; j++)
			{
#define MAXLENKEY 20
				char coordKey[MAXLENKEY];

				snprintf(coordKey, MAXLENKEY, "coord_%d", j);
				coord.value = bo.getField(coordKey).Double();
				//OLM_T(LMT_User06, ("cluster[%d].%s:%lf", clusId, coordKey, coord.value));
				clusterInfo.coords.comsAdd()->copyFrom(&coord);
			}
			clientClusters.clusterAdd()->copyFrom(&clusterInfo);
			clusId++;
		}

		delete mdbConnection;

	}

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		// Intermediate
		double mindist, dist;
		double com, ccom;
		int clusId = -1;

		//OLM_T(LMT_User06, ("Starting run() with inputs[0].num_kvs:%lu and clientClusters.cluster_length:%d", inputs[0].num_kvs, clientClusters.cluster_length));

		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			nodbts.parse(inputs[0].kvs[i]->key);
			clusVector.parse(inputs[0].kvs[i]->value);
			cluster.coords.copyFrom(&clusVector);
			mindist = 1000;

			// Calculate min dist - Euclidean distance
			for(int nClus=0; nClus<clientClusters.cluster_length; nClus++)
			{
				dist = 0;
				for(int nComs=0; nComs<clusVector.coms_length; nComs++)
				{
					ccom = clientClusters.cluster[nClus].coords.coms[nComs].value;
					com = clusVector.coms[nComs].value;
					dist += ((ccom - com) * (ccom - com));
				}
				if(dist < mindist)
				{
					mindist = dist;
					clusId = nClus;
				}
			}
			mindist = sqrt(mindist);

			node.value = nodbts.phone.value;

			// FILTER BY DISTANCE
			cluster.label = clientClusters.cluster[clusId].label;
			cluster.labelgroup = clientClusters.cluster[clusId].labelgroup;

			if(mindist > clientClusters.cluster[clusId].distance.value) // High to cluster level
			{
				cluster.confident = 0;
			}
			else
			{
				cluster.confident = 1;
			}
			cluster.distance.value = mindist;
			cluster.mean.value = 0;

			writer->emit(0, &node,&cluster);
		}

	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif
