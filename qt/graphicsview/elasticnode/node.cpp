#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include "edge.h"
#include "node.h"
#include "graphwidget.h"

Node::Node(GraphWidget *graphWidget)
	: graph(graphWidget)
{
	setFlag(ItemIsMovable);
	setCacheMode(DeviceCoordinateCache);
	setZValue(1);
}

void Node::addEdge(Edge *edge)
{
	edgeList << edge;
	edge->adjust();	
}

QList<Edge *> Node::edges() const
{
	return edgeList;
}

// 实际上是设置新的坐标
void Node::calculateForces()
{
	if (scene() == NULL || scene()->mouseGrabberItem() == this)
	{
		newPos = pos();
		return;
	}

	qreal xvel = 0;
	qreal yvel = 0;

	// 将所有将该点推开的力累加	
	foreach (QGraphicsItem *item, scene()->items())
	{
		Node *node = qgraphicsitem_cast<Node *>(item);
		
		if (node == NULL)
		{
			continue;
		}
	
		QLineF line(mapFromItem(node, 0, 0), QPointF(0, 0));
		qreal dx = line.dx();
		qreal dy = line.dy();
	
		double I = 2.0 * (dx * dx + dy * dy);	
		
		if (I > 0)
		{
			xvel += (dx * 150.0) / I;
			yvel += (dy * 150.0) / I;
		}
	}	

	// 将所有将该点聚合的力累减
	double weight = (edgeList.size() + 1) * 10;
	
	foreach (Edge *edge, edgeList)
	{
		QPointF pos;
		if (edge->sourceNode() == this)
		{
			pos = mapFromItem(edge->destNode(), 0, 0);
		}
		else
		{
			pos = mapFromItem(edge->sourceNode(), 0, 0);
		}
		
		xvel += pos.x() / weight;
		yvel += pos.y() / weight;
	}

	if ((qAbs(xvel) < 0.1) && (qAbs(yvel) < 0.1))
	{
		xvel = yvel = 0;
	}

	QRectF sceneRect = scene()->sceneRect();
	newPos = pos() + QPointF(xvel, yvel);
	newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
	newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
};

bool Node::advance()
{
	if (newPos == pos())
	{
		return false;
	}

	setPos(newPos);
	return true;
}

QRectF Node::boundingRect() const
{
	qreal adjust = 2;
	return QRectF(	-10 - adjust, -10 - adjust, 
					23 + adjust, 23 + adjust);
}

QPainterPath Node::shape() const
{
	QPainterPath path;
	path.addEllipse(-10, -10, 20, 20);
	return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)	
{
	// draw the shadow
	painter->setPen(Qt::NoPen);	
	painter->setBrush(Qt::darkGray);
	painter->drawEllipse(-7, -7, 20, 20);

	QRadialGradient gradient(-3, -3, 10);
	
	if (option->state & QStyle::State_Sunken)
	{
		gradient.setCenter(3, 3);
		gradient.setFocalPoint(3, 3);
		gradient.setColorAt(1, QColor(Qt::yellow).light(120));
		gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
	}
	else
	{
		gradient.setColorAt(0, Qt::yellow);
		gradient.setColorAt(1, Qt::darkYellow);
	}

	painter->setBrush(gradient);
	painter->setPen(QPen(Qt::black, 0));
	painter->drawEllipse(-10, -10, 20, 20);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch (change)
	{
		case ItemPositionHasChanged:
			foreach (Edge *edge, edgeList)
			{
				edge->adjust();
			}
			graph->itemMoved();
			break;
	
		default:
			break;
	}

	return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)	
{
	update();
	QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	update();
	QGraphicsItem::mouseReleaseEvent(event);
}
