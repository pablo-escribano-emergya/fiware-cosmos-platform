#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include <QGraphicsSvgItem>
#include <QColor>

class QueueItem : public QGraphicsSvgItem
{
	Q_OBJECT

public:
	QueueItem();
	QueueItem(QSvgRenderer* renderer)
		{ setSharedRenderer(renderer); };
	QueueItem(const QString &fileName, QGraphicsItem *parent=0)
		: QGraphicsSvgItem(fileName, parent) {};
	~QueueItem();

	virtual void initText(QString text);
	virtual void scaleToDefaultSize();

	void setPos(const QPointF &pos);
	void setPos(qreal x,qreal y);

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    virtual void putOnTop() { setZValue(qreal(0.1)); };
    virtual void restoreOrder() { setZValue(qreal(0)); };

private:
    QSize default_size;
    QRectF* bound_rect;

protected:
    QGraphicsTextItem* text_item;

};

#endif // QUEUEITEM_H
