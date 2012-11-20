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
 * FILE            common_test.cpp
 *
 * AUTHOR         Gregorio Escalada
 *
 * DATE            May 2012
 *
 * DESCRIPTION
 *
 * unit testing of the common class in the samson  library
 *
 */

#include "samson/common/KVRange.h"
#include "samson/common/MemoryCheck.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/Rate.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/status.h"

#include "gtest/gtest.h"

#include "samson/common/KVInfo.h"

// Test  KVInfo;
TEST(samson_common_KVInfo, test1) {
  samson::KVInfo info_1;

  EXPECT_EQ(info_1.str(), "(     0 kvs in     0 bytes )") << "Error in KVInfo str() for default constructor";
  info_1.set(100, 10);
  EXPECT_EQ(info_1.str(), "(  10.0 kvs in   100 bytes )") << "Error in KVInfo str() for set";

  info_1.append(1, 1);
  EXPECT_EQ(info_1.str(), "(  11.0 kvs in   101 bytes )") << "Error in KVInfo str() for append";
  info_1.remove(2, 2);
  EXPECT_EQ(info_1.str(), "(  9.00 kvs in  99.0 bytes )") << "Error in KVInfo str() for append";
  EXPECT_EQ(info_1.isEmpty(), false) << "Error in KVInfo isValid() for isEmpty() false";
  info_1.clear();
  EXPECT_EQ(info_1.isEmpty(), true) << "Error in KVInfo isValid() for isEmpty() true";

  samson::KVInfo info_2(2000, 20);
  EXPECT_EQ(info_2.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in KVInfo for initialised constructor";
  info_1.append(info_2);
  EXPECT_EQ(info_1.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in KVInfo append from KVInfo";
}

