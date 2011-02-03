/* ****************************************************************************
*
* FILE                     DelilahConnection.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
#include <QPen>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "misc.h"               // centerCoordinates
#include "DelilahQueue.h"       // DelilahQueue
#include "DelilahScene.h"       // DelilahScene
#include "DelilahConnection.h"  // Own interface



/* ****************************************************************************
*
* Constructor
*/
DelilahConnection::DelilahConnection(DelilahScene* sceneP, DelilahQueue* from, DelilahQueue* to)
{
#if 1
	QPen    pen;

	scene    = sceneP;
	qFromP   = from;
	qToP     = to;
	lineItem = scene->addLine(0, 0, 0, 0);

	pen.setWidth(3);
	lineItem->setPen(pen);

	lineItem->stackBefore(qFromP->pixmapItem);
	lineItem->stackBefore(qToP->pixmapItem);

	move();
#else
	qreal   fromX;
	qreal   fromY;
	qreal   toX;
	qreal   toY;
	QPen    pen;
	
	centerCoordinates(from->pixmapItem, &fromX, &fromY);
	centerCoordinates(to->pixmapItem,   &toX,   &toY);

	scene    = sceneP;
	qFromP   = from;
	qToP     = to;
	lineItem = scene->addLine(fromX, fromY, toX, toY);

	pen.setWidth(3);
	qFromP->connectionLine->setPen(pen);

	lineItem->stackBefore(qFromP->pixmapItem);
	lineItem->stackBefore(qToP->pixmapItem);
#endif
}



/* ****************************************************************************
*
* Destructor
*/
DelilahConnection::~DelilahConnection()
{
	delete lineItem;
}



/* ****************************************************************************
*
* DelilahConnection::move
*/
void DelilahConnection::move(void)
{
	qreal   fromX;
	qreal   fromY;
	qreal   toX;
	qreal   toY;
	
	centerCoordinates(qFromP->pixmapItem, &fromX, &fromY);
	centerCoordinates(qToP->pixmapItem,   &toX,   &toY);

	lineItem->setLine(fromX, fromY, toX, toY);
}
