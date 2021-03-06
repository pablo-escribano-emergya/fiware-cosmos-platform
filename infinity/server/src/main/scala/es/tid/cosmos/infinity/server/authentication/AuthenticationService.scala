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

package es.tid.cosmos.infinity.server.authentication

import scala.concurrent.Future

import es.tid.cosmos.infinity.common.credentials.Credentials
import es.tid.cosmos.infinity.common.permissions.UserProfile

trait AuthenticationService {

  /** Perform an authentication request.
    *
    * @param origin       Source address of the request
    * @param credentials  Credentials to authenticate with
    * @return  The user profile when successful, an AuthenticationException when not.
    */
  def authenticate(origin: String, credentials: Credentials): Future[UserProfile]
}
