#include "mouse.h"

#include <QGraphicsScene> 
#include <QPainter> 
#include <QStyleOption>

#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;
static const double TwoPi = 2.0 * Pi;

// 将角度angle限制在 0--2Pi 之间
static qreal normalizeAngle(qreal angle)
{
	while (angle < 0)
	{
		angle += TwoPi;
	}

	while (angle > TwoPi)
	{
		angle -= TwoPi;
	}

	return angle;
}

Mouse::Mouse()
	: angle(0), speed(0), mouseEyeDirection(0), color(qrand() % 256, qrand() % 256, qrand() % 256)
{
	rotate(qrand() % (360 * 16));
	startTimer(1000 / 33);
}

QRectF Mouse::boundingRect() const
{
	qreal adjust = 0.5;
	return QRectF(-18 - adjust, -22 - adjust, 36 + adjust, 60 + adjust);
}

QPainterPath Mouse::shape() const
{
	QPainterPath path;
	path.addRect(-10, -20, 20, 40);
	
	return path;
}

void Mouse::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setBrush(color);
	painter->drawEllipse(-10, -20, 20, 40);

	painter->setBrush(Qt::white);
	painter->drawEllipse(-10, -17, 8, 8);
	painter->drawEllipse(2, -17, 8, 8);

	painter->setBrush(Qt::black);
	painter->drawEllipse(QRectF(-2, -22, 4, 4));

	painter->drawEllipse(QRectF(-8.0 + mouseEyeDirection, -17, 4, 4));
	painter->drawEllipse(QRectF(4.0 + mouseEyeDirection , -17, 4, 4));

	painter->setBrush(scene()->collidingItems(this).isEmpty() ? Qt::darkYellow : Qt::red);	
	painter->drawEllipse(-17, -12, 16, 16);
	painter->drawEllipse(1, -12, 16, 16);

	QPainterPath path(QPointF(0, 20));
	path.cubicTo(-5, 22, -5, 22, 0, 25);
	path.cubicTo(5, 27, 5, 32, 0, 30);
	path.cubicTo(-5, 32, -5, 42, 0, 35);
	painter->setBrush(Qt::NoBrush);
	painter->drawPath(path);
}

void Mouse::timerEvent(QTimerEvent *event)
{
	// item's Point(0, 0) to Senen's Point(0, 0)
	QLineF lineToCenter(QPointF(0, 0), mapFromScene(0, 0));
	
	if (lineToCenter.length() > 150)
	{
		qreal angleToCenter = ::acos(lineToCenter.dx() / lineToCenter.length());
		
		if (lineToCenter.dy() < 0)
		{
			angleToCenter = TwoPi - angleToCenter;
		}
		
		angleToCenter = normalizeAngle((Pi - angleToCenter) + Pi / 2);

		if (angleToCenter < Pi && angleToCenter > Pi / 4)
		{
			angle += (angle < -Pi / 2) ? 0.25 : -0.25;
		}
		else if (angleToCenter > Pi && angleToCenter < (Pi + Pi / 2 + Pi / 4))
			{
				angle += (angle < Pi / 2) ? 0.25 : -0.25;
			}
	}
	else if (::sin(angle) < 0)
		{
			angle += 0.25;
		}
		else if (::sin(angle) > 0)
		{
			angle -= 0.25;
		}

	QList<QGraphicsItem *> dangerMice = scene()->items(QPolygonF()
														<< mapToScene(0, 0)
														<< mapToScene(-30, -50)
														<< mapToScene(30, -50));	
	
	foreach (QGraphicsItem *item, dangerMice)
	{
		if (item == this)
		{
			continue;
		}
		
		QLineF lineToMouse(QPointF(0, 0), mapFromItem(item, 0, 0));
		qreal angleToMouse = ::acos(lineToMouse.dx() / lineToCenter.length());

		if (lineToMouse.dy() < 0)
		{
			angleToMouse = TwoPi - angleToMouse;
		}

		angleToMouse = normalizeAngle((Pi - angleToMouse) + Pi / 2);
		
		if (angleToMouse >= 0 && angleToMouse < Pi / 2)
		{
			angle += 0.5;
		}
		else if (angleToMouse <= TwoPi && angleToMouse > (TwoPi - Pi / 2))
		{
			angle -= 0.5;
		}
	}

	if (dangerMice.size() > 1 && (qrand() % 10) == 0)
	{
		if (qrand() % 1)
		{
			angle += (qrand() % 100) / 500.0;
		}
		else
		{
			angle -= (qrand() % 100) / 500.0;
		}
	}

	speed += (-50 + qrand() % 100) / 100.0;

	qreal dx = ::sin(angle) * 10;
	mouseEyeDirection = (qAbs(dx / 5) < 1) ? 0 : dx / 5;
	
	rotate(dx);
	setPos(mapToParent(0, -(3 + sin(speed) * 3)));
}	
