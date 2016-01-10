#ifndef _Q_TFE_CANAL_
#define _Q_TFE_CANAL_


#include <QtWidgets/qwidget.h>
#include <QMouseEvent>
#include <QList>

//! A canal of the Qtfe this class should not get used outside of the Qtfe widget and therefor it is not documented
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
  void removePointAtPosition(int position);
  bool getAllowInteraction() const { return bAllowInteraction; }
  void setAllowInteraction(bool val) { bAllowInteraction = val; }
  bool IsAllowAddingPoints() const { return bAllowAddingOrRemovingPoints; }
  void SetAllowAddingPoints(bool val) { bAllowAddingOrRemovingPoints = val; }

signals:
  void canalChanged();
  void pointAdded();
  void pointRemoved(int pointNummer);

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
  bool bAllowAddingOrRemovingPoints;

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
