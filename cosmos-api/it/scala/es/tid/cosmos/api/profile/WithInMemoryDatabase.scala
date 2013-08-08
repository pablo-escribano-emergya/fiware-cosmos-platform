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

package es.tid.cosmos.api.profile

import play.api.test.Helpers._
import es.tid.cosmos.api.mocks.{TestWithDbGlobal, WithTestApplication}

private[profile] class WithInMemoryDatabase(additionalConfiguration: Map[String, String] = Map.empty)
  extends WithTestApplication(
    additionalConfiguration = (inMemoryDatabase(
      name="default", options=Map("MODE" -> "MYSQL")) ++ additionalConfiguration),
    global = new TestWithDbGlobal)