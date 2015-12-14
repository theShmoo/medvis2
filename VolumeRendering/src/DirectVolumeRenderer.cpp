/*=========================================================================

  Program:   Visualization Toolkit
  Module:    GPURenderDemo.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// VTK includes
#include "vtkBoxWidget.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkColorTransferFunction.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkImageResample.h"
#include "vtkMetaImageReader.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPlanes.h"
#include "vtkProperty.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "vtkRenderWindowInteractor.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkXMLImageDataReader.h"
#include "vtkSmartVolumeMapper.h"
#include "InputParser.h"
#include "DataReader.h"

// Callback for moving the planes from the box widget to the mapper
class vtkBoxWidgetCallback : public vtkCommand
{
public:
  static vtkBoxWidgetCallback *New()
    { return new vtkBoxWidgetCallback; }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
      if (this->Mapper)
        {
        vtkPlanes *planes = vtkPlanes::New();
        widget->GetPlanes(planes);
        this->Mapper->SetClippingPlanes(planes);
        planes->Delete();
        }
    }
  void SetMapper(vtkSmartVolumeMapper* m)
    { this->Mapper = m; }

protected:
  vtkBoxWidgetCallback()
    { this->Mapper = 0; }

  vtkSmartVolumeMapper *Mapper;
};

int main(int argc, char *argv[])
{
  // Parse the parameters
  InputParser *inputParser = new InputParser(argc, argv);

  // Create the renderer, render window and interactor
  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(renderer);

  // Connect it all. Note that funny arithematic on the
  // SetDesiredUpdateRate - the vtkRenderWindow divides it
  // allocated time across all renderers, and the renderer
  // divides it time across all props. If clip is
  // true then there are two props
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  iren->SetDesiredUpdateRate(inputParser->getFrameRate() / (1 + inputParser->isClippingActive()));

  iren->GetInteractorStyle()->SetDefaultRenderer(renderer);

  DataReader *dataReader = new DataReader();

  // Read the data
  dataReader->readFile((DataReader::TFileType)inputParser->getFileType(), inputParser->getFileName(), inputParser->getDirName());
  dataReader->resampleData(inputParser->getReductionFactor());

  // Create our volume and mapper
  vtkVolume *volume = vtkVolume::New();
  vtkSmartVolumeMapper *mapper = vtkSmartVolumeMapper::New();

  // Add a box widget if the clip option was selected
  vtkBoxWidget *box = vtkBoxWidget::New();
  if (inputParser->isClippingActive())
  {
    box->SetInteractor(iren);
    box->SetPlaceFactor(1.01);
    if (dataReader->getResample())
    {
      box->SetInputConnection(dataReader->getOutputPort());
    }
    else
    {
      box->SetInputData(dataReader->getImageData());
    }

    box->SetDefaultRenderer(renderer);
    box->InsideOutOn();
    box->PlaceWidget();
    vtkBoxWidgetCallback *callback = vtkBoxWidgetCallback::New();
    callback->SetMapper(mapper);
    box->AddObserver(vtkCommand::InteractionEvent, callback);
    callback->Delete();
    box->EnabledOn();
    box->GetSelectedFaceProperty()->SetOpacity(0.0);
  }

  mapper->SetInputConnection(dataReader->getOutputPort());


  // Set the sample distance on the ray to be 1/2 the average spacing
  double spacing[3];
  dataReader->getImageData()->GetSpacing(spacing);

//  mapper->SetSampleDistance( (spacing[0]+spacing[1]+spacing[2])/6.0 );
//  mapper->SetMaximumImageSampleDistance(10.0);


  // Create our transfer function
  vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
  vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();

  // Create the property and attach the transfer functions
  vtkVolumeProperty *property = vtkVolumeProperty::New();
  property->SetIndependentComponents(inputParser->hasIndependentComponents());
  property->SetColor( colorFun );
  property->SetScalarOpacity( opacityFun );
  property->SetInterpolationTypeToLinear();

  // connect up the volume to the property and the mapper
  volume->SetProperty( property );
  volume->SetMapper( mapper );

  // Depending on the blend type selected as a command line option,
  // adjust the transfer function
  double opacityLevel = inputParser->getOpacityLevel();
  double opacityWindow = inputParser->getOpacityWindow();
  switch ( inputParser->getBlendType() )
    {
    // MIP
    // Create an opacity ramp from the window and level values.
    // Color is white. Blending is MIP.
    case 0:
      colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 );
      opacityFun->AddSegment(opacityLevel - 0.5*opacityWindow, 0.0,
                              opacityLevel + 0.5*opacityWindow, 1.0 );
      mapper->SetBlendModeToMaximumIntensity();
      break;

    // CompositeRamp
    // Create a ramp from the window and level values. Use compositing
    // without shading. Color is a ramp from black to white.
    case 1:
      colorFun->AddRGBSegment(opacityLevel - 0.5*opacityWindow, 0.0, 0.0, 0.0,
                              opacityLevel + 0.5*opacityWindow, 1.0, 1.0, 1.0 );
      opacityFun->AddSegment( opacityLevel - 0.5*opacityWindow, 0.0,
                              opacityLevel + 0.5*opacityWindow, 1.0 );
      mapper->SetBlendModeToComposite();
      property->ShadeOff();
      break;

    // CompositeShadeRamp
    // Create a ramp from the window and level values. Use compositing
    // with shading. Color is white.
    case 2:
      colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 );
      opacityFun->AddSegment( opacityLevel - 0.5*opacityWindow, 0.0,
                              opacityLevel + 0.5*opacityWindow, 1.0 );
      mapper->SetBlendModeToComposite();
      property->ShadeOn();
      break;

    // CT_Skin
    // Use compositing and functions set to highlight skin in CT data
    // Not for use on RGB data
    case 3:
      colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
      colorFun->AddRGBPoint( -1000, .62, .36, .18, 0.5, 0.0 );
      colorFun->AddRGBPoint( -500, .88, .60, .29, 0.33, 0.45 );
      colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

      opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-1000, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-500, 1.0, 0.33, 0.45 );
      opacityFun->AddPoint(3071, 1.0, 0.5, 0.0);

      mapper->SetBlendModeToComposite();
      property->ShadeOn();
      property->SetAmbient(0.1);
      property->SetDiffuse(0.9);
      property->SetSpecular(0.2);
      property->SetSpecularPower(10.0);
      property->SetScalarOpacityUnitDistance(0.8919);
      break;

    // CT_Bone
    // Use compositing and functions set to highlight bone in CT data
    // Not for use on RGB data
    case 4:
      colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
      colorFun->AddRGBPoint( -16, 0.73, 0.25, 0.30, 0.49, .61 );
      colorFun->AddRGBPoint( 641, .90, .82, .56, .5, 0.0 );
      colorFun->AddRGBPoint( 3071, 1, 1, 1, .5, 0.0 );

      opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-16, 0, .49, .61 );
      opacityFun->AddPoint(641, .72, .5, 0.0 );
      opacityFun->AddPoint(3071, .71, 0.5, 0.0);

      mapper->SetBlendModeToComposite();
      property->ShadeOn();
      property->SetAmbient(0.1);
      property->SetDiffuse(0.9);
      property->SetSpecular(0.2);
      property->SetSpecularPower(10.0);
      property->SetScalarOpacityUnitDistance(0.8919);
      break;

    // CT_Muscle
    // Use compositing and functions set to highlight muscle in CT data
    // Not for use on RGB data
    case 5:
      colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
      colorFun->AddRGBPoint( -155, .55, .25, .15, 0.5, .92 );
      colorFun->AddRGBPoint( 217, .88, .60, .29, 0.33, 0.45 );
      colorFun->AddRGBPoint( 420, 1, .94, .95, 0.5, 0.0 );
      colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

      opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-155, 0, 0.5, 0.92 );
      opacityFun->AddPoint(217, .68, 0.33, 0.45 );
      opacityFun->AddPoint(420,.83, 0.5, 0.0);
      opacityFun->AddPoint(3071, .80, 0.5, 0.0);

      mapper->SetBlendModeToComposite();
      property->ShadeOn();
      property->SetAmbient(0.1);
      property->SetDiffuse(0.9);
      property->SetSpecular(0.2);
      property->SetSpecularPower(10.0);
      property->SetScalarOpacityUnitDistance(0.8919);
      break;

    // RGB_Composite
    // Use compositing and functions set to highlight red/green/blue regions
    // in RGB data. Not for use on single component data
    case 6:
      opacityFun->AddPoint(0, 0.0);
      opacityFun->AddPoint(5.0, 0.0);
      opacityFun->AddPoint(30.0, 0.05);
      opacityFun->AddPoint(31.0, 0.0);
      opacityFun->AddPoint(90.0, 0.0);
      opacityFun->AddPoint(100.0, 0.3);
      opacityFun->AddPoint(110.0, 0.0);
      opacityFun->AddPoint(190.0, 0.0);
      opacityFun->AddPoint(200.0, 0.4);
      opacityFun->AddPoint(210.0, 0.0);
      opacityFun->AddPoint(245.0, 0.0);
      opacityFun->AddPoint(255.0, 0.5);

      mapper->SetBlendModeToComposite();
      property->ShadeOff();
      property->SetScalarOpacityUnitDistance(1.0);
      break;
    default:
       vtkGenericWarningMacro("Unknown blend type.");
       break;
    }

  // Set the default window size
  renWin->SetSize(600,600);
  renWin->Render();

  // Add the volume to the scene
  renderer->AddVolume( volume );

  renderer->ResetCamera();

  // interact with data
  renWin->Render();

  iren->Start();

  opacityFun->Delete();
  colorFun->Delete();
  property->Delete();

  box->Delete();
  volume->Delete();
  mapper->Delete();
  delete dataReader;
  delete inputParser;
  renderer->Delete();
  renWin->Delete();
  iren->Delete();

  return 0;
}