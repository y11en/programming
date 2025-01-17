#include "edge.h"
#include "node.h"

#include <math.h>
#include <QPainter>

static const double Pi = 3.14159265358979323846264338327950288419717;
static const double TwoPi = 2.0 * Pi;

Edge::Edge(Node *sourceNode, Node *destNode)
	: arrowSize(10)
{
	// 禁止响应鼠标事件
	// 即边不响应鼠标
	setAcceptedMouseButtons(0);

	source = sourceNode;
	dest = destNode;
	source->addEdge(this);
	dest->addEdge(this);	

	adjust();
}

Edge::~Edge()
{}

Node *Edge::sourceNode() const
{
	return source;
}

void Edge::setSourceNode(Node *node)
{
	source = node;
	adjust();
}
	
Node *Edge::destNode() const
{
	return dest;
}

void Edge::setDestNode(Node *node)
{
	dest = node;
	adjust();
}

void Edge::adjust()
{
	if ((source == NULL) || (dest == NULL))
	{
		return;
	} 

	QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
	qreal length = line.length();
	QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);

	prepareGeometryChange();
	sourcePoint = line.p1() + edgeOffset;
	destPoint 	= line.p2() - edgeOffset;
}

QRectF Edge::boundingRect() const
{
	if ((source == NULL) || (dest == NULL))
	{
		return QRectF();
	}	

	qreal penWidth = 1;
	qreal extra = (penWidth + arrowSize) / 2.0;
	
	return QRectF(	sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
					destPoint.y() - sourcePoint.y()))
					.normalized()
					.adjusted(-extra, -extra, extra, extra);	
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if ((source == NULL) || (dest == NULL))
	{
		return;
	}

	QLineF line(sourcePoint, destPoint);
	painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->setRenderHint(QPainter::Antialiasing);
	painter->drawLine(line);

	double angle = ::acos(line.dx() / line.length());
	
	if (line.dy() >= 0)
	{
		angle = TwoPi - angle;
	}

	QPointF sourceArrowP1 = sourcePoint + QPointF(sin(angle + Pi / 3) * arrowSize, cos(angle + Pi / 3) * arrowSize);
	QPointF sourceArrowP2 = sourcePoint + QPointF(sin(angle + Pi - Pi / 3) * arrowSize, cos(angle + Pi - Pi / 3) * arrowSize);
	QPointF destArrowP1 = destPoint + QPointF(sin(angle - Pi / 3) * arrowSize, cos(angle - Pi / 3) * arrowSize);
	QPointF destArrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize, cos(angle - Pi + Pi / 3) * arrowSize);
	
	painter->setBrush(Qt::black);
	painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
	painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}
