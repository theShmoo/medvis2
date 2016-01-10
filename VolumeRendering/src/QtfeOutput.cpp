#include "QtfeOutput.h"
#include "Qtfe.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>

#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QPainter>

QtfeOutput::QtfeOutput(Qtfe* par) : QWidget(par), par(par)
{
	R = -1;
	B = -1;
	G = -1;
	A = -1;

	background = new QWidget(this);

// 	QLabel * labelR = new QLabel("R", this); 
// 	qspinboxR = new QSpinBox(this); 
// 	qspinboxR->setMinimum(-1);
// 	qspinboxR->setValue(R);
// 	QObject::connect(qspinboxR, SIGNAL(valueChanged(int)), this, SLOT(bindCanaltoR(int)));
// 
// 	QLabel * labelG = new QLabel("G", this); 
// 	qspinboxG = new QSpinBox(this);	
// 	qspinboxG->setMinimum(-1);
// 	qspinboxG->setValue(G);
// 	QObject::connect(qspinboxG, SIGNAL(valueChanged(int)), this, SLOT(bindCanaltoG(int)));
// 
// 	QLabel * labelB = new QLabel("B", this); 
// 	qspinboxB = new QSpinBox(this);	
// 	qspinboxB->setMinimum(-1);
// 	qspinboxB->setValue(B);
// 	QObject::connect(qspinboxB, SIGNAL(valueChanged(int)), this, SLOT(bindCanaltoB(int)));
// 
// 	QLabel * labelA = new QLabel("A", this); 
// 	qspinboxA = new QSpinBox(this);	
// 	qspinboxA->setMinimum(-1);
// 	qspinboxA->setValue(A);
// 	QObject::connect(qspinboxA, SIGNAL(valueChanged(int)), this, SLOT(bindCanaltoA(int)));

// 	QGridLayout * layoutCanals = new QGridLayout;
// 	layoutCanals->addWidget(labelR, 0, 0);
// 	layoutCanals->addWidget(qspinboxR, 0, 1);
// 	layoutCanals->addWidget(labelG, 0, 2);
// 	layoutCanals->addWidget(qspinboxG, 0, 3);
// 	layoutCanals->addWidget(labelB, 1, 0);
// 	layoutCanals->addWidget(qspinboxB, 1, 1);
// 	layoutCanals->addWidget(labelA, 1, 2);
// 	layoutCanals->addWidget(qspinboxA, 1, 3);
// 
 	QHBoxLayout * layout = new QHBoxLayout(this);
  layout->addWidget(background);
  background->setMinimumHeight(40);
// 	layout->addLayout(layoutCanals);
}

void QtfeOutput::bindCanaltoR(int canal)
{
	if(-1 <= canal && canal < par->dim())
		R = canal;
// 	qspinboxR->setValue(R);
	repaint();
}

void QtfeOutput::bindCanaltoG(int canal)
{
	if(-1 <= canal && canal < par->dim())
		G = canal;
	//qspinboxG->setValue(G);
	repaint();
}

void QtfeOutput::bindCanaltoB(int canal)
{
	if(-1 <= canal && canal < par->dim())
		B = canal;
	//qspinboxB->setValue(B);
	repaint();
}

void QtfeOutput::bindCanaltoA(int canal)
{
	if(-1 <= canal && canal < par->dim())
		A = canal;
	//qspinboxA->setValue(A);
	repaint();
}

void QtfeOutput::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	
	QImage image(background->size(), QImage::Format_ARGB32);

	for(int i=0 ; i < background->width() ; ++i)
	{
		qreal x = i / (qreal)background->width();
		qreal r = par->evalf(R,x);
		qreal g = par->evalf(G,x);
		qreal b = par->evalf(B,x);
		qreal a = (-1<A && A<par->dim()) ? par->evalf(A,x) : 1.0;

		unsigned int R = r * 255;
		unsigned int G = g * 255;
		unsigned int B = b * 255;
		unsigned int color = (R<<16) + (G<<8) + B;

		int jmin = (1.0-a) * background->height();

		for(int j=0 ; j < jmin ; ++j)
		{
			image.setPixel(i,j,color);
		}
		for(int j=jmin ; j < background->height() ; ++j)
		{
			image.setPixel(i,j,color+0xff000000);
		}
	}

	painter.drawImage(background->pos(),image);
	QWidget::paintEvent(event);
}