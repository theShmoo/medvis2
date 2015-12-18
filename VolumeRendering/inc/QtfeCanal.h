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

#ifndef _Q_TFE_CANAL_
#define _Q_TFE_CANAL_


#include <QWidget>
#include <QMouseEvent>
#include <QList>

// Warning : QtfeCanal is not supposed to be used outside Qtfe
class QtfeCanal : public QWidget 
{
	Q_OBJECT

	friend class Qtfe;

private:
	QtfeCanal();
	~QtfeCanal();

	qreal evalf(qreal x) const;
	const QList<QPointF*>& getPoints() const;
	void setFirstPoint(qreal y);
	void setLastPoint(qreal y);
	void insertPoint(const QPointF& p);
  void removeAllPoints();
  bool getAllowInteraction() const { return bAllowInteraction; }
  void setAllowInteraction(bool val) { bAllowInteraction = val; }

signals:
	void canalChanged();

private:
	virtual void paintEvent(QPaintEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void leaveEvent(QEvent* event);
	virtual void resizeEvent(QResizeEvent* event);

	// curve
	QPointF first;
	QPointF last;
	QList<QPointF*> list;

	// mouse
	bool pressed;
	QPointF* selected;
	qreal pMin, pMax;
  bool bAllowInteraction;
	// paint
	QImage * background;
	QPoint listPos2WidgetPos(QPointF pf);
	QPointF WidgetPos2listPos(QPoint p);

private:
	static qreal interp1(qreal p0, qreal p1, qreal t);
	static qreal interp2(qreal p0, qreal v0, qreal p1, qreal v1, qreal t);
	static bool cmp(QPointF* p1, QPointF* p2);
	static const int pointSizePixel;
	static const int circleSizePixel;
	static const int lineWidth;
};
 

#endif
