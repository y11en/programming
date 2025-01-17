#ifndef HELPER_H
#define HELPER_H

#include <QBrush>
#include <QFont>
#include <QPen>

class QPainter;
class QPaintEvent;

class Helper
{
	public:
		Helper();
	
	public:
		void paint(QPainter *painter, QPaintEvent *event, int elapsed);

	private:
		QBrush background;
		QBrush circleBrush;
		QFont textFont;
		QPen circlePen;
		QPen textPen;
};

#endif
