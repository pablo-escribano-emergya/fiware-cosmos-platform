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

package es.tid.cosmos.api.profile

import org.mockito.BDDMockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.controllers.cluster.ClusterReference
import es.tid.cosmos.api.profile.UserState.Enabled
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.clusters.ClusterState.ActiveStates
import es.tid.cosmos.platform.common.scalatest.matchers.ValidationMatchers
import es.tid.cosmos.platform.common.scalatest.data.DataPicker

class ProfileQuotasTest extends FlatSpec
    with MustMatchers with MockitoSugar with ValidationMatchers with DataPicker {

  override val seed: Int = 1

  "A user request exceeding their personal quota" must "not be allowed" in {
    val userGroup = GuaranteedGroup("A", FiniteQuota(3))
    val groupUsage = Map[Group, Int](
      userGroup -> 2,
      GuaranteedGroup("B", FiniteQuota(2)) -> 0,
      NoGroup -> 3
    )
    val context = Context(
      groupUsage,
      userGroup,
      personalMaxQuota = FiniteQuota(3),
      usedMachinesByUser = 2,
      machinePool = 10
    )
    context.ensureThat { (quotas, profile) =>
      quotas.withinQuota(profile, 1) must (beSuccessful and haveValidValue(1))
      quotas.withinQuota(profile, 2) must haveFailures(
        "Profile quota exceeded",
        "You can request up to 1 machine(s) at this point."
      )
    }
  }

  "A user request exceeding their minimum group quota" must "be allowed if machines available" in {
    /*
    +----+----+---+---+----+----+----+---+---+----+
    | 1  | 2  | 3 | 4 | 5  | 6  | 7  | 8 | 9 | 10 |
    +----+----+---+---+----+----+----+---+---+----+
    | A  | A  | B | B | NG | NG | NG | o | o | o  |
    | ur | ur | r | r | u  | u  | u  | o | o | o  |
    +----+----+---+---+----+----+----+---+---+----+
                                       X   X   X
     */
    val userGroup = GuaranteedGroup("A", FiniteQuota(2))
    val groupUsage = Map[Group, Int](
      userGroup -> 2,
      GuaranteedGroup("B", FiniteQuota(2)) -> 0,
      NoGroup -> 3
    )
    val context = Context(
      groupUsage,
      userGroup,
      personalMaxQuota = UnlimitedQuota,
      usedMachinesByUser = 2,
      machinePool = 10
    )
    context.ensureThat { (quotas, profile) =>
      quotas.withinQuota(profile, 1) must (beSuccessful and haveValidValue(1))
      quotas.withinQuota(profile, 2) must (beSuccessful and haveValidValue(2))
      quotas.withinQuota(profile, 3) must (beSuccessful and haveValidValue(3))
      quotas.withinQuota(profile, 4) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 3 machine(s) at this point."
      )
    }
  }

  "A user without personal quota" must "be allowed the max number of available machines" in {
    /*
    +----+----+---+---+---+----+----+----+---+----+
    | 1  | 2  | 3 | 4 | 5 | 6  | 7  | 8  | 9 | 10 |
    +----+----+---+---+---+----+----+----+---+----+
    | A  | A  | A | B | B | NG | NG | NG | o | o  |
    | ur | ur | r | r | r | u  | u  | u  | o | o  |
    +----+----+---+---+---+----+----+----+---+----+
                X                          X   X
     */
    val userGroup = GuaranteedGroup("A", FiniteQuota(3))
    val groupUsage = Map[Group, Int](
      userGroup -> 2,
      GuaranteedGroup("B", FiniteQuota(2)) -> 0,
      NoGroup -> 3
    )
    val context = Context(
      groupUsage,
      userGroup,
      personalMaxQuota = UnlimitedQuota,
      usedMachinesByUser = 2,
      machinePool = 10
    )
    context.ensureThat{ (quotas, profile) =>
      quotas.withinQuota(profile, 3) must (beSuccessful and haveValidValue(3))
      quotas.withinQuota(profile, 4) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 3 machine(s) at this point."
      )
    }
  }

  "A quota" must "not consider the machines of any clusters in terminal states machines as used" in {
    /*
    +----+----+---+----+----+----+----+---+---+----+
    | 1  | 2  | 3 | 4  | 5  | 6  | 7  | 8 | 9 | 10 |
    +----+----+---+----+----+----+----+---+---+----+
    | A  | A  | B | B  | NG | NG | NG | o | o | o  |
    | tr | ur | r | rt | u  | f  | u  | o | o | o  |
    +----+----+---+----+----+----+----+---+---+----+
      X                       X         X   X   X
     */
    val userGroup = GuaranteedGroup("A", FiniteQuota(2))
    val groupB = GuaranteedGroup("B", FiniteQuota(2))
    val groupUsage = Map[Group, Int](
      userGroup -> 1,
      groupB -> 0,
      NoGroup -> 2
    )
    val terminatedClusters = Map[Group, List[ClusterReference]](
      userGroup -> List(clusterReference(1, Terminated)),
      groupB -> List(clusterReference(1, Terminated)),
      NoGroup -> List(clusterReference(1, Failed("Cluster blew up")))
    )
    val context = Context(
      groupUsage,
      userGroup,
      personalMaxQuota = UnlimitedQuota,
      usedMachinesByUser = 1,
      machinePool = 10,
      extraClusters = terminatedClusters
    )
    context.ensureThat { (quotas, profile) =>
      quotas.withinQuota(profile, 1) must (beSuccessful and haveValidValue(1))
      quotas.withinQuota(profile, 2) must (beSuccessful and haveValidValue(2))
      quotas.withinQuota(profile, 3) must (beSuccessful and haveValidValue(3))
      quotas.withinQuota(profile, 4) must (beSuccessful and haveValidValue(4))
      quotas.withinQuota(profile, 5) must (beSuccessful and haveValidValue(5))
      quotas.withinQuota(profile, 6) must haveFailures(
        "Quota exceeded for group [A].",
        "You can request up to 5 machine(s) at this point."
      )
    }
  }

  case class Context(
    groupUsage: Map[Group, Int],
    userGroup: Group,
    personalMaxQuota: Quota,
    usedMachinesByUser: Int,
    machinePool: Int,
    extraClusters: Map[Group, List[ClusterReference]] = Map.empty
  ) {

    /**
     * Execute a test on the given context as configuration.
     *
     * 1. Create 1 profile or 1 + user for userGroup
     * 2. For each profile we need to create one cluster with the size of the group's usage
     * 3. Register the profiles
     * 4. assign clusters
     *
     * @param test the test as a function receiving the quotas manager and the profile to check
     */
    def ensureThat(test: (ProfileQuotas, CosmosProfile) => Unit) {
      import scalaz.Scalaz._
      val groupProfiles = (for ((group, id) <- groupUsage.keys zip (1 to groupUsage.size)) yield {
        val quota = if (group == userGroup) personalMaxQuota else UnlimitedQuota
        val profile = CosmosProfile(
          id,
          state = Enabled,
          handle = s"handle$id",
          email = "user@example.com",
          ApiCredentials.random(),
          keys = Nil,
          group,
          quota,
          capabilities = UntrustedUserCapabilities
        )
        group -> Set(profile)
      }).toMap

      val groupClusters = groupUsage.map {
        case (group, used) =>
          groupProfiles(group).head -> List(clusterReference(used, pickAnyOne(ActiveStates)))
      } |+| extraClusters.mapKeys(groupProfiles(_).head)

      val quotas = new ProfileQuotas(
        machinePoolSize = machinePool,
        groups = groupUsage.keys.toSet,
        lookupByGroup = groupProfiles,
        listClusters = groupClusters
      )

      test(quotas, groupProfiles(userGroup).head)
    }
  }

  def clusterReference(size: Int, state: ClusterState): ClusterReference = {
    val description = mock[ClusterDescription]
    given(description.size).willReturn(size)
    given(description.state).willReturn(state)
    val reference = mock[ClusterReference]
    given(reference.description).willReturn(description)
    reference
  }
}
