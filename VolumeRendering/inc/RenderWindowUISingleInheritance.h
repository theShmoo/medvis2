#ifndef RenderWindowUISingleInheritance_H
#define RenderWindowUISingleInheritance_H

#include <QtWidgets\qmainwindow.h>
//#include <QMainWindow>
#include <vtkSmartPointer.h>

// Forward Qt class declarations
class Ui_RenderWindowUISingleInheritance;
class InputParser;
class DataReader;
class GradientPostprocessFilter;
class vtkVolumeMapper;
class vtkVolume;
class vtkVolumeProperty;
class vtkRenderer;
class vtkOpenGLRenderer;
class vtkRenderWindow;
class vtkImageAlgorithm;

//! Class for rendering a volume data set to a VTKWidget which is in a QT Window
class RenderWindowUISingleInheritance : public QMainWindow
{
  Q_OBJECT
public:

  //! Constructor of the class which gets the input parser which contains the input of the user via CLI
  RenderWindowUISingleInheritance(InputParser *inputParser);

  //! Destructor of the class
  ~RenderWindowUISingleInheritance();

  //! creates the volume mapper and connects the volume to the input port of the mapper
  vtkVolumeMapper* createVolumeMapper(vtkVolume* volume);

  //! Add the transfer function to the volume which can be specified by the user input via CLI
  void addTransferFunction(InputParser * inputParser, vtkVolume* volume, vtkVolumeMapper* mapper);

  public slots:

  //! called when the window closes
  virtual void slotExit();

  //! called when the ambient slider changes its position
  virtual void on_ambient_change(int position);
  //! called when the specular slider changes its position
  virtual void on_specular_change(int position);
  //! called when the specular power slider changes its position
  virtual void on_power_change(int position);
  //! called when the diffuse slider changes its position
  virtual void on_diffuse_change(int position);
  //! called when the opacity slider changes its position
  virtual void on_opacity_change(int position);
  //! called when the transfer function changes. For example the position of one point.
  virtual void on_transfer_function_change();
  //! called when the filter of the volume changes.
  virtual void on_filter_changed();

private:

  //! Designer form
  Ui_RenderWindowUISingleInheritance *ui;
  //! reads data for example of DICOM files
  DataReader *dataReader;
  //! the properties of the volume (e.g.: ambient)
  vtkSmartPointer<vtkVolumeProperty> volumeProp;
  //! the open gl renderer
  vtkSmartPointer<vtkOpenGLRenderer> renderer;
  //! The post process filter
  GradientPostprocessFilter * postprocessFilter;
  //! the currently active filter. if no filter is active this is null
  vtkImageAlgorithm* filter;
  //! the volume mapper
  vtkSmartPointer<vtkVolumeMapper> mapper;
  //! The render window
  vtkRenderWindow *renWin;
  //! After initialization this variable gets set to true so that the user is able to interact with the DVR
  bool bReady;
};

#endif