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

package es.tid.cosmos.admin.profile

import es.tid.cosmos.api.profile.dao.CosmosDataStore
import es.tid.cosmos.servicemanager.ServiceManager

private[profile] class DefaultProfileCommands(
    store: CosmosDataStore, serviceManager: ServiceManager) extends ProfileCommands {

  private lazy val listCommand = new ListProfilesCommand(store)

  override def list() = listCommand()
  override lazy val quota = new ProfileQuotaCommands(store)
  override lazy val capability = new ProfileCapabilityCommands(store)
  override lazy val group = new ProfileGroupCommands(store, serviceManager)
}
