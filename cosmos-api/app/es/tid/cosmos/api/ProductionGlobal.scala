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

package es.tid.cosmos.api

import es.tid.cosmos.api.auth.multiauth.ConfigBasedMultiAuthProviderComponent
import es.tid.cosmos.api.auth.request.ChainedAuthenticationComponent
import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.api.controllers.admin.InMemoryMaintenanceStatusComponent
import es.tid.cosmos.api.email.PlayEmailerComponent
import es.tid.cosmos.api.profile.dao.sql.PlayDbDataStoreComponent
import es.tid.cosmos.api.task.inmemory.InMemoryTaskDaoComponent
import es.tid.cosmos.api.usage.DynamicMachineUsageComponent
import es.tid.cosmos.common.ApplicationConfigComponent
import es.tid.cosmos.servicemanager.production.ProductionServiceManagerComponent

/** Global application settings tied to real services. */
object ProductionGlobal extends AbstractGlobal(new Application
  with ConfigBasedMultiAuthProviderComponent
  with PlayDbDataStoreComponent
  with ProductionServiceManagerComponent
  with ApplicationConfigComponent
  with DynamicMachineUsageComponent
  with InMemoryMaintenanceStatusComponent
  with ChainedAuthenticationComponent
  with InMemoryTaskDaoComponent
  with PlayEmailerComponent
)
