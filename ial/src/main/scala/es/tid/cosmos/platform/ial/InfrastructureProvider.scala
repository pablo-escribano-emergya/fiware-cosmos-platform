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

package es.tid.cosmos.platform.ial

import scala.concurrent.Future

/**
 * An abstraction for an infrastructure provider
 */
trait InfrastructureProvider {
  /**
   * Create count machines of given profile with given name prefix. If there are not enough
   * infrastructure resources to satisfy the request, an error is returned with
   * ResourceExhaustedException wrapped.
   *
   * @param profile the machine profile for the machines to be created.
   * @param numberOfMachines the amount of machines to be created.
   * @param bootstrapAction action to be performed on every host just after it starts running and
   *                        to be finished to consider the provision successful
   * @return a future which after success provides the sequence of newly created machines
   */
  def createMachines(
      profile: MachineProfile.Value,
      numberOfMachines: Int,
      bootstrapAction: MachineState => Future[Unit]): Future[Seq[MachineState]]

  /**
   * Get the machines already in use for the given host names.
   *
   * @param hostNames the host names of the machines to look for
   * @return the future of the machine list corresponding to the given host names
   */
  def assignedMachines(hostNames: Seq[String]): Future[Seq[MachineState]]

  /**
   * Release the machines so that its resources can be reused in further createMachine requests
   *
   * @param machines The set of machines to be released.
   * @return         a future which terminates once the release has completed
   */
  def releaseMachines(machines: Seq[MachineState]): Future[Unit]

  /**
   * The ssh key that enables root access to the machines
   */
  val rootPrivateSshKey: String

  def availableMachineCount(profile: MachineProfile.Value): Future[Int]
}