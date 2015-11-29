#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
#include <vtkImageClip.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkShader2.h>
#include <vtkShaderProgram2.h>
#include <vtkProperty.h>
#include <vtkOpenGLProperty.h>
#include <iostream>
#include <fstream>
#include <string>

#include "LoadShader.h"


int main(int argc, char *argv[])
{
	std::string filename;

  if(argc < 2)
  {
	  filename = "./Data/ironProt.vtk";
  }
  else
  {
	  filename = argv[1]; //  "/Data/ironProt.vtk";
  }


  // Create the renderers, render window, and interactor
  vtkSmartPointer<vtkRenderWindow> renWin = 
    vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkRenderWindowInteractor> iren = 
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);
  vtkSmartPointer<vtkRenderer> ren =
    vtkSmartPointer<vtkRenderer>::New();
  renWin->AddRenderer(ren);

  // Read the data from a vtk file
  vtkSmartPointer<vtkStructuredPointsReader> reader = 
    vtkSmartPointer<vtkStructuredPointsReader>::New();
  reader->SetFileName(filename.c_str());
  reader->Update();
 
  // Create a transfer function mapping scalar value to opacity
  vtkSmartPointer<vtkPiecewiseFunction> oTFun = 
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  oTFun->AddSegment(0, 1.0, 256, 0.1);
 
  vtkSmartPointer<vtkColorTransferFunction> cTFun = 
    vtkSmartPointer<vtkColorTransferFunction>::New();
  cTFun->AddRGBPoint(   0, 1.0, 1.0, 1.0 );
  cTFun->AddRGBPoint( 255, 1.0, 1.0, 1.0 );

  // Need to crop to actually see minimum intensity
  vtkSmartPointer<vtkImageClip> clip = 
    vtkSmartPointer<vtkImageClip>::New();
  clip->SetInputConnection( reader->GetOutputPort() );
  clip->SetOutputWholeExtent(0,66,0,66,30,37);
  clip->ClipDataOn();
 
  //-.-.-.-.-.-.-.-.-.-.-.-.-.-Baustelle fuers DVR-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
  /*
  std::string filenameDVRvertexshader = "./Shader/VolumeRenderer.vert";
  std::string filenameDVRgeometryshader = "./Shader/VolumeRenderer.geo";
  std::string filenameDVRfragmentshader = "./Shader/VolumeRenderer.frag";

  vtkSmartPointer<vtkShaderProgram2> program = LoadShader::loadShaders(filenameDVRvertexshader, filenameDVRgeometryshader, filenameDVRfragmentshader, renWin, true);

  if (program == nullptr)
	  std::cout << "failed to load Program" << std::endl;
  
  float camPosFloat[3];
  double* camPos = ren->GetActiveCamera()->GetPosition();
  std::copy(camPos, camPos + 3, camPosFloat);
  
  float screenSizeFloat[3];
  int* screenSize = renWin->GetScreenSize();
  std::copy(screenSize, screenSize + 3, screenSizeFloat);

  program->SetUniform2f("WindowSize", screenSizeFloat);
  program->SetUniform3f("RayOrigin", camPosFloat);

  */
  //-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-



  vtkSmartPointer<vtkVolumeProperty> property = 
    vtkSmartPointer<vtkVolumeProperty>::New();
  property->SetScalarOpacity(oTFun);
  property->SetColor(cTFun);
  property->SetInterpolationTypeToLinear();
 
  vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> mapper = 
    vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
  mapper->SetBlendModeToMinimumIntensity();
  mapper->SetInputConnection( clip->GetOutputPort() );
 
  vtkSmartPointer<vtkVolume> volume = 
    vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(mapper);
  volume->SetProperty(property);
 

  ren->AddViewProp(volume);
  renWin->Render();
  iren->Start();
 
  return EXIT_SUCCESS;
}

// Vorlage
/*
vtkSmartPointer<vtkConeSource> cone = vtkConeSource::New();
vtkSmartPointer<vtkPolyDataMapper> coneMapper = vtkPolyDataMapper::New();
coneMapper->SetInputConnection(cone->GetOutputPort());
vtkSmartPointer<vtkActor> coneActor = vtkActor::New();
coneActor->SetMapper(coneMapper);
vtkSmartPointer<vtkRenderer> ren = vtkRenderer::New();
ren->AddActor(coneActor);

vtkSmartPointer<vtkRenderWindow> renWin = vtkRenderWindow::New();
renWin->AddRenderer(ren);
const char* frag = "void propFuncFS(void){ gl_FragColor = vec4(255,0,0,1);}";

vtkSmartPointer<vtkShaderProgram2> pgm = vtkShaderProgram2::New();
pgm->SetContext(renWin);

vtkSmartPointer<vtkShader2> shader = vtkShader2::New();
shader->SetType(VTK_SHADER_TYPE_FRAGMENT);
shader->SetSourceCode(frag);
shader->SetContext(pgm->GetContext());

pgm->GetShaders()->AddItem(shader);
vtkSmartPointer<vtkOpenGLProperty> openGLproperty =
static_cast<vtkOpenGLProperty*>(coneActor->GetProperty());
openGLproperty->SetPropProgram(pgm);
openGLproperty->ShadingOn();
int i;
for (i = 0; i < 360; ++i)
{
renWin->Render();
ren->GetActiveCamera()->Azimuth(1);
}*/