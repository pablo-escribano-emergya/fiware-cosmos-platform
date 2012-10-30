#ifndef _H_SAMSON_QUEUE_TASK_BASE
#define _H_SAMSON_QUEUE_TASK_BASE

#include <set>                       // std::set
#include <sstream>
#include <string>
#include <vector>

#include "au/log/LogCentral.h"
#include "au/log/LogCommon.h"
#include "au/string/StringUtilities.h"               // au::Format
#include "engine/ProcessItem.h"      // engine::ProcessItem
#include "samson/common/MessagesOperations.h"
#include "samson/common/samson.pb.h"
#include "samson/stream/Block.h"     // samson::Stream::Block
#include "samson/stream/BlockList.h"  // stream::BlockList
#include "samson/stream/BlockListContainer.h"
#include "samson/worker/CommitCommand.h"

namespace samson {
class SamsonWorker;
namespace stream {
class Block;

/*
 *
 * WorkerTaskBase
 *
 * Base class for all the tasks executed in SAMSON
 *
 */

class WorkerTaskBase : public engine::ProcessItem {
  public:
    // Constructor and destructor
    WorkerTaskBase(::samson::SamsonWorker* samson_worker, size_t id, const std::string& concept , bool simple_task = false);
    virtual ~WorkerTaskBase() {}

    // Add blocks for input
    void AddInput(int channel, BlockPointer block, KVRange range, KVInfo info);

    // Add blocks for output ( generated by running the operation )
    void AddOutput(int channel, BlockPointer block, KVRange range, KVInfo info);

    bool is_ready();   // Function to check if all the blocks are in memory

  bool simple_task()
  {
    return simple_task_;
  }
  
    size_t worker_task_id() const { return id_;}  // Get the id of this task
    std::string concept() const { return concept_;}   // Get the concept ot this worker task
    std::string task_state() const { return task_state_;}
    size_t id() const { return id_;}

    // set & get over queue_task_state ( just debugging on console )
    void SetTaskState(const std::string& _queue_task_state);

    // Method to fill records with my information ( tables on delilah )
    void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization);

    // Debug strings
    virtual std::string str() = 0;   // Debug string to get a description for  last_operations command
    std::string str_inputs();
    std::string str_outputs();
  std::string str_short()
  {
    std::ostringstream output;
    output << "[" << id() << ":";
    output << task_state_;
    if( running() )
      output << "<Run>";
    if( finished() )
      output << "<finish>";
    if( IsWorkerTaskFinished())
      output << "<WT Finish>";
    output << "]";
    return output.str();
  }
  
    // Methods to activate the "finish" flag when the operation is over
    void SetWorkerTaskFinished();
    void SetWorkerTaskFinishedWithError(const std::string& error_message);
    bool IsWorkerTaskFinished() const;   // Check if it finished

    // Method to generate commit command to update data-model
    std::string
        generate_commit_command(const std::vector<std::string>& inputs, const std::vector<std::string>& outputs);

    // For statistics
    void setActivity( const std::string& name );
    std::string GetActivitySummary();
  
  protected:
    // Container for all input/output blocks
    BlockListContainer block_list_container_;

  private:
  
    // Pointer to the worker
    ::samson::SamsonWorker* samson_worker_;

    // Id of the operation
    size_t id_;

  // Simple task ( not use ProcessManager  to execute it )
  bool simple_task_;
  
    // Concept
    std::string concept_;

    // State information
    std::string task_state_;

    // Flag to determine if this task is ready ( all blocks in memory )
    bool ready_;

    // Flag to indicate that the operation has finished
    bool worker_task_finished_;

    // BLocks generated by this tasks
    std::vector<size_t> output_block_ids_;
  
  // Statistics
  au::statistics::ActivityMonitor activity_monitor_;
};
}
}
#endif  // ifndef _H_SAMSON_QUEUE_TASK_BASE
