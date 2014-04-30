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

package es.tid.comos.infinity.client

import java.io.{InputStreamReader, BufferedReader}
import java.util.Date
import scala.concurrent.{Future, Await}
import scala.concurrent.duration._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class HttpInfinityClientTest extends FlatSpec
    with MustMatchers with FutureMatchers with HttpInfinityClientBehavior {

  "Reading metadata" must behave like new TestData {
    canHandleCommonErrors(_.pathMetadata(somePath))

    it must "succeed for an existing file" in new Fixture {
      val fileMetadata = dataFactory.fileMetadata(somePath, permissions)
      infinity.givenExistingPaths(fileMetadata)
      infinity.withServer {
        client.pathMetadata(somePath) must eventually(be(Some(fileMetadata)))
      }
    }

    it must "return nothing when file doesn't exist" in new Fixture {
      infinity.withServer {
        client.pathMetadata(somePath) must eventually(be('empty))
      }
    }

    it must "fail with protocol mismatch exception for unparseable responses" in new Fixture {
      infinity.givenWillReturnGibberish()
      infinity.withServer {
        client.pathMetadata(somePath) must eventuallyFailWith[ProtocolMismatchException]
      }
    }
  }

  "Creating a file or directory" must behave like new TestData {
    canHandleCommonErrors(_.createFile(somePath / "aFile", permissions))
    canHandleAlreadyExistsError(_.createFile(somePath / "aFile", permissions))
    canHandleNotFoundError(_.createFile(somePath / "aFile", permissions))

    it must "create a new file" in new Fixture {
      val directoryMetadata = dataFactory.dirMetadata(somePath, permissions)
      val filePath = somePath / "newFile"
      val expectedMetadata = dataFactory.fileMetadata(filePath, permissions)
      infinity.givenExistingPaths(directoryMetadata)
      infinity.withServer {
        client.createFile(filePath, permissions) must eventuallySucceed
        client.pathMetadata(filePath) must eventually (be (Some(expectedMetadata)))
      }
    }
  }

  "Creating a directory" must behave like new TestData {
    canHandleCommonErrors(_.createDirectory(somePath / "aDir", permissions))
    canHandleAlreadyExistsError(_.createDirectory(somePath / "aDir", permissions))
    canHandleNotFoundError(_.createDirectory(somePath / "aDir", permissions))

    it must "create a new directory" in new Fixture {
      val parentDirectory = dataFactory.dirMetadata(somePath, permissions)
      val newDirPath = somePath / "newDirectory"
      val expectedMetadata = dataFactory.dirMetadata(newDirPath, permissions)
      infinity.givenExistingPaths(parentDirectory)
      infinity.withServer {
        client.createDirectory(newDirPath, permissions) must eventuallySucceed
        client.pathMetadata(newDirPath) must eventually(be(Some(expectedMetadata)))
      }
    }
  }

  "Moving a file or directory" must behave like new TestData {
    canHandleCommonErrors(_.move(somePath / "aFile", somePath / "aDir"))
    canHandleAlreadyExistsError(_.move(somePath / "aFile", somePath / "aDir"))
    canHandleNotFoundError(_.move(somePath / "aFile", somePath / "aDir"))

    it must "move an existing file to an existing directory" in new Fixture {
      val origin = dataFactory.dirMetadata(somePath, permissions)
      val destination = dataFactory.dirMetadata(Path.absolute("/some/otherpath"), permissions)
      val file = dataFactory.fileMetadata(somePath / "aFile", permissions)
      infinity.givenExistingPaths(origin, destination, file)
      infinity.withServer {
        client.move(somePath / "aFile", destination.path) must eventuallySucceed
        client.pathMetadata(destination.path / "aFile") must eventually (be (
          Some(dataFactory.fileMetadata(destination.path / "aFile", permissions))))
      }
    }

    it must "move an existing directory to another existing directory" in new Fixture {
      val origin = dataFactory.dirMetadata(somePath, permissions)
      val destination = dataFactory.dirMetadata(Path.absolute("/some/otherpath"), permissions)
      val directory = dataFactory.dirMetadata(somePath / "aDir", permissions)
      infinity.givenExistingPaths(origin, destination, directory)
      infinity.withServer {
        client.move(somePath / "aDir", destination.path) must eventuallySucceed
        client.pathMetadata(destination.path / "aDir") must eventually (be (
          Some(dataFactory.dirMetadata(destination.path / "aDir", permissions))))
      }
    }
  }

  "Changing the owner" must behave like new TestData {
    canHandleCommonErrors(_.changeOwner(somePath, "newOwner"))
    canHandleNotFoundError(_.changeOwner(somePath, "newOwner"))

    it must "succeed for an existing file" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(somePath / "aFile", permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.withServer {
        client.changeOwner(file.path, "newOwner") must eventuallySucceed
        client.pathMetadata(file.path) must eventually (be (Some(file.copy(owner = "newOwner"))))
      }
    }
  }

  "Changing the group" must behave like new TestData {
    canHandleCommonErrors(_.changeGroup(somePath, "newGroup"))
    canHandleNotFoundError(_.changeGroup(somePath, "newGroup"))

    it must "succeed for an existing file" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(somePath / "aFile", permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.withServer {
        client.changeGroup(file.path, "newGroup") must eventuallySucceed
        client.pathMetadata(file.path) must eventually (be (Some(file.copy(group = "newGroup"))))
      }
    }
  }

  "Changing permissions" must behave like new TestData {
    val newPermissions = PermissionsMask.fromOctal("700")
    canHandleCommonErrors(_.changePermissions(somePath, newPermissions))
    canHandleNotFoundError(_.changePermissions(somePath, newPermissions))

    it must "succeed for an existing file" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(somePath / "aFile", permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.withServer {
        client.changePermissions(file.path, newPermissions) must eventuallySucceed
        client.pathMetadata(file.path) must eventually (be (
          Some(file.copy(permissions = newPermissions))))
      }
    }
  }

  "Deleting" must behave like new TestData {
    canHandleCommonErrors(_.delete(somePath, isRecursive = true))
    canHandleNotFoundError(_.delete(somePath, isRecursive = true))

    it must "succeed for an existing file" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(somePath / "aFile", permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.withServer {
        client.delete(file.path) must eventuallySucceed
        client.pathMetadata(file.path) must eventually (be ('empty))
      }
    }

    it must "succeed for an existing directory" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val directory = dataFactory.dirMetadata(somePath / "aDir", permissions)
      infinity.givenExistingPaths(parent, directory)
      infinity.withServer {
        client.delete(directory.path) must eventuallySucceed
        client.pathMetadata(directory.path) must eventually (be ('empty))
      }
    }
  }

  "Reading content" must behave like new TestData {
    canHandleCommonErrors(_.read(somePath, offset = None, length = None))
    // when metadata is not available
    canHandleNotFoundError(_.read(somePath, offset = None, length = None))

    it must "succeed for an existing file with content" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(somePath / "aFile", permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.givenExistingContent(file, "aContent")
      infinity.withServer {
        contentOf(client.read(file.path, offset = None, length = None)) must be ("aContent")
      }
    }

    it must "fail to get content for directories" in new Fixture {
      val directory = dataFactory.dirMetadata(somePath, permissions)
      infinity.givenExistingPaths(directory)
      infinity.withServer {
        client.read(
          directory.path,
          offset = None,
          length = None
        ) must eventuallyFailWith[IllegalArgumentException]
      }
    }
  }

  trait TestData {
    val somePath = Path.absolute("/some/path")
    val aDate = new Date(1398420798000L)
    val permissions = PermissionsMask.fromOctal("644")
  }

  trait Fixture extends TestData {
    val infinity = new MockInfinityServer(metadataPort = 8898, defaultDate = aDate)
    val dataFactory = infinity.DataFactory
    val client = new HttpInfinityClient(infinity.metadataEndpoint)
    val timeOut = 10.seconds

    def contentOf(reader_> : Future[InputStreamReader]): String = {
      val reader = Await.result(reader_>, timeOut)
      val contentStream =
        Stream.continually(new BufferedReader(reader).readLine).takeWhile(_ != null)
      contentStream.toList.mkString
    }
  }
}
