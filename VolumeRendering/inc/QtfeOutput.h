/************************************************************************

Copyright (C) 2012 Eric Heitz (er.heitz@gmail.com). All rights reserved.

This file is part of Qtfe (Qt Transfer Function Editor).

Qtfe is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of 
the License, or (at your option) any later version.

Qtfe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Qtfe.  If not, see <http://www.gnu.org/licenses/>.

************************************************************************/

#ifndef _Q_TFE_OUTPUT_
#define _Q_TFE_OUTPUT_

#include <QWidget>

class QSpinBox;
class QMultiEditor;
class Qtfe;

// Warning : QtfeOutput is not supposed to be used outside Qtfe

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
