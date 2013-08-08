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

package es.tid.cosmos.admin

import java.net.URI
import scala.Some
import scala.concurrent.Future

import org.mockito.BDDMockito.given
import org.mockito.Mockito.{verify, never}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager._

class ClusterTest extends FlatSpec with MustMatchers with MockitoSugar {

  val clusterId = ClusterId("booya")

  trait WithServiceManager {
    val sm = mock[ServiceManager]
  }

  trait WithMissingStorage extends WithServiceManager {
    given(sm.describeCluster(clusterId)).willReturn(None)
  }

  trait WithExistingStorage extends WithServiceManager {
    given(sm.describeCluster(clusterId)).willReturn(Some(new ClusterDescription {
      val id = clusterId
      val name = ""
      val size = 3
      val state = Running
      val nameNode_> = Future.successful(new URI("hdfs://host:1234"))
      val master_> = Future.successful(HostDetails("host", "ipAddress"))
      val slaves_>  = Future.successful(Seq(HostDetails("host", "ipAddress")))
    }))
  }

  it must "not terminate the cluster if it hasn't been found" in new WithMissingStorage {
    Cluster.terminate(sm, clusterId) must be (false)
    verify(sm).describeCluster(clusterId)
    verify(sm, never()).terminateCluster(clusterId)
  }

  it must "terminate the cluster if it has been found" in new WithExistingStorage {
    given(sm.terminateCluster(clusterId)).willReturn(Future.successful())
    Cluster.terminate(sm, clusterId) must be (true)
    verify(sm).describeCluster(clusterId)
  }
}