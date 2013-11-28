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


/** Utilities for setting up users on integration tests. */
object CosmosProfileTestHelpers {

  /** Generates a registration object given a handle.
    * The rest of the fields are generated by plugin in the handle into dummy
    * domains or keys.
    *
    * @param handle  Handle for the registration
    * @return        A complete registration object
    */
  def registrationFor(handle: String): Registration = {
    val email = s"$handle@example.com"
    Registration(
      handle = handle,
      publicKey = s"ssh-rsa AAAAAA $email",
      email = email
    )
  }

  /** Generates a user id based on a keyword.
    * 
    * @param keyword To fill in the auth id
    * @return        A user id on the default realm
    */
  def userIdFor(keyword: String): UserId = UserId(s"id-$keyword")

  /** Registers a new user by generating dummy data based on the handle.
    * See registrationFor and userIdFor methods for more details.
    *
    * @param handle  Handle to base the dummy info generation on
    * @param dao     Where to create the user in
    * @return        The newly created profile
    *
    * @see [[es.tid.cosmos.api.profile.CosmosProfileTestHelpers$#registrationFor]]
    * @see [[es.tid.cosmos.api.profile.CosmosProfileTestHelpers$#userIdFor]]
    */
  def registerUser(handle: String)(implicit dao: CosmosProfileDao): CosmosProfile =
    dao.withTransaction { implicit c =>
      dao.registerUser(
        userId = userIdFor(handle),
        reg = registrationFor(handle))
    }

  /** Lookup a user profile by their handle.
    *
    * @param handle the user's handle
    * @param dao the dao to use
    * @return the profile iff found
    */
  def lookup(handle: String)(implicit dao: CosmosProfileDao): Option[CosmosProfile] =
    dao.withTransaction{ implicit c =>
      dao.lookupByUserId(userIdFor(handle))
    }
}