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

#ifndef _Q_TFE_
#define _Q_TFE_

#include <QtWidgets/qwidget.h>
#include <QVector>
class QLabel;

#include "QtfeCanal.h"
#include "QtfeOutput.h"

class Qtfe : public QWidget 
{
	Q_OBJECT

public:
	Qtfe(QWidget * parent = 0, Qt::WindowFlags f = 0);
	~Qtfe();

	// add a canal
	void addCanal(int n, QString name);
	// add an RGBA output
	void addOutputs(int n);
	// bind a canal to an output (-1 to disable the canal)
	void bindCanaltoOutputR(int canal, int output);
	void bindCanaltoOutputG(int canal, int output);
	void bindCanaltoOutputB(int canal, int output);
	void bindCanaltoOutputA(int canal, int output);
	// transfer function dimension (canal number)
	int dim() const;

  void setSource(qreal dMin, qreal dMax);

  void removeAllRGBPoints();

  void addRGBAPoint(qreal x, qreal r, qreal g, qreal b, qreal a);

  void changeXofRGBPoints();

  void getAllRGBAPoints(qreal* x, qreal* r, qreal* g, qreal* b, qreal* a);

  uint getNumPoints();

	// evaluate canal at x in [0.0, 1.0]
	// unvalid canal or x values return 0.0
	qreal evalf(int canal, qreal x) const;

signals:
	// emitted if at least one canal changed
	void functionChanged();

protected slots:
	void save();
	void saveAs();
	void load();
	void canalChanged();

protected:
  qreal tfMin;
  qreal tfMax;
	QVector<QtfeCanal*> canals;
	QVector<QtfeOutput*> outputs;
	QString filename;
	QLabel* fileLabel;
};

#endif
