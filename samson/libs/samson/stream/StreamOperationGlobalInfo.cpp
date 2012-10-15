#include "samson/stream/StreamOperationGlobalInfo.h"      // Own interface

#include <algorithm>

#include "engine/Engine.h"
#include "engine/ProcessManager.h"
#include "samson/common/gpb_operations.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/module/ModulesManager.h"
#include "samson/stream/StreamOperationRangeInfo.h"
#include "samson/stream/WorkerTask.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
namespace stream {

/*
 * classes BlockRange and BlockRangeCollection are used only locally to compute how to organize defrag operations
 */

class BlockRange {
  public:

    BlockRange(size_t block_id, KVRange range, size_t size) {
      block_id_ = block_id;
      range_ = range;
      size_ = size;
    }

    bool operator<(const BlockRange& block_range) const {
      return range_.hg_begin < block_range.range_.hg_begin;
    }

    size_t block_id() {
      return block_id_;
    }

    KVRange range() {
      return range_;
    }

    size_t size() {
      return size_;
    }

  private:

    size_t block_id_;
    KVRange range_;
    size_t size_;

};

class BlockRangeCollection : public std::vector<BlockRange> {

  public:

    void add(size_t block_id, KVRange range, size_t size) {
      push_back(BlockRange(block_id, range, size));
    }

    BlockRange get() {
      BlockRange block_range = at(size() - 1);
      pop_back();
      return block_range;
    }

};

StreamOperationGlobalInfo::StreamOperationGlobalInfo(SamsonWorker *samson_worker, size_t stream_operation_id,
                                                     const std::string& stream_operation_name,
                                                     const std::vector<KVRange>& ranges) {
  samson_worker_ = samson_worker;
  stream_operation_id_ = stream_operation_id;
  stream_operation_name_ = stream_operation_name;
  ranges_ = ranges;
  division_factor_ = 1;
}

void StreamOperationGlobalInfo::schedule_defrag(gpb::Data *data) {
  if (defrag_tasks_.size() > 0) {
    // There are previous defrags going on...y
    return;
  }

  // Recover stream operation from data
  gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, stream_operation_id_);
  if (!stream_operation) {
    LM_W(("Stream operation %s not found when trying to defrag"));
    return;
  }

  // Reset error in timeout 60
  if ((error_.IsActivated() && (cronometer_error_.seconds() > 60))) {
    error_.Reset();
  }

  if (error_.IsActivated())
    return;

  // Check if the operatio is valid
  au::ErrorManager error;
  if (!isStreamOperationValid(data, *stream_operation, &error)) {
    SetError(error.GetMessage());
    return;
  }

  // If operation is paused, do not consider then...
  if (stream_operation->paused()) {
    state_ = "Operation paused";
    return;
  }

  // In batch operation, defrag all input queues ( if necessary )
  bool batch_operation = stream_operation->batch_operation();

  if (!batch_operation) {
    SetError("Trying to defrag a non batch operation");
    return;
  }

  for (int i = 0; i < stream_operation->inputs_size(); i++) {
    std::string input_queue = stream_operation->inputs(i);
    schedule_defrag(data, input_queue);

  }
}

void StreamOperationGlobalInfo::schedule_defrag(gpb::Data* data, const std::string& queue_name) {

  gpb::Queue *queue = ::samson::gpb::get_queue(data, queue_name);
  if (!queue) {
    return; // No data at this input
  }

  // Prepare a collection with all blocks and ranges
  BlockRangeCollection block_range_collection;

  for (int b = 0; b < queue->blocks_size(); b++) {

    const gpb::Block& block = queue->blocks(b);

    size_t block_id = block.block_id();
    size_t memory_size = block.block_size();
    KVRanges ranges = block.ranges();
    KVRange range = ranges.max_range();
    block_range_collection.add(block_id, range, memory_size);
  }

  // Sort blocks so
  std::sort(block_range_collection.begin(), block_range_collection.end());

  // Create candidate task
  au::SharedPointer<DefragTask> defrag_task(
                                            new DefragTask(samson_worker_, queue_name,
                                                           samson_worker_->task_manager()->getNewId()));
  size_t max_memory = 0.5 * (double) engine::Engine::shared()->memory_manager()->memory();
  size_t accumulated_size = 0;

  while (block_range_collection.size() > 0) {

    BlockRange block_range = block_range_collection.get();

    // Add this block to the current task
    BlockPointer real_block = BlockManager::shared()->GetBlock(block_range.block_id());

    if (real_block == NULL) {
      error_.set(au::str("Block %lu not found but present in queue %s", block_range.block_id(), queue_name.c_str()));
      return;
    }

    defrag_task->AddInput(0, real_block, block_range.range(), KVInfo(0, 0));
    accumulated_size += block_range.size();

    if (accumulated_size > max_memory) {
      accumulated_size = 0;

      // Schedule task
      samson_worker_->task_manager()->Add(defrag_task.dynamic_pointer_cast<WorkerTaskBase> ());
      defrag_tasks_.push_back(defrag_task);

      // Create a new task
      defrag_task.Reset(new DefragTask(samson_worker_, queue_name, samson_worker_->task_manager()->getNewId()));
    }

  }

  // Schedule task
  samson_worker_->task_manager()->Add(defrag_task.dynamic_pointer_cast<WorkerTaskBase> ());
  defrag_tasks_.push_back(defrag_task);

}

void StreamOperationGlobalInfo::ReviewCurrentTasks() {
  bool all_defrag_tasks_finished = true;
  for (size_t i = 0; i < defrag_tasks_.size(); i++) {
    if (defrag_tasks_[i] == NULL)
      continue; // Previously finished tasks
    all_defrag_tasks_finished = false;

    if (!defrag_tasks_[i]->finished())
      continue;

    if (defrag_tasks_[i]->error().IsActivated()) {
      LM_W(("Defrag task ended with error: %s" , defrag_tasks_[i]->error().GetMessage().c_str() ));
    } else {
      // Commit changes and release task

      std::string commit_command = defrag_tasks_[i]->commit_command();
      std::string caller = au::str("task %lu // Defrag %s", defrag_tasks_[i]->worker_task_id(), str().c_str());
      au::ErrorManager error;
      samson_worker_->data_model()->Commit(caller, commit_command, &error);
      if (error.IsActivated()) {
        LM_W(("Defrag task error in commit: %s" , error.GetMessage().c_str() ));
      }

    }

    // Remove defrag task
    defrag_tasks_[i].Reset(NULL);

  }

  if (all_defrag_tasks_finished)
    defrag_tasks_.clear();

}

std::string StreamOperationGlobalInfo::str() {
  std::ostringstream output;
  output << "StreamOperation " << stream_operation_id_;
  output << " " << stream_operation_name_;
  return output.str();
}

void StreamOperationGlobalInfo::Review(gpb::Data *data , int num_running_operations ) {
  
  // Review current tasks
  ReviewCurrentTasks();

  size_t max_memory_per_task = GetMaxMemoryPerTask();
  
  gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data, stream_operation_id_);

  bool batch_operation = stream_operation->batch_operation();
  bool reduce_forward = stream_operation->reduce_forward();

  state_ = "";
  state_input_queues_ = "";

  // All ranges currently in use
  std::vector<KVRange> all_ranges = ranges();

  // Information about input queues...
  // Scan all inputs
  for (int i = 0; i < stream_operation->inputs_size(); i++)
  {
    std::string input_queue = stream_operation->inputs(i);
    gpb::DataInfoForRanges info = gpb::get_data_info_for_ranges(data, input_queue, all_ranges);
    
    state_input_queues_ += au::str("[%s %s %s]"
                                   , input_queue.c_str()
                                   , au::str(info.data_kvs_in_ranges,"kvs").c_str()
                                   , au::str(info.data_size_in_ranges,"B").c_str()
                                   );
    
  }
  
  // If running defrag tasks, wait for them...
  if (defrag_tasks_.size() > 0) {
    state_ = au::str("Running %lu defrag tasks" , defrag_tasks_.size() );
    execute_range_operations_ = false;
    return;
  }
  
  if (batch_operation) {
  
    // Consider all input queues
    std::vector<std::string> all_queues;
    for (int i = 0; i < stream_operation->inputs_size(); i++)
      all_queues.push_back(stream_operation->inputs(i));

    // Global information for all queues
    gpb::DataInfoForRanges info = gpb::get_data_info_for_ranges(data, all_queues, all_ranges);
    
    if (info.max_memory_size_for_a_range > max_memory_per_task )
    {
      // too much memory for standalone application
      execute_range_operations_ = false; // In all cases, individual operations should not be scheduled
      
      if (info.max_data_size_for_a_range > max_memory_per_task )
      {
        // defrag is not enougth, we need to increase the division factor...
        state_ = "Increasing division factor...";
        division_factor_ *= 2;
        return;
      }
      
      // Defrag is necessary
      if( num_running_operations == 0 )
      {
        // Schedule defrag operation only if normal range tasks are finished
        state_ = "Scheduling defrag operations...";
        schedule_defrag(data);
      }
      else
        state_ = au::str("Waiting %lu tasks to run defrag over input queus",num_running_operations);
      
      return;
    }
    
    execute_range_operations_ = true; // Authorize range operations to be executed
    state_ = au::str("OK. Max Data per range %s / Max Memory per range %s]"
                     , au::str(info.max_data_size_for_a_range).c_str()
                     , au::str(info.max_memory_size_for_a_range).c_str());
    
    return;
  }
    
  
  if (reduce_forward) {
    
    state_ = "Still not implemented how to defrag in reduce forward";
    return;
  }

  
  // Normal stream operations
  
  // Consider only the last queue ( state ) to increase the number of divisions
  std::string state_queue = stream_operation->inputs( stream_operation->inputs_size() - 1 );
  
  // Get information for this queue in this ranges
  gpb::DataInfoForRanges state_info = gpb::get_data_info_for_ranges(data, state_queue, all_ranges);
  
  if( state_info.max_memory_size_for_a_range > (max_memory_per_task/2) )
  {
    execute_range_operations_ = false;
    division_factor_ *= 2;
    state_ = "Increasing number of divisions";
  }
  else
  {
    execute_range_operations_ = true; // Authorize range operations to be executed
    state_ = au::str("OK. Max State Data per range %s / Max State Memory per range %s]"
                     , au::str(state_info.max_data_size_for_a_range).c_str()
                     , au::str(state_info.max_memory_size_for_a_range).c_str());
  }
  
}

void StreamOperationGlobalInfo::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {

  ::samson::add(record, "id", stream_operation_id_, "different");
  ::samson::add(record, "name", stream_operation_name_, "different");
  ::samson::add(record, "division", division_factor_, "different");
  ::samson::add(record, "Allow running", execute_range_operations_ ? "YES" : "NO", "different");

  if( visualization.get_flag("state"))
  {
    ::samson::add(record, "state", state_, "different");
    return;
  }
  
  if(  defrag_tasks_.size() > 0 )
    ::samson::add(record, "Defrag", au::str("%lu operations", defrag_tasks_.size()), "different");
  else
    ::samson::add(record, "Defrag", "none", "different");

  ::samson::add(record, "inputs", state_input_queues_, "different");
  ::samson::add(record, "state", state_, "different");

}

}
} // End of namespace samson::stream
