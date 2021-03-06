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

package es.tid.cosmos.servicemanager.clusters.sql

import java.net.URI

import org.squeryl.PrimitiveTypeMode._
import org.squeryl.dsl.ast.UpdateAssignment

import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.clusters.sql.HostEntityTypes.{SlaveKey, MasterKey}

/** This class provides a mutable cluster description which reads directly from the SQL DAO
  * and writes any changes to the DAO too
  * @param id The ID of the cluster
  * @param dao The DAO that backs the description of the cluster
  */
private[sql] class SqlMutableClusterDescription(
    override val id: ClusterId,
    dao: SqlClusterDao) extends MutableClusterDescription  {

  import ClusterSchema._

  override def name: ClusterName = ClusterName(getField(_.name))

  override def name_=(name: ClusterName) {
    setField(_.name := name.underlying)
  }

  override def size: Int = getField(_.size)

  override def state: ClusterState = getField(c => (c.state, c.reason)) match {
    case (Provisioning.name, _) => Provisioning
    case (Running.name, _) => Running
    case (Terminating.name, _) => Terminating
    case (Terminated.name, _) => Terminated
    case (Failed.name, Some(reason)) => Failed(reason)
    case (unknownState, reason) => throw new IllegalStateException(
      s"Unknown state [$unknownState] with reason: $reason")
  }

  override def state_=(state: ClusterState) {
    setFields(c => Seq(c.state := state.name, c.reason := (state match {
      case Failed(reason) => Some(reason)
      case _ => None
    })))
  }

  override def nameNode: Option[URI] = getField(_.nameNode).map(new URI(_))

  override def nameNode_=(nameNode: URI) {
    setField(_.nameNode := Some(nameNode.toString))
  }

  override def master: Option[HostDetails] = dao.newTransaction {
    from(masters)(m => where (m.clusterId === this.id.toString) select m)
      .headOption.map(m => HostDetails(m.name, m.ip))
  }

  override def master_=(master: HostDetails) {
    dao.newTransaction {
      masters.insertOrUpdate(
        new HostEntity(this.id.toString, master.hostname, master.ipAddress) with MasterKey)
    }
  }

  override def slaves: Seq[HostDetails] = dao.newTransaction {
    from(ClusterSchema.slaves)(s => where (s.clusterId === this.id.toString) select s)
      .map(s => HostDetails(s.name, s.ip)).toSeq
  }

  override def slaves_=(slaves: Seq[HostDetails]) {
    dao.newTransaction {
      slaves.foreach { h =>
        ClusterSchema.slaves.insertOrUpdate(
          new HostEntity(this.id.toString, h.hostname, h.ipAddress) with SlaveKey)
      }
    }
  }

  override def users: Option[Set[ClusterUser]] = dao.getUsers(id)

  override def users_=(users: Set[ClusterUser]) = dao.setUsers(id, users)

  override def services: Set[String] = dao.newTransaction {
    from(ClusterSchema.services)(s => where (s.clusterId === this.id.toString) select s)
      .map(_.name).toSet
  }

  override def services_=(services: Set[String]) {
    dao.newTransaction {
      val cluster = clusterState.lookup(this.id.toString).get
      cluster.services.deleteAll
      services.foreach { serviceName =>
        cluster.services.associate(new ClusterServiceEntity(serviceName))
      }
    }
  }

  override def blockedPorts: Set[Int] = dao.newTransaction {
    from(ClusterSchema.blockedPorts)(s => where (s.clusterId === this.id.toString) select s)
      .map(_.port).toSet
  }

  override def blockedPorts_=(blockedPorts: Set[Int]) {
    dao.newTransaction {
      val cluster = clusterState.lookup(this.id.toString).get
      cluster.blockedPorts.deleteAll
      blockedPorts.foreach { blockedPort =>
        cluster.blockedPorts.associate(new ClusterBlockedPortEntity(blockedPort))
      }
    }
  }

  private def getField[A](field: ClusterEntity => A) = dao.newTransaction {
    from(clusterState)(c => where (c.id === this.id.toString) select(field(c))).single
  }

  private def setField(setter: ClusterEntity => UpdateAssignment) {
    setFields(c => Seq(setter(c)))
  }

  private def setFields(setter: ClusterEntity => Seq[UpdateAssignment]): Unit = dao.newTransaction {
    update(clusterState)(c => where (c.id === this.id.toString) set(setter(c): _*))
  }
}
