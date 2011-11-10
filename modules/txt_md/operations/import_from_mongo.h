
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_txt_import_from_mongo
#define _H_SAMSON_txt_import_from_mongo


#include <iostream>
#include <samson/module/samson.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>
#include "mongo/client/dbclient.h"
#include "mongo/client/dbclientcursor.h"

using namespace mongo;


namespace samson
{
namespace txt_md
{


class import_from_mongo : public samson::Generator
{
	std::string          mongo_ip;
	std::string          mongo_db;
	std::string          mongo_collection;
	std::string          mongo_db_path;
	DBClientConnection*  mdbConnection;
	bool                 active;

public:


/* ****************************************************************************
*
* init - 
*/
void init(samson::KVWriter* writer)
{
	mongo_ip           = environment->get("mongo.ip",    "no-mongo-ip");
	mongo_db           = environment->get("mongo.db",    "no-mongo-db");
	mongo_collection   = environment->get("mongo.collection", "no-mongo-collection");

	mongo_db_path = mongo_db + "." + mongo_collection;

	mdbConnection = new DBClientConnection();
	mdbConnection->connect(mongo_ip);

	active = false;
}



/* ****************************************************************************
*
* setup - 
*/
void setup(int worker, int num_workers, int process, int num_processes)
{
	// Remember that num_workers * num_processes operation will be executed in parallel

	if ((worker == 0) && (process == 0))
		active = true;
	else
		active = false;
}



/* ****************************************************************************
*
* run - 
*/
void run(samson::KVWriter* writer)
{
	if (active == false)
		return;

	if (mongo_ip == "no-mongo-ip")
	{
		tracer->setUserError("No mongo ip is specified. Please specify mongo ip with 'mongo.ip' environment variable");
		return;
	}

	if (mongo_db == "no-mongo-db")
	{
		tracer->setUserError("No db specified. Please specify mongo database db name with 'mongo.db' environment variable");
		return;
	}

	if (mongo_collection == "no-mongo-collection")
	{
		tracer->setUserError("No collection is specified. Please specify mongo collection with 'mongo.collection' environment variable");
		return;
	}

	// Access data and emit with "writer" object
	BSONObj                 bo;
	samson::system::String  K;
	samson::system::UInt    V;

	auto_ptr<DBClientCursor> cursor = mdbConnection->query(mongo_db_path);

	while (cursor->more())
	{
		bo       = cursor->next();
		K.value  = bo.getStringField("K");
		V.value  = bo.getIntField("V");

		writer->emit(0, &K, &V);
	}
}



/* ****************************************************************************
*
* finish - 
*/
void finish(samson::KVWriter* writer)
{
}

};


} // end of namespace txt_md
} // end of namespace samson

#endif