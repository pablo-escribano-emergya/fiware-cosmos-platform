package es.tid.cosmos.api

import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.api.sm.MockedServiceManagerComponent

/**
 * TODO: use real service manager
 *
 * @author sortega
 */
object ProductionApplication
  extends Application
  with MockedServiceManagerComponent
