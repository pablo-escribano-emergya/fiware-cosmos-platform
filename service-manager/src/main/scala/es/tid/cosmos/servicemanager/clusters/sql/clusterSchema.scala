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

package es.tid.cosmos.servicemanager.clusters.sql

import org.squeryl.{ForeignKeyDeclaration, KeyedEntity, Schema}
import org.squeryl.PrimitiveTypeMode._
import org.squeryl.annotations.Column
import org.squeryl.dsl.{ManyToOne, OneToMany, CompositeKey3}

import es.tid.cosmos.servicemanager.ClusterUser

private[sql] case class ClusterEntity(
    override val id: String,
    name: String,
    size: Int,
    @Column("name_node") nameNode: Option[String],
    state: String,
    reason: Option[String]
) extends KeyedEntity[String] {

  lazy val users: OneToMany[ClusterUserEntity] = ClusterSchema.clusterToUsers.left(this)
  lazy val services: OneToMany[ClusterServiceEntity] = ClusterSchema.clusterToServices.left(this)
}

private[sql] case class HostEntity(
    @Column("cluster_id") clusterId: String,
    name: String,
    ip: String
)

private[sql] case class ClusterUserEntity(
    @Column("user_name") userName: String,
    @Column("public_key") publicKey: String,
    @Column("ssh_enabled") sshEnabled: Boolean,
    @Column("hdfs_enabled") hdfsEnabled: Boolean,
    @Column("is_sudoer") isSudoer: Boolean
) extends KeyedEntity[Long] {

  override val id: Long = 0
  @Column("cluster_id") val clusterId: String = ""
  lazy val users: ManyToOne[ClusterEntity] = ClusterSchema.clusterToUsers.right(this)

  def toClusterUser = ClusterUser(userName, publicKey, sshEnabled, hdfsEnabled, isSudoer)
}

private[sql] object ClusterUserEntity {

  def apply(user: ClusterUser): ClusterUserEntity = ClusterUserEntity(
    user.username,
    user.publicKey,
    user.sshEnabled,
    user.hdfsEnabled,
    user.isSudoer
  )
}

private[sql] case class ClusterServiceEntity(name: String) extends KeyedEntity[Long] {
  override val id: Long = 0
  @Column("cluster_id") val clusterId: String = ""
  lazy val services: ManyToOne[ClusterEntity] = ClusterSchema.clusterToServices.right(this)
}

object HostEntityTypes {
  type MasterEntity = HostEntity with MasterKey
  type SlaveEntity = HostEntity with SlaveKey
  trait MasterKey extends KeyedEntity[String]{
    def clusterId: String
    override def id = clusterId
  }
  trait SlaveKey extends KeyedEntity[CompositeKey3[String, String, String]] {
    def name: String
    def clusterId: String
    def ip: String
    override def id = CompositeKey3(name, clusterId, ip)
  }
}

private[sql] object ClusterSchema extends Schema {
  import scala.language.postfixOps
  import HostEntityTypes._

  private[sql] val clusterState = table[ClusterEntity]("cluster_state")
  private[sql] val clusterUsers = table[ClusterUserEntity]("cluster_user")
  private[sql] val masters = table[MasterEntity]("cluster_master")
  private[sql] val slaves = table[SlaveEntity]("cluster_slave")
  private[sql] val services = table[ClusterServiceEntity]("cluster_service")
  private[sql] val clusterToUsers = oneToManyRelation(clusterState, clusterUsers)
    .via((c, u) => c.id === u.clusterId)
  private[sql] val clusterToServices = oneToManyRelation(clusterState, services)
      .via((c, s) => c.id === s.clusterId)

  override def applyDefaultForeignKeyPolicy(foreignKeyDeclaration: ForeignKeyDeclaration) =
    foreignKeyDeclaration.constrainReference

  clusterToUsers.foreignKeyDeclaration.constrainReference(onDelete cascade)
  clusterToServices.foreignKeyDeclaration.constrainReference(onDelete cascade)

  on(clusterUsers)(usr => declare(
    usr.id is autoIncremented("cluster_users_id_seq")
  ))
  on(services)(srv => declare(
    srv.id is autoIncremented("cluster_services_id_seq")
  ))
}