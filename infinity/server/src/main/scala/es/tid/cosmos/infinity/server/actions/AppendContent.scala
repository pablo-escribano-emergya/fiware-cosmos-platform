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
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent._

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.actions.Action._
import es.tid.cosmos.infinity.server.hadoop.DfsClientFactory
import org.apache.hadoop.io.IOUtils

case class AppendContent(
    dfsClientFactory: DfsClientFactory,
    on: Path,
    in: InputStream) extends Action {
  import AppendContent._

  override def apply(context: Context): Future[Result] = future {
    val client = dfsClientFactory.newClient
    val out = client.append(on.toString, BufferSize, NoProgress, NoStatistics)
    IOUtils.copyBytes(in, out, BufferSize)
    ContentAppended(on)
  }
}

object AppendContent {
  private val BufferSize = 4096
  private val NoProgress, NoStatistics = null
}
