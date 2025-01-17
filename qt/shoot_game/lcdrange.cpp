#include <QLCDNumber>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>

#include "lcdrange.h"

LCDRange::LCDRange(QWidget *parent) : QWidget(parent)
{
	init();
}

LCDRange::LCDRange(const QString &text, QWidget *parent) : QWidget(parent)
{
	init();
	setText(text);
}

void LCDRange::init()
{
	QLCDNumber *lcd = new QLCDNumber(2);
	lcd->setSegmentStyle(QLCDNumber::Filled);
	
	slider = new QSlider(Qt::Horizontal);
	slider->setRange(0, 99);
	slider->setValue(0);
	
	label = new QLabel;
	label->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);	
	connect(slider, SIGNAL(valueChanged(int)), lcd, SLOT(display(int)));
	connect(slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(lcd);
	layout->addWidget(slider);
	layout->addWidget(label);
	setLayout(layout);

	setFocusProxy(slider);
}

int LCDRange::value() const
{
	return slider->value();
}

QString LCDRange::text() const
{
	return label->text();
}

void LCDRange::setValue(int value)
{
	slider->setValue(value);
}

void LCDRange::setText(const QString &text)
{
	label->setText(text);
}

void LCDRange::setRange(int minValue, int maxValue)
{
	if (minValue < 0 || maxValue > 99 || minValue > maxValue)
	{
		qWarning("LCDRange::setRange(%d, %d)\n"
				"\tRange must be 0..99\n"
				"\tand minValue must not be greater than maxValue", minValue, maxValue);	

		return;
	}
	
	slider->setRange(minValue, maxValue);
}


