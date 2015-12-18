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

#include "QtfeCanal.h"

#include <QPainter>

const int QtfeCanal::pointSizePixel = 5;
const int QtfeCanal::circleSizePixel = 9;
const int QtfeCanal::lineWidth = 2;

QtfeCanal::QtfeCanal() : first(0.0, 0.0), last(1.0, 1.0), selected(NULL), pressed(false), bAllowInteraction(true)
{
	this->setMouseTracking(true);
	
	list.push_back(&first);
	list.push_back(&last);

	background = new QImage(this->size(), QImage::Format_Mono);
	background->fill(1);
}

QtfeCanal::~QtfeCanal()
{
	delete background;
	for(int i=0 ; i<list.size() ; ++i)
	if(list[i]!=&first && list[i]!=&last)
			delete list[i];	
}

const QList<QPointF*>& QtfeCanal::getPoints() const
{
	return list;
}

void QtfeCanal::removeAllPoints()
{
  for (int i = 0; i < list.size(); ++i)
    if (list[i] != &first && list[i] != &last)
      delete list[i];
  list.clear();
}

void QtfeCanal::setFirstPoint(qreal v)
{
	first.setY(qMin(qMax(v,0.0),1.0));
	selected = NULL;
	repaint();
	emit canalChanged(); 
}

void QtfeCanal::setLastPoint(qreal v)
{
	last.setY(qMin(qMax(v,0.0),1.0));
	selected = NULL;
	repaint();
	emit canalChanged(); 
}

void QtfeCanal::insertPoint(const QPointF& p)
{
	QPointF* p_ = new QPointF(qMin(qMax(p.x(),0.0),1.0),qMin(qMax(p.y(),0.0),1.0));
	list.push_back(p_);
	selected = NULL;
	qSort(list.begin(), list.end(), cmp);
	repaint();
	emit canalChanged(); 
}

void QtfeCanal::resizeEvent ( QResizeEvent * event )
{
	delete background;
	background = new QImage(event->size(), QImage::Format_Mono);
	background->fill(1);
}

bool QtfeCanal::cmp(QPointF* p1, QPointF* p2)
{
	return p1->x() <= p2->x();
}

qreal QtfeCanal::interp1(qreal p0, qreal p1, qreal t)
{
	return (1.0f-t)*p0 + t*p1;
}

qreal QtfeCanal::interp2(qreal p0, qreal v0, qreal p1, qreal v1, qreal t)
{
	return t*(t*(t*(2.0f*p0+v0-2.0f*p1+v1)+(-3.0f*p0-2.0f*v0+3.0f*p1-v1))+v0) + p0;
}

QPoint QtfeCanal::listPos2WidgetPos(QPointF pf)
{
	return QPoint(pf.x()*width(), height() * (1.0 - pf.y()));
}

QPointF QtfeCanal::WidgetPos2listPos(QPoint p)
{
	return QPointF(p.x()/(qreal)width(), 1.0 - p.y()/(qreal)height());
}

void QtfeCanal::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	// background
	painter.drawImage(0,0,*background);

	// all points
	QPen pen(Qt::black, pointSizePixel, Qt::SolidLine);
	painter.setPen(pen);
	for(int i=0 ; i<list.size() ; ++i)
	{
	  painter.drawPoint(listPos2WidgetPos(*list[i]));
	}

	// points interpolation line
	pen.setWidth(lineWidth);
	painter.setPen(pen);
	qreal x0 = 0.0;
	qreal y0 = evalf(x0);
	for(int p=1 ; p < list.size() ; ++p)
	for(int i=1 ;  i <= 10  ; ++i)
	{
		qreal x1 = interp1(list[p-1]->x(), list[p]->x(), i/10.0);
		qreal y1 = evalf(x1);
		painter.drawLine(listPos2WidgetPos(QPointF(x0, y0)), listPos2WidgetPos(QPointF(x1, y1)));
		x0 = x1;
		y0 = y1;
	}

	// selected point
	pen.setColor(Qt::red);
	painter.setPen(pen);
	if(selected)
	{
		painter.drawEllipse(listPos2WidgetPos(*selected),circleSizePixel,circleSizePixel);
	}

	QWidget::paintEvent(event);
}

void QtfeCanal::mousePressEvent(QMouseEvent * event)
{
  if (bAllowInteraction)
  {
    QPointF pf = WidgetPos2listPos(event->pos());

    if (event->button() == Qt::LeftButton)
    {
      pressed = true;
      if (!selected)
      {
        for (int i = 1; i < list.size(); ++i)
        {
          if (list[i - 1]->x() < pf.x() && pf.x() < list[i]->x())
          {
            pMin = list[i - 1]->x() + 0.01;
            pMax = list[i]->x() - 0.01;
            break;
          }
        }
        selected = new QPointF(pf);
        list.push_back(selected);
        qSort(list.begin(), list.end(), cmp);
        this->repaint();
        emit canalChanged();
      }
    }
    if (event->button() == Qt::RightButton && selected)
    {
      if (selected != &first && selected != &last)
      {
        list.removeOne(selected);
        delete selected;
        selected = NULL;
        this->repaint();
        emit canalChanged();
      }
    }
  }
}

void QtfeCanal::mouseReleaseEvent ( QMouseEvent * event )
{	
	if(event->button() == Qt::LeftButton)
	{
		pressed = false;
	}
}

void QtfeCanal::mouseMoveEvent ( QMouseEvent * event )
{
  if (bAllowInteraction)
  {
    QPointF pf = WidgetPos2listPos(event->pos());

    if (pressed)
    {
      if (selected)
      {
        if (selected != &first && selected != &last)
        {
          selected->setX(qMin(qMax(pf.x(), pMin), pMax));
        }
        selected->setY(qMin(qMax(pf.y(), 0.0), 1.0));
        this->repaint();
        emit canalChanged();
        return;
      }
    }
    else
    {
      qreal d_min = circleSizePixel*circleSizePixel;
      qreal W = width()*width();
      qreal H = height()*height();
      QPointF* nearest = NULL;

      for (int i = 0; i < list.size(); ++i)
      {
        qreal x = list[i]->x() - pf.x();
        qreal y = list[i]->y() - pf.y();

        qreal d = x*x*W + y*y*H;
        if (d < d_min)
        {
          nearest = list[i];
          d_min = d;
          if (i == 0)
          {
            pMin = 0.0;
            pMax = 0.0;
          }
          else if (i == list.size() - 1)
          {
            pMin = 1.0;
            pMax = 1.0;
          }
          else
          {
            pMin = list[i - 1]->x() + 0.01;
            pMax = list[i + 1]->x() - 0.01;
          }
        }
      }

      if (nearest != selected)
      {
        selected = nearest;
        this->repaint();
      }
    }
  }
}

void QtfeCanal::leaveEvent ( QEvent * event )
{
  if (bAllowInteraction)
  {
    selected = NULL;
    this->repaint();
    QWidget::leaveEvent(event);
  }
	
}

qreal QtfeCanal::evalf(qreal t) const
{
	if(t<=0.0) return first.y();
	if(t>=1.0) return last.y();

	for(int i=0 ; i<list.size()-1 ; ++i)
	{
		qreal x0 = list[i]->x();
		qreal x1 = list[i+1]->x();

		if(t < x0 || t > x1)
			continue;

		qreal y0 = list[i]->y();
		qreal y1 = list[i+1]->y();
		qreal v0 = (i>0) ? (list[i+1]->y() - list[i-1]->y()) : (list[1]->y() - list[0]->y());
		qreal v1 = (i<list.size()-2) ? (list[i+2]->y() - list[i]->y()) : (list[list.size()-1]->y() - list[list.size()-2]->y());

		qreal res = qMin(qMax(interp2(y0, v0, y1, v1, (t-x0)/(x1-x0)), 0.0), 1.0);
		return res;
	 }
	return 0.0;
}