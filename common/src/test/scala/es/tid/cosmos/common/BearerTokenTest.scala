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

package es.tid.cosmos.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class BearerTokenTest extends FlatSpec with MustMatchers {

  "Bearer token" must "be formatted as a header" in {
    BearerToken("token") must be ("Bearer token")
  }

  it must "be extracted from a valid header" in {
    BearerToken.unapply("Bearer 1a.2b_3c-4d+5e/6f") must be (Some("1a.2b_3c-4d+5e/6f"))
  }

  it must "consider the scheme in a case insensitive fashion" in {
    BearerToken.unapply("BEARER 1a.2b_3c-4d+5e/6f") must be (Some("1a.2b_3c-4d+5e/6f"))
  }

  it must "not be extracted if scheme is not 'bearer'" in {
    BearerToken.unapply("basic 1a.2b_3c-4d+5e/6f") must be ('empty)
  }

  it must "not be extracted if there is no token" in {
    BearerToken.unapply("bearer ") must be ('empty)
  }
}
