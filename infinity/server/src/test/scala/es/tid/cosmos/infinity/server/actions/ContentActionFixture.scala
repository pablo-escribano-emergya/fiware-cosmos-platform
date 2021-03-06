/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.actions

import java.io.InputStream

import org.mockito.Mockito.spy
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.permissions.UserProfile
import es.tid.cosmos.infinity.server.hadoop.DummyDataNode
import es.tid.cosmos.infinity.server.urls.UrlMapper

trait ContentActionFixture extends MockitoSugar {
  val urlMapper = mock[UrlMapper]
  val user = UserProfile("gandalf", Seq("istari"))
  val context = Action.Context(user, urlMapper)
  val on = Path.absolute("/to/file")
  val dataNode = spy(new DummyDataNode)
  val in = mock[InputStream]("internalStream")
}
