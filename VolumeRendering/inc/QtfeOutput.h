#ifndef _Q_TFE_OUTPUT_
#define _Q_TFE_OUTPUT_

#include <QtWidgets/qwidget.h>

class QSpinBox;
class QMultiEditor;
class Qtfe;

//! An output of the Qtfe class. This should not get used outside of the Qtfe widget and therefor it is not documented
class QtfeOutput : public QWidget 
{
	Q_OBJECT

	friend class Qtfe;

private slots:
		void bindCanaltoR(int canal);
		void bindCanaltoG(int canal);
		void bindCanaltoB(int canal);
		void bindCanaltoA(int canal);
private:
		QtfeOutput(Qtfe* par);		
		virtual void paintEvent(QPaintEvent *event);
		const Qtfe* par;
		int R, G, A, B;
		QWidget * background;
		QSpinBox * qspinboxR;
		QSpinBox * qspinboxG;
		QSpinBox * qspinboxB;
		QSpinBox * qspinboxA;
};

#endif
