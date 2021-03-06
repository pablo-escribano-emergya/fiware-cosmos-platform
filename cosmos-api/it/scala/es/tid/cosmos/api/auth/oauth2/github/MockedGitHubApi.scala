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

package es.tid.cosmos.api.auth.oauth2.github

import scala.concurrent.stm.{TSet, atomic}
import scala.util.Random

import play.api.libs.json.Json
import unfiltered.filter.Planify
import unfiltered.jetty.Http
import unfiltered.request._
import unfiltered.response._

import es.tid.cosmos.api.auth.oauth2.github.UrlUtils.parseQueryParams

class MockedGitHubApi(
    port: Int,
    clientId: String,
    clientSecret: String,
    existingUser: GitHubProfile
  ) {

  private val grantedCodes = TSet.empty[String]
  private val grantedTokens = TSet.empty[String]
  private val server = defineServer()

  def requestAuthorizationCode(authUrl: String, userId: Int): String = {
    val params = parseQueryParams(authUrl)
    require(params.contains("client_id" -> clientId), "Missing client id in code request")
    require(params.contains("scope" -> "user"), "Scope is missing or different from 'user'")
    require(params.exists(pair => pair._1 == "redirect_uri"))
    val code = randomId()
    atomic { implicit txn =>
      grantedCodes.add(code)
    }
    code
  }

  def start() {
    server.start()
  }

  def stop() {
    server.stop()
  }

  def clear() {
    atomic { implicit txt =>
      grantedCodes.clear()
      grantedTokens.clear()
    }
  }

  def configurationKeys: Map[String, String] = {
    val baseUrl = s"http://127.0.0.1:$port"
    Map(
      "name" -> "GitHub",
      "class" -> classOf[Provider].getName,
      "client.id" -> clientId,
      "client.secret" -> clientSecret,
      "auth.url" -> s"$baseUrl/login/oauth/",
      "api.url" -> s"$baseUrl/api/v3/",
      "signup.url" -> s"$baseUrl/auth/ldap/"
    )
  }

  private def defineServer() = {
    object ClientId extends Params.Extract("client_id", Params.first)
    object ClientSecret extends Params.Extract("client_secret", Params.first)
    object Code extends Params.Extract("code", Params.first)
    object Token extends Params.Extract("access_token", Params.first)

    Http.local(port).filter(Planify({
      case req @ POST(Path("/login/oauth/access_token") &
        Params(ClientId(paramId) & ClientSecret(paramSecret) & Code(code))) =>
        atomic { implicit txn =>
          if (clientId == paramId && paramSecret == clientSecret && grantedCodes.contains(code)) {
            val token = randomId()
            grantedTokens.add(token)
            ResponseString(Json.obj("access_token" -> token).toString())
          } else BadRequest ~> ResponseString("error=invalid_request")
        }

      case req @ GET(Path("/api/v3/user") & Params(Token(token))) => atomic { implicit txn =>
        if (grantedTokens.contains(token)) ResponseString(
          s"""
            |{
            |    "login": "${existingUser.login}",
            |    "id": ${existingUser.id},
            |    "url": "https://pdihub.hi.inet/api/v3/users/${existingUser.login}",
            |    "html_url": "https://pdihub.hi.inet/${existingUser.login}",
            |    "followers_url": "https://pdihub.hi.inet/api/v3/users/${existingUser.login}/followers",
            |    "following_url": "https://pdihub.hi.inet/api/v3/users/${existingUser.login}/following{/other_user}",
            |    "type": "User",
            |    "name": "${existingUser.name.get}",
            |    "email": "${existingUser.email.get}",
            |    "site_admin": false
            |}
          """.stripMargin
        ) else Unauthorized
      }
    }))
  }

  private def randomId() = Seq.fill(8)(Random.nextPrintableChar()).mkString
}
