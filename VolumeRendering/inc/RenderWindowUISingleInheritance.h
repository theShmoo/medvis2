#ifndef RenderWindowUISingleInheritance_H
#define RenderWindowUISingleInheritance_H

#include <QtWidgets\qmainwindow.h>
//#include <QMainWindow>
#include <vtkSmartPointer.h>

// Forward Qt class declarations
class Ui_RenderWindowUISingleInheritance;
class InputParser;
class DataReader;
class vtkSmartVolumeMapper;
class vtkVolume;
class vtkVolumeProperty;
class vtkRenderer;
class vtkRenderWindow;

class RenderWindowUISingleInheritance : public QMainWindow
{
  Q_OBJECT
public:

  // Constructor/Destructor
  RenderWindowUISingleInheritance(InputParser *inputParser);
  ~RenderWindowUISingleInheritance();

  vtkSmartVolumeMapper* createVolumeMapper(InputParser * inputParser, vtkVolume* volume);

  void addTransferFunction(InputParser * inputParser, vtkVolume* volume, vtkSmartVolumeMapper* mapper);

  public slots:

  virtual void slotExit();
  virtual void on_ambient_change(int position);
  virtual void on_specular_change(int position);
  virtual void on_power_change(int position);
  virtual void on_diffuse_change(int position);
  virtual void on_opacity_change(int position);

  virtual void on_transfer_function_change();

private:

  // Designer form
  Ui_RenderWindowUISingleInheritance *ui;
  DataReader *dataReader;
  vtkSmartPointer<vtkVolumeProperty> volumeProp;
  vtkSmartPointer<vtkRenderer> renderer;
  vtkRenderWindow *renWin;
  bool bReady;
};

#endif