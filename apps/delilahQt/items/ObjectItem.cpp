/*
 * ObjectItem.cpp
 *
 *  Created on: Oct 22, 2010
 *      Author: ania
 */

#include <QSize>

#include "ObjectItem.h"
#include "ProcessScene.h"
#include "globals.h"

ObjectItem::ObjectItem(QSvgRenderer* renderer, QGraphicsItem* parent)
	: QGraphicsSvgItem(parent)
{
	setSharedRenderer(renderer);
	init();
}

ObjectItem::~ObjectItem()
{}

void ObjectItem::init()
{
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

void ObjectItem::setPos(const QPointF &pos)
{
	QRectF bound_rect(QGraphicsSvgItem::boundingRect());
	QPointF new_pos = pos - mapToScene(bound_rect.center());

	QGraphicsSvgItem::setPos(new_pos);
}

void ObjectItem::setPos(qreal x,qreal y)
{
	setPos(QPointF(x, y));
}

void ObjectItem::setDefaultSize()
{
	if (!default_size.isValid())
		initializeDefaultSize();

	setSize(default_size);
}

void ObjectItem::setSize(QSize size)
{
	QSizeF bound_size = QRectF(QGraphicsSvgItem::boundingRect()).size();
	bound_size.scale(size.width(), size.height(), Qt::KeepAspectRatio);
	qreal scale_factor = bound_size.width() / QGraphicsSvgItem::boundingRect().width();

	prepareGeometryChange();
	setScale(scale_factor);
}

void ObjectItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	ProcessScene* s = (ProcessScene*)scene();
	switch(s->getTool())
	{
		case TOOL_SELECT:
			//TODO:
			QGraphicsSvgItem::mousePressEvent(event);
			putOnTop();
			break;
		default:
			break;
	}
}

void ObjectItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	ProcessScene* s = (ProcessScene*)scene();
	switch(s->getTool())
	{
		case TOOL_SELECT:
			//TODO:
			QGraphicsSvgItem::mouseMoveEvent(event);
			break;
		default:
			break;
	}
}

void ObjectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	ProcessScene* s = (ProcessScene*)scene();
	switch(s->getTool())
	{
		case TOOL_SELECT:
			//TODO:
			QGraphicsSvgItem::mouseReleaseEvent(event);
			restoreOrder();
			break;
		default:
			break;
	}

}
