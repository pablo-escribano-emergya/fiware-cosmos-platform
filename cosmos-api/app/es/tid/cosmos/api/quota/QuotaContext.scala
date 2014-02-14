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

package es.tid.cosmos.api.quota

import scalaz._

/** Characterization of the available and used resources that can be queried to check if new
  * provisions are off limits.
  *
  * @constructor Creates a new QuotaContext. Note that this will fail if group quotas can not be
  * guaranteed with current resource usage.
  *
  * @tparam ConsumerId     Type of the consumer identifier
  * @param poolSize        Maximum available resources
  * @param groupQuotas     Group and user quotas
  * @param usageByProfile  Current resource usage
  * */
case class QuotaContext[ConsumerId](
    poolSize: Int,
    groupQuotas: GlobalGroupQuotas[ConsumerId],
    usageByProfile: Map[ConsumerId, Int]) {

  import scalaz.Scalaz._

  val totalUsedResources: Int = usageByProfile.values.sum

  val reservedUnusedResourcesByGroup: Map[GuaranteedGroup, Int] =
    groupQuotas.reservedUnusedResources(usageByProfile)

  val totalReservedUnusedResources: Int = reservedUnusedResourcesByGroup.values.sum

  val totalUnreserved: Int = poolSize - totalUsedResources - totalReservedUnusedResources

  require(totalUnreserved >= 0,
    s"There are not enough machines available (deficit of ${-totalUnreserved}) " +
      "to meet the guaranteed minimum group quotas")

  def withinQuota(consumer: QuotaConsumer[ConsumerId], size: Int): ValidationNel[String, Int] = {
    val availableFromConsumer = maximumConsumerRequest(consumer)
    val consumerValidation = validate(availableFromConsumer, size, "Profile quota exceeded.")

    val maybeGroup = guaranteedGroupOf(consumer)
    val groupValidationMessage = "Quota exceeded for %s.".format(
      maybeGroup.map(group => s"group [${group.name}]").getOrElse("users not belonging to any group")
    )
    val availableFromGroup = maximumGroupRequest(maybeGroup)
    val groupValidation = validate(availableFromGroup, size, groupValidationMessage)

    val overallAvailable = Quota.min(availableFromConsumer, availableFromGroup)
    val available = overallAvailable.toInt.getOrElse(0)
    val overallValidation = validate(overallAvailable, size,
      s"You can request up to $available machine${if (available != 1) "s" else ""} at this point.")

    (consumerValidation |@| groupValidation |@| overallValidation) { (_, _, last) => last }
  }

  def isGroupQuotaFeasible(group: GuaranteedGroup): Validation[String, GuaranteedGroup] = {
    val currentGuaranteedQuota = groupQuotas.get(group.name).map(_.minimumQuota).getOrElse(EmptyQuota)
    val maxGuaranteedQuota = currentGuaranteedQuota + Quota(totalUnreserved)

    if (maxGuaranteedQuota.withinQuota(group.minimumQuota)) group.success
    else s"Group ${group.name} can have a minimum quota of up to $maxGuaranteedQuota.".failure
  }

  private def currentUse(id: ConsumerId): Int = usageByProfile.getOrElse(id, 0)

  private def availableForGroup(group: GuaranteedGroup): Int =
    totalUnreserved + reservedUnusedResourcesByGroup.getOrElse(group, 0)

  private def maximumGroupRequest(group: Option[GuaranteedGroup]): LimitedQuota =
    Quota(group.map(availableForGroup).getOrElse(totalUnreserved))

  private def maximumConsumerRequest(consumer: QuotaConsumer[ConsumerId]): Quota =
    consumer.quota - Quota(currentUse(consumer.id))

  private def validate(quota: Quota, size: Int, errorMessage: String): ValidationNel[String, Int] =
    if (quota.withinQuota(size)) size.successNel[String] else errorMessage.failureNel[Int]

  private def guaranteedGroupOf(profile: QuotaConsumer[_]): Option[GuaranteedGroup] =
    profile.group match {
      case group: GuaranteedGroup => Some(group)
      case _ => None
    }
}
