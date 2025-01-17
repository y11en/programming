#ifndef WINDOW_H 
#define WINDOW_H

#include <QWidget>

class QDataWidgetMapper;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QStandardItemModel;
class QTextEdit;

class Window : public QWidget
{
	Q_OBJECT
	
	public:
		Window(QWidget *parent = 0);

	private slots:
		void updateButtons(int row);
	
	private:
		void setupModel();
		
		QLabel *nameLabel;
		QLabel *addressLabel;
		QLabel *ageLabel;
		QLineEdit *nameEdit;
		QTextEdit *addressEdit;
		QSpinBox *ageSpinBox;
		QPushButton *nextButton;
		QPushButton *previousButton;
			
		QStandardItemModel *model;
		QDataWidgetMapper *mapper;
};

#endif
