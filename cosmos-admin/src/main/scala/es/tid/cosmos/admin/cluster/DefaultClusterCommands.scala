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

package es.tid.cosmos.admin.cluster

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._

import es.tid.cosmos.admin.command.CommandResult
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.ClusterId

private[cluster] class DefaultClusterCommands(serviceManager: ServiceManager)
  extends ClusterCommands {

  private val commandTimeout = 15.minutes

  override def terminate(clusterId: ClusterId): CommandResult =
    serviceManager.describeCluster(clusterId) match {
      case None =>
        CommandResult.error("Cluster not found. Cannot terminate...")
      case _ =>
        println("Cluster found. Terminating...")
        val clusterTermination = serviceManager.terminateCluster(clusterId)
          .map(_ => CommandResult.success("Cluster terminated successfully"))
        CommandResult.await(clusterTermination, commandTimeout)
    }
}
