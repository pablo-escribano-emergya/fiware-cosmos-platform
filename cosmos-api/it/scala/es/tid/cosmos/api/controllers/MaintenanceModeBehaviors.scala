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

package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.FakeRequest
import play.api.test.Helpers._
import play.api.http.MimeTypes.{JSON, HTML}
import play.api.http.Writeable

import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.mocks.WithTestApplication

trait MaintenanceModeBehaviors { this: FlatSpec with MustMatchers =>

  def resourceDisabledWhenUnderMaintenance[T](
      request: FakeRequest[T])(implicit evidence: Writeable[T]) {
    rejectingWithMaintenanceMessage(request, name = "resource", expectedMimeType = JSON)
  }

  def pageDisabledWhenUnderMaintenance[T](
      request: FakeRequest[T])(implicit evidence: Writeable[T]) {
    rejectingWithMaintenanceMessage(request, name = "page", expectedMimeType = HTML)
  }

  def enabledWhenUnderMaintenance[T](request: FakeRequest[T])(implicit evidence: Writeable[T]) {
    it must "accept requests when under maintenance status" in new WithSampleSessions {
      services.maintenanceStatus.enterMaintenance()
      status(regUserInGroup.doRequest(request)) must not(equal(SERVICE_UNAVAILABLE))
    }
  }

  def enabledOnlyForOperatorsWhenUnderMaintenance[T](request: FakeRequest[T])
                                                    (implicit evidence: Writeable[T]) {
    it must "reject non-operator requests when under maintenance status" in new WithSampleSessions {
      services.maintenanceStatus.enterMaintenance()
      status(regUserInGroup.doRequest(request)) must equal(SERVICE_UNAVAILABLE)
    }

    it must "accept operator requests when under maintenance status" in new WithSampleSessions {
      services.maintenanceStatus.enterMaintenance()
      status(opUser.doRequest(request)) must not(equal(SERVICE_UNAVAILABLE))
    }
  }

  private def rejectingWithMaintenanceMessage[T](
      request: FakeRequest[T],
      name: String,
      expectedMimeType: String)(implicit evidence: Writeable[T]) {

    it must s"reject request with 503 status as a $name under maintenance status" in
      new WithSampleSessions {
        services.maintenanceStatus.enterMaintenance()
        val response = regUserInGroup.doRequest(request)
        status(response) must equal (SERVICE_UNAVAILABLE)
        contentType(response) must be (Some(expectedMimeType))
        contentAsString(response) must include("Service temporarily in maintenance mode")
      }

    it must s"accept request when not a $name under maintenance" in new WithTestApplication {
      status(route(request).get) must not(equal(SERVICE_UNAVAILABLE))
    }
  }
}
