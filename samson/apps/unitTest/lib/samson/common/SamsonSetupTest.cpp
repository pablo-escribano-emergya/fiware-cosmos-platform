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

#include "gtest/gtest.h"
#include "samson/common/SamsonSetup.h"

namespace samson {
  extern bool isNumber(const std::string& txt);
}

// Test void SamsonSetup();
TEST(samson_common_SamsonSetup, SamsonSetup) {
  std::string samson_home_dir = au::GetRandomTmpFileOrDirectory();
  std::string samson_working_dir = au::GetRandomTmpFileOrDirectory();


  // Better not to set /opt/samson and /var/samson, so init() can get environment variables
  au::Singleton<samson::SamsonSetup>::shared()->SetWorkerDirectories(samson_home_dir, samson_working_dir);

  // The following value (300) for 'isolated.timeout' depends on an external setup-file
  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("isolated.timeout"), "300");

  au::Singleton<samson::SamsonSetup>::shared()->AddItem("unit_test.samsonSetupTest"
                                                        , "initial"
                                                        , "dummy for testing");

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->IsParameterDefined("unit_test.samsonSetupTest"), true)
  << "Error in isParameterDefined positive test";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("unit_test.samsonSetupTest"), "initial")
  << "Error in getValueForParameter for unit_test.samsonSetupTest";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Set("unit_test.samsonSetupTest", "successful"), true)
  << "Error in setValueForParameter for unit_test.samsonSetupTest";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("unit_test.samsonSetupTest"), "successful")
  << "Error in getValueForParameter for unit_test.samsonSetupTest";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Set("isolated.timeout", "1000"), true)
  << "Error in setValueForParameter for isolated.timeout";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("isolated.timeout"), "1000")
  << "Error in getValueForParameter for isolated.timeout";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->IsParameterDefined("non_existing_entry"), false)
  << "Error in isParameterDefined for non_existing_entry";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("non_existing_entry"), "")
  << "Error in getValueForParameter for non_existing_entry";

  au::Singleton<samson::SamsonSetup>::shared()->ResetToDefaultValues();

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->IsParameterDefined("unit_test.samsonSetupTest"), true)
  << "Error in isParameterDefined negative test";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("unit_test.samsonSetupTest"), "initial")
  << "Error in getValueForParameter for unit_test.samsonSetupTest";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("isolated.timeout"), "300")
  << "Error in getValueForParameter for isolated.timeout";

  au::Singleton<samson::SamsonSetup>::shared()->ResetToDefaultValues();

  EXPECT_TRUE(au::Singleton<samson::SamsonSetup>::shared()->Save()) << "Error in save SamsonSetup";

  samson::SamsonSetup* setup                    = au::Singleton<samson::SamsonSetup>::shared();
  std::string          modules_directory        = setup->modules_directory();
  std::string          worker_modules_directory = setup->worker_modules_directory();
  std::string          blocks_directory         = setup->blocks_directory();
  std::string          block_filename           = setup->block_filename(0);
  std::string prefix = "/tmp/";


  EXPECT_EQ(prefix, modules_directory.substr(0, prefix.length()));
  EXPECT_EQ(prefix, worker_modules_directory.substr(0, prefix.length()));
  EXPECT_EQ(prefix, blocks_directory.substr(0, prefix.length()));
  EXPECT_EQ(prefix, block_filename.substr(0, prefix.length()));

  EXPECT_NE(0, setup->block_id_from_filename("xxx"));

  std::string path = setup->samson_working() + "/blocks/block_0";
  EXPECT_NE(0, setup->block_id_from_filename(path));

  path = setup->samson_working() + "/blocks/block_1_2";
  EXPECT_NE(0, setup->block_id_from_filename(path));

  std::string stream_mgr_log_file = setup->samson_working() + "/log/log_stream_state.txt";
  std::string cluster_info_file   = setup->samson_working() + "/log/log_cluster_information.txt";
  EXPECT_STREQ(stream_mgr_log_file.c_str(), setup->stream_manager_log_filename().c_str());
  EXPECT_STREQ(cluster_info_file.c_str(),   setup->cluster_information_filename().c_str());
  
  EXPECT_TRUE(samson::SharedSamsonSetup() != NULL);
  EXPECT_TRUE(samson::isNumber(std::string("24")));
  EXPECT_FALSE(samson::isNumber(std::string("24a")));

  // Destroy singletons (SamsonSetup)
  au::singleton_manager.DestroySingletons();

  au::ErrorManager error;
  au::RemoveDirectory(samson_home_dir, error);
  EXPECT_FALSE(error.HasErrors());
  au::RemoveDirectory(samson_working_dir, error);
  EXPECT_FALSE(error.HasErrors());
}
