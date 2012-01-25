#ifndef GLOBALS_H
#define GLOBALS_H

/* ****************************************************************************
*
* FILE                     globals.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include <pthread.h>            // pthread_t

#include <QGraphicsItem>

#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "SamsonSupervisor.h"   // SamsonSupervisor
#include "TabManager.h"         // TabManager
#include "ConnectionMgr.h"      // ConnectionMgr
#include "QueueMgr.h"           // QueueMgr
#include "SourceMgr.h"          // SourceMgr
#include "ResultMgr.h"          // ResultMgr
#include "UserMgr.h"            // UserMgr
#include "user.h"               // User
#include "InfoWin.h"            // InfoWin



class SamsonSupervisor;
/* ****************************************************************************
*
* Global variables
*/
extern ss::Network*         networkP;
extern SamsonSupervisor*    supervisorP;
extern TabManager*          tabManager;
extern pthread_t            networkThread;
extern pthread_t            qtThread;
extern ss::Endpoint*        logServerEndpoint;
extern int                  logServerFd;
extern bool                 qtAppRunning;
extern ConnectionMgr*       connectionMgr;
extern QueueMgr*            queueMgr;
extern SourceMgr*           sourceMgr;
extern ResultMgr*           resultMgr;
extern UserMgr*             userMgr;
extern ss::Delilah*         delilah;
extern ss::DelilahConsole*  delilahConsole;
extern User*                userP;
extern int                  mainWinWidth;
extern int                  mainWinHeight;
extern InfoWin*             infoWin;
extern int                  workers;
extern bool                 noPopups;



/* ****************************************************************************
*
* imagePath -
*/
extern char* imagePath(const char* fileName, char* buf, int bufLen);

#endif