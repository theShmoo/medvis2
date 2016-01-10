#ifndef _Q_TFE_
#define _Q_TFE_

#include <QtWidgets/qwidget.h>
#include <QVector>
class QLabel;

#include "QtfeCanal.h"
#include "QtfeOutput.h"

//! A QWidget to change a transfer function
class Qtfe : public QWidget 
{
	Q_OBJECT

public:
  //! Constructor
	Qtfe(QWidget * parent = 0, Qt::WindowFlags f = 0);
  //! Deconstructor
	~Qtfe();

	//! add a canal
	void addCanal(int n, QString name);
	//! add an RGBA output
	void addOutputs(int n);
	//! bind a canal to the Red output (-1 to disable the canal)
	void bindCanaltoOutputR(int canal, int output);
  //! bind a canal to the Green output (-1 to disable the canal)
	void bindCanaltoOutputG(int canal, int output);
  //! bind a canal to the Blue output (-1 to disable the canal)
	void bindCanaltoOutputB(int canal, int output);
  //! bind a canal to the Alpha output (-1 to disable the canal)
	void bindCanaltoOutputA(int canal, int output);
	//! transfer function dimension (canal number)
	int dim() const;
  //! Set the source mapping area of the transfer function
  void setSource(qreal dMin, qreal dMax);
  //! Remove all points from the transfer function
  void removeAllRGBPoints();
  //! Add a new rgb point to the transfer function
  void addRGBAPoint(qreal x, qreal r, qreal g, qreal b, qreal a);
  //! adjust all x of the rgb points by the x of the points of the alpha canal
  void changeXofRGBPoints();
  //! Returns all values of all points (x,r,g,b,a)
  void getAllRGBAPoints(qreal* x, qreal* r, qreal* g, qreal* b, qreal* a);
  //! Returns the number of points
  uint getNumPoints();

	/*! evaluate canal at x in [0.0, 1.0]
	 invalid canal or x values return 0.0 */
	qreal evalf(int canal, qreal x) const;

signals:
	//! emitted if at least one canal changed
	void functionChanged();

protected slots:
  //! emitted if the user presses "save"
	void save();
  //! emitted if the user presses "save as"
	void saveAs();
  //! emitted if the user presses "load"
	void load();
  //! emitted if a canal changes
  void canalChanged();
  //! emitted if the user adds a point to a canal
  void pointAdded();
  //! emitted if the user removes a point
  void pointRemoved(int position);

protected:
  qreal tfMin;                  //! The source mapping minimum of the transfer function
  qreal tfMax;                  //! The source mapping minimum of the transfer function
	QVector<QtfeCanal*> canals;   //! All canals of the transfer function (usually: rgba)
  QtfeCanal* alphacanal;        //! The alpha canals of the transfer function
	QVector<QtfeOutput*> outputs; //! All outputs of the transfer function (usually: rgba)
	QString filename;             //! The name of the file where the transfer function xml file gets saved
	QLabel* fileLabel;            //! the QLabel of the filename
};

#endif
