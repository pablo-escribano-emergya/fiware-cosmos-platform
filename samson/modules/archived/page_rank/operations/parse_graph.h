/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_page_rank_parse_graph
#define _H_SAMSON_page_rank_parse_graph

#include <samson/module/samson.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/StringVector.h>
#include <samson/modules/system/SimpleParser.h>

namespace samson {
namespace page_rank {

class parse_graph : public samson::system::SimpleParser {

    samson::system::String key_;
    samson::system::StringVector value_;

  public:

#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
    // If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

    output: system.String system.StringVector

    helpLine: Parse a txt file where wvery line is node link1 link2 ....
#endif // de INFO_COMMENT

    void init(samson::KVWriter *writer) {
    }

    void parseLine(char *data, samson::KVWriter *writer) {

      std::stringstream ss(data);

      if (!std::getline(ss, key_.value, ' '))
        return; // Key not present


      value_.valuesSetLength(0); // Clear vector

      std::string item;
      while (std::getline(ss, item, ' '))
        value_.valuesAdd()->value = item;

      writer->emit(0, &key_, &value_);

    }

    void finish(samson::KVWriter *writer) {
    }

};

} // end of namespace page_rank
} // end of namespace samson

#endif
