#include <QtWidgets/qwidget.h>
#include <QPainter>
#include <QImage>

#include <Qtfe.h>

#include <math.h>

class QImageTweak : public QWidget
{
	Q_OBJECT

public:

	QImageTweak(Qtfe * editor) : editor(editor)
	{
		resize(256,256);
		QObject::connect(editor, SIGNAL(functionChanged()), this, SLOT(colorMapChanged()));
    editor->addCanal(0, "Red");
    editor->addCanal(1, "Green");
    editor->addCanal(2, "Blue");
    editor->addCanal(3, "Alpha");
		editor->addOutputs(1);
		editor->bindCanaltoOutputR(0,0);
		editor->bindCanaltoOutputG(1,0);
    editor->bindCanaltoOutputB(2, 0);
    editor->bindCanaltoOutputA(3, 0);
	}

protected slots:
	void colorMapChanged(){repaint();}

protected:
	Qtfe* editor;

	virtual void paintEvent(QPaintEvent* event)
	{
		QWidget::paintEvent(event);

		QPainter painter(this);
		QImage image(size(), QImage::Format_RGB32);

		for(int j=0 ; j < height() ; ++j)
		for(int i=0 ; i < width() ; ++i)		
		{
			qreal x = i / (qreal)width();
			qreal y = j / (qreal)height();
			qreal v = 0.5 + 
				0.1*sin(-90.0*x + 50.0*y + 30.0) + 
				0.1*sin(14.0*x - 25.0*y + 10.0) + 
				0.1*sin(-2.1*x - 4.7*y + 70.5) + 
				0.1*sin(6.2*x + 70.3*y + 50.0) + 
				0.1*sin(1.4*x - 64.4*y + 90.5);

			qreal r = editor->evalf(0,v);
			qreal g = editor->evalf(1,v);
			qreal b = editor->evalf(2,v);

			unsigned int R = r * 255;
			unsigned int G = g * 255;
			unsigned int B = b * 255;
			unsigned int color = (R<<16) + (G<<8) + B;
			image.setPixel(i,j,color);
		}

		painter.drawImage(0,0,image);
	}
};