/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity

import scala.concurrent.duration._

import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs.permission.FsPermission

import es.tid.cosmos.infinity.common.credentials.{ClusterCredentials, Credentials, UserCredentials}

private[infinity] class InfinityConfiguration(config: Configuration) {

  import InfinityConfiguration._

  /** Default authority or none */
  def defaultAuthority: Option[String] = Option(config.get(DefaultAuthorityProperty))

  def useSsl: Boolean = config.getBoolean(UseSslProperty, DefaultUseSslValue)

  def shortOperationTimeout: FiniteDuration = {
    val duration = config.getLong(ShortOperationTimeoutProperty, DefaultShortTimeout)
    requireValidTimeout(duration)
    duration.millis
  }

  def longOperationTimeout: FiniteDuration = {
    val duration = config.getLong(LongOperationTimeoutProperty, DefaultLongTimeout)
    requireValidTimeout(duration)
    duration.millis
  }

  def credentials: Option[Credentials] = clusterCredentials orElse userCredentials

  def umask = FsPermission.getUMask(config)

  private def clusterCredentials: Option[ClusterCredentials] =
    Option(config.get(ClusterSecretProperty)).map(ClusterCredentials.apply)

  private def userCredentials: Option[UserCredentials] = for {
    key <- Option(config.get(ApiKeyProperty))
    secret <- Option(config.get(ApiSecretProperty))
  } yield UserCredentials(key, secret)

  private def requireValidTimeout(value: Long) =
    require(value > 0, s"Timeout must be strictly positive but was $value ms")
}

object InfinityConfiguration {
  val DefaultAuthorityProperty = property("defaultAuthority")
  val UseSslProperty = property("ssl.enabled")
  val DefaultUseSslValue = true
  val ShortOperationTimeoutProperty = property("timeout.shortOperation.millis")
  val LongOperationTimeoutProperty = property("timeout.longOperation.millis")
  val DefaultShortTimeout = 3.seconds.toMillis
  val DefaultLongTimeout = 15.minutes.toMillis
  val ClusterSecretProperty = property("clusterSecret")
  val ApiKeyProperty = property("apiKey")
  val ApiSecretProperty = property("apiSecret")

  private def property(suffix: String) = s"fs.infinity.$suffix"
}
