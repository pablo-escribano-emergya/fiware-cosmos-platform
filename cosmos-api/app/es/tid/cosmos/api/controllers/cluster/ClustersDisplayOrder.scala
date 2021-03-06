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

package es.tid.cosmos.api.controllers.cluster

import es.tid.cosmos.servicemanager.clusters._

/**
 * Sort ClusterReferences by state and then by name
 */
object ClustersDisplayOrder extends Ordering[ClusterReference] {

  def compare(left: ClusterReference, right: ClusterReference): Int = {
    val stateComparison = compareStates(left.description.state, right.description.state)
    if (stateComparison != 0) stateComparison
    else left.description.name.underlying.compare(right.description.name.underlying)
  }

  private val stateOrder = Map[ClusterState, Int](
    Provisioning -> 0,
    Running -> 1,
    Terminating -> 2,
    Terminated -> 3
  ).withDefaultValue(4)

  private def compareStates(left: ClusterState, right: ClusterState): Int =
    stateOrder(left).compare(stateOrder(right))
}
