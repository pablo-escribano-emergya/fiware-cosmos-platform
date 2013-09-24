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

package es.tid.cosmos.api.mocks.oauth2

import scala.concurrent.Future

import es.tid.cosmos.api.oauth2._
import es.tid.cosmos.api.profile.UserId

object MockOAuthConstants {
  val BaseUrl = "http://mock-oauth"
  val GrantedCode = "fake-code"
  val GrantedToken = "fake-token-123"
  val User101 = UserProfile(UserId("101"), Some("John Smith"), Some("jsmith@tid.es"))
  val ProviderId = "id_service"
}

object MockOAuthProvider extends OAuthProvider {
  import MockOAuthConstants._

  override def id = ProviderId

  override val name = ProviderId.replace("_", " ")

  override def newAccountUrl: Option[String] = Some(s"$BaseUrl/signup/")

  override def authenticationUrl(redirectUri: String): String =
    s"$BaseUrl/oauth?client_id=fake&redirect_to=$redirectUri"

  override def requestAccessToken(code: String): Future[String] =
    if (code == GrantedCode) Future.successful(GrantedToken)
    else Future.failed(OAuthException(OAuthError.InvalidGrant, "testing invalid grant"))

  override def requestUserProfile(token: String): Future[UserProfile] =
    if (token == GrantedToken) Future.successful(User101)
    else Future.failed(OAuthException(OAuthError.InvalidRequest, "testing invalid requests"))
}

object MockMultiOAuthProvider extends MultiOAuthProvider {
  override val providers = Map(MockOAuthConstants.ProviderId -> MockOAuthProvider)
}

trait MockMultiOAuthProviderComponent extends MultiOAuthProviderComponent {
  override val multiOAuthProvider: MultiOAuthProvider = MockMultiOAuthProvider
}