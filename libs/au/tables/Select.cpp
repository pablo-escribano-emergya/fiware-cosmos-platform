
#include <algorithm>
#include <fnmatch.h>

#include "Table.h"
#include "Tree.h"

#include "Select.h" // Own interface

NAMESPACE_BEGIN(au)
NAMESPACE_BEGIN(tables)

bool compare_strings( std::string& pattern , std::string& value)
{
    return ( ::fnmatch( pattern.c_str() , value.c_str() , 0 ) == 0 );
}


#pragma mark SelectRowCondition

bool SelectCondition::check( TreeItem *tree )
{
    std::string _value = tree->getFirstNodeValue(name);
    return compare_strings( value , _value );
}





NAMESPACE_END
NAMESPACE_END