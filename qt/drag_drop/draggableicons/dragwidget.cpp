#include <QtGui>

#include "dragwidget.h"

DragWidget::DragWidget(QWidget *parent)
	: QFrame(parent)
{
	setMinimumSize(200, 200);
	setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	setAcceptDrops(true);

	QLabel *boatIcon = new QLabel(this);
	boatIcon->setPixmap(QPixmap(":/images/boat.png"));
	boatIcon->move(20, 20);
	boatIcon->show();
	boatIcon->setAttribute(Qt::WA_DeleteOnClose);

	QLabel *carIcon = new QLabel(this);
	carIcon->setPixmap(QPixmap(":/images/car.png"));
	carIcon->move(120, 20);
	carIcon->show();
	carIcon->setAttribute(Qt::WA_DeleteOnClose);
	
	QLabel *houseIcon = new QLabel(this);
	houseIcon->setPixmap(QPixmap(":/images/house.png"));
	houseIcon->move(20, 120);
	//houseIcon->show();
	houseIcon->setAttribute(Qt::WA_DeleteOnClose);
}

void DragWidget::dragEnterEvent(QDragEnterEvent *event)
{
qDebug() << "DragWidget::dragEnterEvent";
	if (event->mimeData()->hasFormat("application/x-dnditemdata"))
	{
		if (event->source() == this)
		{
			event->setDropAction(Qt::MoveAction);
			event->accept();
		}
		else
		{
			event->acceptProposedAction();
		}
	}
	else
	{
		event->ignore();
	}
}

void DragWidget::dragMoveEvent(QDragMoveEvent *event)
{
qDebug() << "DragWidget::dragMoveEvent";
	if (event->mimeData()->hasFormat("application/x-dnditemdata"))
	{
		if (event->source() == this)
		{
			event->setDropAction(Qt::MoveAction);
			event->accept();
		}
		else
		{
			event->acceptProposedAction();
		}
	}
	else
	{
		event->ignore();
	}
}

void DragWidget::dropEvent(QDropEvent *event)
{
qDebug() << "DragWidget::dropEvent";
	if (event->mimeData()->hasFormat("application/x-dnditemdata"))
	{
		QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
		QDataStream dataStream(&itemData, QIODevice::ReadOnly);
	
		QPixmap pixmap;
		QPoint offset;
		dataStream >> pixmap >> offset;
	
		QLabel *newIcon = new QLabel(this);
		newIcon->setPixmap(pixmap);
		newIcon->move(event->pos() - offset);
		newIcon->show();
		newIcon->setAttribute(Qt::WA_DeleteOnClose);
		
		if (event->source() == this)
		{
			event->setDropAction(Qt::MoveAction);
			event->accept();
		}
		else
		{
			event->acceptProposedAction();
		}
	}
	else
	{
		event->ignore();
	}
}

void DragWidget::mousePressEvent(QMouseEvent *event)
{
qDebug() << "DragWidget::mousePressEvent";
	QLabel *child = static_cast<QLabel *>(childAt(event->pos()));
	
	if (child == NULL)
		return;

	QPixmap pixmap = *child->pixmap();
	
	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	dataStream << pixmap << QPoint(event->pos() - child->pos());

	QMimeData *mimeData = new QMimeData;
	mimeData->setData("application/x-dnditemdata", itemData);
	
	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);	
	drag->setPixmap(pixmap);
	drag->setHotSpot(event->pos() - child->pos());

	if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
	{
		// move in the same widget
		child->close();
	}
	else
	{
		// move to another widget
		child->show();
		child->setPixmap(pixmap);
	}
}

void DragWidget::mouseReleaseEvent(QMouseEvent *event)
{
qDebug() << "DragWidget::mouseReleaseEvent";
}

void DragWidget::mouseMoveEvent(QMouseEvent *event)
{
qDebug() << "DragWidget::mouseMoveEvent";

}
