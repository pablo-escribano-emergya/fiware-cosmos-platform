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


/* ****************************************************************************
*
* FILE            Pugi.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         pugi library
*
* DATE            7/15/11
*
* DESCRIPTION
*
*  Usefull functions to simplify development with pugi library
*  Everything ready for running xpath queries over xml data
*
* ****************************************************************************/

#ifndef _H_xml_Pugi
#define _H_xml_Pugi


#include <set>
#include <sstream>          // std::ostringstream
#include <string>           // std::string
#include <vector>

#include "au/string/StringUtilities.h"
#include "au/tables/pugixml.hpp"     // pugi::...

namespace au {
namespace tables {
class TreeItem;
}
}

/**
 * \brief Namespace for manipulation of objectes generated by pugi library
 * \auth Andreu Urruela
 */

namespace pugi {
// Working with TreeItems
au::tables::TreeItem *treeItemFromDocument(const xml_document& xml_doc);
au::tables::TreeItem *treeItemFromNode(const xml_node& xml_node);
}
#endif  // ifndef _H_xml_Pugi
