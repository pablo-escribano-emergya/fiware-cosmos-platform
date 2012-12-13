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
#include "samson/worker/WorkerCommand.h"     // Own interface

#include <algorithm>

#include "au/ErrorManager.h"
#include "au/string/StringUtilities.h"
#include "au/utils.h"

#include "engine/DiskManager.h"
#include "engine/Notification.h"
#include "engine/ProcessManager.h"

#include "samson/common/EnvironmentOperations.h"        // copyEnvironment
#include "samson/common/Logs.h"
#include "samson/common/SamsonSetup.h"                  // SamsonSetup
#include "samson/delilah/DelilahCommandCatalogue.h"
#include "samson/network/Packet.h"                      // samson::Packet
#include "samson/stream/Block.h"
#include "samson/stream/BlockList.h"
#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
bool ignoreCommand(std::string command) {
  if (command.length() == 0) {
    return true;
  }

  for (size_t i = 0; i < command.length(); ++i) {
    if (command[i] != ' ') {
      if (command[i] == '#') {
        return true;
      } else {
        return false;
      }
    }
  }

  return true;
}

class AliasManager {
  au::simple_map<std::string, std::string> aliases;

public:
  void add(std::string reference, std::string name) {
    aliases.insertInMap(reference, name);
  }

  std::string transform(std::string command) {
    au::simple_map<std::string, std::string>::iterator it_aliases;

    for (it_aliases = aliases.begin(); it_aliases != aliases.end(); ++it_aliases) {
      au::FindAndReplaceInString(command, it_aliases->first, it_aliases->second);
    }

    return command;
  }
};

WorkerCommand::WorkerCommand(SamsonWorker *samson_worker, std::string worker_command_id, size_t delilah_id,
                             size_t delilah_component_id, const gpb::WorkerCommand& command) {
  LOG_D(logs.worker_command, ("New WorkerCommand %s ( Delilah %s : %lu )"
                              , worker_command_id.c_str()
                              , au::code64_str(delilah_id).c_str()
                              , delilah_component_id_));

  // Keep pointer to samson worker
  samson_worker_ = samson_worker;

  // Unique identifier of the worker
  worker_command_id_ = worker_command_id;

  // Identifiers to notify when finished
  delilah_id_ = delilah_id;
  delilah_component_id_ = delilah_component_id;

  notify_finish_ = (delilah_id_ != 0);   // 0 used as a non notify delilah

  // Copy the original message
  originalWorkerCommand_.Reset(new gpb::WorkerCommand());
  originalWorkerCommand_->CopyFrom(command);

  // Extract environment properties
  copyEnviroment(originalWorkerCommand_->environment(), &enviroment_);

  // Extract command for simplicity
  command_ = originalWorkerCommand_->command();

  // Original value for the flags
  pending_to_be_executed_ = true;
  finished_ = false;

  // No pending process at the moment
  num_pending_processes_ = 0;
  num_pending_disk_operations_ = 0;
}

WorkerCommand::~WorkerCommand() {
  LOG_D(logs.worker_command, ("Destructor WorkerCommand %s", worker_command_id_.c_str()));
}

bool WorkerCommand::finished() {
  return finished_;
}

void WorkerCommand::RunCommand(std::string command, au::ErrorManager& error) {
  if (ignoreCommand(command)) {
    return;
  }

  LOG_V(logs.worker_command, ("[%s] Running command %s", worker_command_id_.c_str(), command.c_str()));

  // Parse command
  au::CommandLine cmd;
  cmd.SetFlagBoolean("clear_inputs");
  cmd.SetFlagBoolean("f");

  cmd.SetFlagBoolean("v");
  cmd.SetFlagBoolean("vv");
  cmd.SetFlagBoolean("vvv");

  cmd.SetFlagBoolean("new");
  cmd.SetFlagBoolean("remove");
  cmd.SetFlagString("prefix", "", au::CommandLine::kCollisionInsertAtBegin);
  cmd.Parse(command, false);

  std::string prefix = cmd.GetFlagString("prefix");

  if (cmd.get_num_arguments() == 0) {
    error.AddError("No command provided");
    return;
  }

  // Set the main command
  std::string main_command = cmd.get_argument(0);

  if (main_command == "init_stream") {
    if (cmd.get_num_arguments() < 2) {
      error.AddError(
        au::str("Not enough parameters for command 'init_stream' ( only %d argument provided )",
                cmd.get_num_arguments()));
      return;
    }

    std::string operation_name;

    if (cmd.get_num_arguments() == 3) {
      prefix.append(cmd.get_argument(1));
      prefix.append(".");

      operation_name = cmd.get_argument(2);
    } else if (cmd.get_num_arguments() == 2) {
      operation_name = cmd.get_argument(1);
    }

    Operation *op = au::Singleton<ModulesManager>::shared()->GetOperation(operation_name);
    if (!op) {
      error.AddError(au::str("Unknown operation:'%s' in command arguments to init_stream", operation_name.c_str()));
      return;
    }

    if (op->getType() != Operation::script) {
      error.AddError(
        au::str("Invalid operation %d. Only script operations supported for init_stream command",
                operation_name.c_str()));
      return;
    }

    // Structure to set names alias
    AliasManager alias_manager;

    // Read code, execute it recursively
    au::ErrorManager sub_error;
    for (size_t i = 0; i < op->code.size(); ++i) {
      std::string sub_command = op->code[i];

      au::CommandLine intern_cmdLine;
      intern_cmdLine.Parse(sub_command, false);  // strict mode i disabled to avoid unnecessary warnings

      if (intern_cmdLine.get_argument(0) == "alias") {
        if (intern_cmdLine.get_num_arguments() < 3) {
          error.AddError(au::str("Not enough parameters for command %s", main_command.c_str()));
          return;
        }

        std::string reference = intern_cmdLine.get_argument(1);
        std::string value = intern_cmdLine.get_argument(2);

        alias_manager.add(reference, value);
      } else {
        std::string full_command = alias_manager.transform(sub_command);

        if (prefix.length() > 0) {
          full_command.append(au::str(" -prefix %s", prefix.c_str()));
        }
        RunCommand(full_command, sub_error);
      }

      if (sub_error.HasErrors()) {
        error.AddError(au::str("[%s:%d]%s", operation_name.c_str(), i, sub_error.GetLastError().c_str()));
        return;
      }
    }

    return;
  }

  // If operation can be process by DataMode, go ahead
  if (samson_worker_->data_model()->IsValidCommand(main_command)) {
    std::string caller = au::str("Command %s from delilah %s", main_command.c_str(), au::code64_str(delilah_id_).c_str());
    samson_worker_->data_model()->Commit(caller, command, error);
    return;
  }

  // Unknown command error message
  LOG_E(logs.worker_command, ("Unknown command %s", main_command.c_str()));
  error.AddError(au::str("Unknown command %s", main_command.c_str()));
}

bool compare_blocks_defrag(stream::Block *b, stream::Block *b2) {
  KVHeader h1 = b->getHeader();
  KVHeader h2 = b2->getHeader();

  if (h1.range.hg_begin_ < h2.range.hg_begin_) {
    return true;
  }
  if (h1.range.hg_begin_ > h2.range.hg_begin_) {
    return false;
  }

  // If the same
  if (h1.range.hg_end_ > h2.range.hg_end_) {
    return true;
  } else {
    return false;
  }
}

typedef struct LogLineInfo {
  char type;
  std::string date;
  int ms;
  std::string progName;
  std::string fileName;
  int lineNo;
  int pid;
  int tid;
  std::string funcName;
  std::string message;
} LogLineInfo;

void WorkerCommand::Run() {
  // Nothing to to if this is waiting for another thing
  if (!pending_to_be_executed_) {
    return;
  }

  LOG_V(logs.worker_command, ("[%s] Run", worker_command_id_.c_str()));

  pending_to_be_executed_ = false;   // Not pending any more, except if something happen...

  // Parse a delilah command
  DelilahCommandCatalogue delilah_command_catalogue;
  au::SharedPointer<au::console::CommandInstance> command_instance = delilah_command_catalogue.Parse(command_, error_);
  if (error_.HasErrors()) {
    return;   // Finish with this error
  }
  // General visualization options
  Visualization visualization;

  // Add all bool flags like -v -state automatically to visualization
  const std::vector<au::console::CommandItem *> options = command_instance->command()->options();
  for (size_t i = 0; i < options.size(); ++i) {
    if (options[i]->type() == au::console::options::option_bool) {
      std::string name = options[i]->name();
      visualization.set_flag(name, command_instance->GetBoolOption(name));
    }
  }

  // Add argument pattern if exist
  if (command_instance->HasStringArgument("pattern")) {
    visualization.set_pattern(command_instance->GetStringArgument("pattern"));   // Get main command
  }
  std::string main_command = command_instance->main_command();

  // TODO(@jges): Remove log message
  LOG_D(logs.worker_command, ("Processing '%s' command", main_command.c_str()));

  if (main_command == "ls") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->data_model()->GetCollectionForQueues(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "data_model_status") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->GetCollectionForDataModelStatus(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "data_model_commits") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->GetCollectionForDataModelCommits(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_queue_ranges") {
    std::string queue_name = command_instance->GetStringArgument("name");
    au::SharedPointer<gpb::Collection> c = samson_worker_->data_model()->GetCollectionForQueueRanges(visualization,
                                                                                                     queue_name);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "get_replication_factor") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->data_model()->GetCollectionForReplication(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_queue_blocks") {
    au::SharedPointer<gpb::Collection> c =
      samson_worker_->data_model()->GetCollectionForQueuesWithBlocks(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_last_commits_debug") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->data_model()->GetLastCommitsDebugCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_last_commits") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->data_model()->GetLastCommitsCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_last_tasks") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->task_manager()->GetLastTasksCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_stream_operations_statistics") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->task_manager()->GetSOStatisticsCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_block_requests") {
    au::SharedPointer<gpb::Collection> c =
      samson_worker_->worker_block_manager()->GetCollectionForBlockRequests(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_block_defrags") {
    au::SharedPointer<gpb::Collection> c =
      samson_worker_->worker_block_manager()->GetCollectionForBlockDefrags(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_blocks") {
    au::SharedPointer<gpb::Collection> c = stream::BlockManager::shared()->GetCollectionOfBlocks(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_buffers") {
    au::SharedPointer<gpb::Collection> c = GetCollectionOfBuffers(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_pop_connections") {
    /*
     * gpb::Collection * c = streamManager->GetCollectionForPopConnections(&visualitzation);
     * c->set_title( command  );
     * collections_.push_back( c );
     */
    error_.AddError("Unimplemented");
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_stream_operations") {
    au::SharedPointer<gpb::Collection> c =
      samson_worker_->data_model()->GetCollectionForStreamOperations(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_batch_operations") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->data_model()->GetCollectionForBatchOperations(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ps_stream_operations_ranges") {
    au::SharedPointer<gpb::Collection> c =
      samson_worker_->task_manager()->GetCollectionForStreamOperationsRanges(visualization);
    c->set_title("List of stream-operation ranges ( R= Ready // P: Processing )");
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ps_stream_operations") {
    au::SharedPointer<gpb::Collection> c =
      samson_worker_->task_manager()->GetCollectionForStreamOperations(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_queue_connections") {
    au::SharedPointer<gpb::Collection> c =
      samson_worker_->data_model()->GetCollectionForQueueConnections(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ps_tasks") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->task_manager()->GetCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_workers") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->GetWorkerCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "push_internal_modules") {
    std::string directory = au::Singleton<SamsonSetup>::shared()->samson_home() + "/modules";
    std::vector<std::string> files =  au::getRegularFilesFromDirectory(directory);


    au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
    collection->set_name("modules");
    collection->set_title(command_);

    for (size_t i = 0; i < files.size(); ++i) {
      gpb::CollectionRecord *record = collection->add_record();
      add(record, "File", au::GetLastPathComponent(files[i]), "left");

      size_t size = au::sizeOfFile(files[i]);

      if (size == 0) {
        add(record, "Result", "Error, size=0", "left");
        continue;
      }
      if (size > 200000000) {
        add(record, "Result", au::str("Error, excesive size %lu", size), "left");
        continue;
      }

      engine::BufferPointer buffer(engine::Buffer::Create("push_internal_modules", size + sizeof(KVHeader)));
      au::ErrorManager error;
      KVHeader *header = reinterpret_cast<KVHeader *>(buffer->data());
      buffer->SkipWrite(sizeof(KVHeader));
      buffer->WriteFromFile(files[i], error);
      header->InitForModule(size);

      if (error.HasErrors()) {
        add(record, "Result", error.GetLastError(), "left");
        continue;
      }

      std::vector<std::string> queues;
      queues.push_back(".modules");
      samson_worker_->worker_block_manager()->ReceivedPushBlock(-1, -1, buffer, queues);
      add(record, "Result", "OK", "left");
    }
    collections_.push_back(collection);
    FinishWorkerTask();
    return;
  }


  if (main_command == "ls_kv_ranges") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->GetKVRangesCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "wlog_all_channels") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->GetWorkerAllLogChannels(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "wlog_status") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->GetWorkerLogStatus(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "wlog_set") {
    std::string channel_pattern_string = command_instance->GetStringArgument("channel_pattern");
    std::string str_log_level = command_instance->GetStringArgument("log_level");

    au::log_central->EvalCommand("log_set " + channel_pattern_string + " " + str_log_level + " server");
    FinishWorkerTask();
    return;
  }

  if (main_command == "wlog_set_log_server") {
    std::string host = command_instance->GetStringArgument("host");

    au::log_central->RemovePlugin("server");
    au::log_central->AddServerPlugin("server", host, host + "_local_log.log");
    au::log_central->EvalCommand("log_set * X server");
    au::log_central->EvalCommand("log_set samson::W M server");
    au::log_central->EvalCommand("log_set samson::OP W server");


    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_modules_information") {
    // Show information about loading process
    au::SharedPointer<gpb::Collection> c = samson_worker_->GetModulesCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_modules") {
    au::SharedPointer<gpb::Collection> c = au::Singleton<ModulesManager>::shared()->GetModulesCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }
  if (main_command == "ls_operations") {
    au::SharedPointer<gpb::Collection> c =
      au::Singleton<ModulesManager>::shared()->GetOperationsCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }
  if (main_command == "ls_datas") {
    au::SharedPointer<gpb::Collection> c = au::Singleton<ModulesManager>::shared()->GetDatasCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_connections") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->network()->GetConnectionsCollection(visualization);
    if (c != NULL) {
      c->set_title(command_);
      collections_.push_back(c);
    }
    FinishWorkerTask();
    return;
  }

  if (main_command == "ls_network_queues") {
    au::SharedPointer<gpb::Collection> c = samson_worker_->network()->GetQueuesCollection(visualization);
    if (c != NULL) {
      c->set_title(command_);
      collections_.push_back(c);
    }
    FinishWorkerTask();
    return;
  }

  if (main_command == "ps_workers") {
    au::SharedPointer<gpb::Collection> c =
      samson_worker_->workerCommandManager()->GetCollection(visualization);
    c->set_title(command_);
    collections_.push_back(c);
    FinishWorkerTask();
    return;
  }

  if (main_command == "wait") {
    if (samson_worker_->data_model()->CheckForAllStreamOperationsFinished() == false) {
      pending_to_be_executed_ = true;
      return;
    }

    // Nothing else to be waited
    FinishWorkerTask();
    return;
  }

  if (main_command == "wait_batch_tasks") {
    // Recovering old wait command
    if (samson_worker_->data_model()->CheckForAllBatchOperationsFinished() == false) {
      pending_to_be_executed_ = true;
      return;
    }

    // Nothing else to be waited
    FinishWorkerTask();
    return;
  }

  if (main_command == "wait_my_batch_tasks") {
    // Recovering old wait command
    if (samson_worker_->data_model()->CheckForAllBatchOperationsFinished(delilah_id_) == false) {
      pending_to_be_executed_ = true;
      return;
    }

    // Nothing else to be waited
    FinishWorkerTask();
    return;
  }

  if (main_command == "send_alert") {
    std::string message = command_instance->GetStringArgument("message");
    bool error = command_instance->GetBoolOption("-error");
    bool warning = command_instance->GetBoolOption("-error");

    // Full message
    std::string full_message = au::str("[Alert from Delilah_%s] %s", au::code64_str(delilah_id_).c_str(),
                                       message.c_str());

    // Send a trace to all delilahs
    if (error) {
      samson_worker_->network()->SendAlertToAllDelilahs("error", "delilah", full_message);
    } else if (warning) {
      samson_worker_->network()->SendAlertToAllDelilahs("warning", "delilah", full_message);
    } else {
      samson_worker_->network()->SendAlertToAllDelilahs("message", "delilah", full_message);
    }
    FinishWorkerTask();
    return;
  }

  /**
   * "run" calls are transformed into "batch" to add extra information about delilah client
   * the "batch" command cannot be called directly from delilah
   */

  if (main_command == "batch") {
    FinishWorkerTaskWithError("the 'batch' command cannot be called from delilah");
    return;
  }

  if (main_command == "run") {
    std::string operation = command_instance->GetStringArgument("operation");
    std::string inputs = command_instance->GetStringOption("input");
    std::string outputs = command_instance->GetStringOption("output");
    std::string env = command_instance->GetStringOption("env");

    std::string command = au::str(
      "batch %s -input \"%s\" -output \"%s\" -delilah_id %lu -delilah_component_id %lu -env \"%s\"",
      operation.c_str(), inputs.c_str(), outputs.c_str(), delilah_id_,
      delilah_component_id_, env.c_str());

    au::ErrorManager error;
    std::string caller = au::str("run_deliah_%s_%lu", au::code64_str(delilah_id_).c_str(), delilah_component_id_);
    samson_worker_->data_model()->Commit(caller, command, error);

    if (error.HasErrors()) {
      LOG_E(logs.worker_command,
            ("Error in Commit for command:'%s', error: '%s'", command.c_str(), error.GetLastError().c_str()));
      FinishWorkerTaskWithError(error.GetLastError());
    } else {
      FinishWorkerTask();
    }
    return;
  }

  // Simple commands directly to data model
  RunCommand(command_, error_);
  if (error_.HasErrors()) {
    LOG_E(logs.worker_command,
          ("Error in Commit for command:'%s', error: '%s'", command_.c_str(), error_.GetLastError().c_str()));
  }
  FinishWorkerTask();
}

void WorkerCommand::FinishWorkerTaskWithError(std::string error_message) {
  LOG_D(logs.worker_command, ("[%s] Finished with error %s ", worker_command_id_.c_str(), error_message.c_str()));

  error_.AddError(error_message);
  FinishWorkerTask();

  // Notify everything so it is automatically canceled
  engine::Notification *notification = new engine::Notification("cancel");
  notification->environment().Set("id", worker_command_id_);
  engine::Engine::shared()->notify(notification);
}

void WorkerCommand::FinishWorkerTask() {
  if (finished_) {
    // If this worker command is finished, not do anything else again.
    return;
  }

  LOG_D(logs.worker_command, ("[%s] Finished OK", worker_command_id_.c_str()));

  if (notify_finish_) {
    LOG_D(logs.worker_command, ("notify_finish for command:'%s', delilah_id:%lu", command_.c_str(), delilah_id_));
    PacketPointer p(new Packet(Message::WorkerCommandResponse));
    gpb::WorkerCommandResponse *c = p->message->mutable_worker_command_response();
    c->mutable_worker_command()->CopyFrom(*originalWorkerCommand_);

    // Put warnings and errors ( if any ) into the message for delilah client
    for (size_t i = 0; i < error_.items().size(); ++i) {
      switch (error_.items()[i]->type()) {
        case au::error:
          c->add_error(error_.items()[i]->message());
          break;

        case au::warning:
          c->add_warning(error_.items()[i]->message());
          break;

        case au::message:
          // Currently not transmitted to delilah
          break;

        default:
          break;
      }
    }

    // Set delilah id
    p->message->set_delilah_component_id(delilah_component_id_);

    // Direction of this packets
    p->to.node_type = DelilahNode;
    p->to.id = delilah_id_;

    // Add collections as answers...
    for (size_t i = 0; i < collections_.size(); ++i) {
      p->message->add_collection()->CopyFrom(*collections_[i]);
    }

    // Send the packet
    samson_worker_->network()->Send(p);
  }

  // Set the finished flag
  finished_ = true;
}

void WorkerCommand::notify(engine::Notification *notification) {
  if (notification->isName(notification_process_request_response)) {
    LOG_D(logs.worker_command, ("Notification about finished process"));
    --num_pending_processes_;
    if (notification->environment().IsSet("error")) {
      error_.AddError(notification->environment().Get("error", "no_error"));
    }
    CheckFinish();
    return;
  }

  LOG_W(logs.worker_command, ("Unexpected notification at WorkerCommand"));
}

void WorkerCommand::CheckFinish() {
  if (error_.HasErrors()) {
    FinishWorkerTask();
  } else if ((num_pending_processes_ <= 0) && (num_pending_disk_operations_ <= 0)) {
    FinishWorkerTask();
  }
}

void WorkerCommand::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
  std::string name = NodeIdentifier(DelilahNode, delilah_id_).getCodeName();

  add(record, "id", worker_command_id_, "left,different");

  if (finished_) {
    add(record, "status", "finished", "left,different");
  } else {
    add(record, "status", "running", "left,different");
  }
  add(record, "command", command_, "left,different");
  add(record, "#operations", num_pending_processes_, "left,uint64,sum");
  add(record, "#disk_operations", num_pending_disk_operations_, "left,uint64,sum");
  add(record, "error", error_.GetLastError(), "left,different");
}

au::SharedPointer<gpb::Collection> WorkerCommand::GetCollectionOfBuffers(const Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("buffers");

  au::tables::Table table = engine::Engine::memory_manager()->getTableOfBuffers();

  // Debug
  // printf("%s\n" , table.str().c_str() );

  for (size_t r = 0; r < table.getNumRows(); ++r) {
    gpb::CollectionRecord *record = collection->add_record();

    for (size_t c = 0; c < table.getNumColumns(); ++c) {
      std::string concept = table.getColumn(c);
      std::string value = table.getValue(r, c);
      ::samson::add(record, concept, value, "left");
    }
  }

  return collection;
}

std::string WorkerCommand::str() {
  std::ostringstream output;

  output << worker_command_id_ << " " << command_;
  return output.str();
}
}
