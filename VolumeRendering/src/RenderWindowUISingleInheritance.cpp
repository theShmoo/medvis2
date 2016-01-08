// This is included here because it is forward declared in
#include "RenderWindowUISingleInheritance.h"
#include "ui_RenderWindowUISingleInheritance.h"
#include "InputParser.h"
#include "DataReader.h"
#include "Qtfe.h"

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
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkXMLImageDataReader.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkSmartPointer.h"
#include "vtkOpenGLGPUMultiVolumeRayCastMapper.h"
#include "vtkImageAnisotropicDiffusion3D.h"
#include "vtkImageMedian3D.h"
#include "vtkImageGaussianSmooth.h"
#include <assert.h>
#include "GaussianFilter.h"

//#include "vtkGDCMImageReader/vtkGDCMImageWriter"


// Constructor
RenderWindowUISingleInheritance::RenderWindowUISingleInheritance(InputParser *inputParser)
{
  bReady = false;
  // create data reader and the user interface
  this->dataReader = new DataReader();

  filter = nullptr;

  this->ui = new Ui_RenderWindowUISingleInheritance;
  this->ui->setupUi(this);

  // Create the renderer
  renderer = vtkSmartPointer<vtkRenderer>::New();

  // VTK/Qt wedded
  renWin = this->ui->qvtkWidget->GetRenderWindow();
  renWin->AddRenderer(renderer);

  // Set up action signals and slots
  connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
  connect(this->ui->ambientSlider, SIGNAL(valueChanged(int)), this, SLOT(on_ambient_change(int)));
  connect(this->ui->diffuseSlider, SIGNAL(valueChanged(int)), this, SLOT(on_diffuse_change(int)));
  connect(this->ui->specSlider, SIGNAL(valueChanged(int)), this, SLOT(on_specular_change(int)));
  connect(this->ui->powerSlider, SIGNAL(valueChanged(int)), this, SLOT(on_power_change(int)));
  connect(this->ui->opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(on_opacity_change(int)));
  connect(this->ui->qtfe, SIGNAL(functionChanged()), this, SLOT(on_transfer_function_change()));
  connect(this->ui->changeFilter, SIGNAL(clicked()), this, SLOT(on_filter_changed()));

  // Read the data
  this->dataReader->readFile((DataReader::TFileType)inputParser->getFileType(), inputParser->getFileName(), inputParser->getDirName());
  this->dataReader->resampleData(inputParser->getReductionFactor());

  // Create our volume
  vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New(); 

  // create out mapper
  vtkOpenGLGPUMultiVolumeRayCastMapper* mapper = static_cast<vtkOpenGLGPUMultiVolumeRayCastMapper*>(createVolumeMapper(inputParser, volume));
  mapper->setNumberOfAdditionalVolumes(0);
  mapper->SetBlendModeToComposite();

  // Add the transfer function to the volume
  addTransferFunction(inputParser, volume, mapper);

  // Set the default window size
  renWin->SetSize(600, 600);

  // Add the volume to the scene
  renderer->AddVolume(volume);

  // init camera
  renderer->ResetCamera();

  // interact with data
  renWin->Render();

  bReady = true;
}

RenderWindowUISingleInheritance::~RenderWindowUISingleInheritance()
{
  filter->Delete();
  delete dataReader;
  delete ui;
}

void RenderWindowUISingleInheritance::slotExit()
{
  qApp->exit();
}

void RenderWindowUISingleInheritance::on_ambient_change(int position)
{
  if (bReady)
  {
    float fPosition = position / 10.0f;
    printf("ambient: %f\n", fPosition);
    volumeProp->SetAmbient(fPosition);
    renWin->Render();
  }
}

void RenderWindowUISingleInheritance::on_specular_change(int position)
{
  if (bReady)
  {
    float fPosition = position / 10.0f;
    printf("specular: %f\n", fPosition);
    volumeProp->SetSpecular(fPosition);
    renWin->Render();
  }
}

void RenderWindowUISingleInheritance::on_power_change(int position)
{
  if (bReady)
  {
    float fPosition = position / 10.0f;
    printf("power: %f\n", fPosition);
    volumeProp->SetSpecularPower(fPosition);
    renWin->Render();
  }
}

void RenderWindowUISingleInheritance::on_diffuse_change(int position)
{
  if (bReady)
  {
    float fPosition = position / 10.0f;
    printf("diffuse: %f\n", fPosition);
    volumeProp->SetDiffuse(fPosition);    
    renWin->Render();
  }
}

void RenderWindowUISingleInheritance::on_opacity_change(int position)
{
  if (bReady)
  {
    float fPosition = position;
    printf("opacity: %f\n", fPosition);
    volumeProp->SetScalarOpacityUnitDistance(fPosition);
    volumeProp->GetScalarOpacity()->Modified();
    renWin->Render();
  }
}

void RenderWindowUISingleInheritance::on_transfer_function_change()
{
  if (bReady)
  {
    vtkColorTransferFunction* ctf = volumeProp->GetRGBTransferFunction();
    vtkPiecewiseFunction* otf = volumeProp->GetScalarOpacity();
    ctf->RemoveAllPoints();
    otf->RemoveAllPoints();
    uint nNumPoints = this->ui->qtfe->getNumPoints();
    qreal *x = new qreal[nNumPoints];
    qreal *r = new qreal[nNumPoints];
    qreal *g = new qreal[nNumPoints];
    qreal *b = new qreal[nNumPoints];
    qreal *a = new qreal[nNumPoints];

    this->ui->qtfe->getAllRGBAPoints(x, r, g, b, a);
    this->ui->qtfe->changeXofRGBPoints();

    for (uint i = 0; i < nNumPoints; i++)
    {
      ctf->AddRGBPoint(x[i], r[i], g[i], b[i]);
      otf->AddPoint(x[i], a[i]);
    }
    delete x, r, g, b, a;
    ctf->Modified();
    otf->Modified();
    renWin->Render();
  }
}

void RenderWindowUISingleInheritance::on_filter_changed()
{
  if (filter)
  {
    mapper->SetInputConnection(dataReader->GetOutputPort());
    filter->Delete();
    filter = nullptr;
  }

  int i = this->ui->cbFilter->currentIndex();
  int kernelSize = this->ui->sbKernelSize->value();
  
  switch (i)
  {
  case 0: // no filter
    // nothing to do
    break;
  case 1: // Bilateral Filter
    filter = vtkImageAnisotropicDiffusion3D::New();
    break;
  case 2: // Gauss Filter
  {
    filter = GaussianFilter::New();
    GaussianFilter* pFilter = GaussianFilter::SafeDownCast(filter);
    pFilter->SetKernelSize(kernelSize);
    break;
  }
  case 3: // Median Filter
  {
    filter = vtkImageMedian3D::New();
    vtkImageMedian3D* pFilter = vtkImageMedian3D::SafeDownCast(filter);
    pFilter->SetKernelSize(kernelSize, kernelSize, kernelSize);
    break;
  }
  default:
    assert(!"Filter not supported!");
  }

  if (i > 0)
  {
    filter->SetInputConnection(dataReader->GetOutputPort());
    filter->Update();
    mapper->SetInputConnection(filter->GetOutputPort());
  }
  volumeProp->GetRGBTransferFunction()->Modified();
  volumeProp->GetScalarOpacity()->Modified();
  renWin->Render();
}

void RenderWindowUISingleInheritance::addTransferFunction(InputParser * inputParser, vtkVolume* volume, vtkVolumeMapper* mapper)
{
  // Create our transfer function
  vtkSmartPointer<vtkColorTransferFunction> colorFun = vtkSmartPointer<vtkColorTransferFunction>::New();
  vtkSmartPointer<vtkPiecewiseFunction> opacityFun = vtkSmartPointer<vtkPiecewiseFunction>::New();

  // Create the property and attach the transfer functions
  volumeProp = vtkSmartPointer<vtkVolumeProperty>::New();
  volumeProp->SetIndependentComponents(inputParser->hasIndependentComponents());
  volumeProp->SetColor(colorFun);
  volumeProp->SetScalarOpacity(opacityFun);
  volumeProp->SetInterpolationTypeToLinear();

  // connect up the volume to the property and the mapper
  volume->SetProperty(volumeProp);

  Qtfe * qtfe = this->ui->qtfe;

  // Depending on the blend type selected as a command line option,
  // adjust the transfer function
  double opacityLevel = inputParser->getOpacityLevel();
  double opacityWindow = inputParser->getOpacityWindow();
  switch (inputParser->getBlendType())
  {
    // MIP
    // Create an opacity ramp from the window and level values.
    // Color is white. Blending is MIP.
  case 0:
    colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0);
    opacityFun->AddSegment(opacityLevel - 0.5*opacityWindow, 0.0,
      opacityLevel + 0.5*opacityWindow, 1.0);
    mapper->SetBlendModeToMaximumIntensity();
    break;

    // CompositeRamp
    // Create a ramp from the window and level values. Use compositing
    // without shading. Color is a ramp from black to white.
  case 1:
    colorFun->AddRGBSegment(opacityLevel - 0.5*opacityWindow, 0.0, 0.0, 0.0,
      opacityLevel + 0.5*opacityWindow, 1.0, 1.0, 1.0);
    opacityFun->AddSegment(opacityLevel - 0.5*opacityWindow, 0.0,
      opacityLevel + 0.5*opacityWindow, 1.0);
    mapper->SetBlendModeToComposite();
    volumeProp->ShadeOff();
    break;

    // CompositeShadeRamp
    // Create a ramp from the window and level values. Use compositing
    // with shading. Color is white.
  case 2:
    colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0);
    opacityFun->AddSegment(opacityLevel - 0.5*opacityWindow, 0.0,
      opacityLevel + 0.5*opacityWindow, 1.0);
    mapper->SetBlendModeToComposite();
    volumeProp->ShadeOn();
    break;

    // CT_Skin
    // Use compositing and functions set to highlight skin in CT data
    // Not for use on RGB data
  case 3:
    colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
    colorFun->AddRGBPoint(-1000, .62, .36, .18, 0.5, 0.0);
    colorFun->AddRGBPoint(-500, .88, .60, .29, 0.33, 0.45);
    colorFun->AddRGBPoint(3071, .83, .66, 1, 0.5, 0.0);

    opacityFun->AddPoint(-3024, 0, 0.5, 0.0);
    opacityFun->AddPoint(-1000, 0, 0.5, 0.0);
    opacityFun->AddPoint(-500, 1.0, 0.33, 0.45);
    opacityFun->AddPoint(3071, 1.0, 0.5, 0.0);

    mapper->SetBlendModeToComposite();
    volumeProp->ShadeOn();
    volumeProp->SetAmbient(0.1);
    volumeProp->SetDiffuse(0.9);
    volumeProp->SetSpecular(0.2);
    volumeProp->SetSpecularPower(10.0);
    volumeProp->SetScalarOpacityUnitDistance(0.8919);
    break;

    // CT_Bone
    // Use compositing and functions set to highlight bone in CT data
    // Not for use on RGB data
  case 4:
    qtfe->removeAllRGBPoints();
    qtfe->setSource(-3024, 3071);
    qtfe->addRGBAPoint(-3024, 0, 0, 0, 0);
    qtfe->addRGBAPoint(-16, 0.73, 0.25, 0.30, 0);
    qtfe->addRGBAPoint(641, .90, .82, .56, .72);
    qtfe->addRGBAPoint(3071, 1, 1, 1, .71);

    colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
    colorFun->AddRGBPoint(-16, 0.73, 0.25, 0.30, 0.49, .61);
    colorFun->AddRGBPoint(641, .90, .82, .56, .5, 0.0);
    colorFun->AddRGBPoint(3071, 1, 1, 1, .5, 0.0);

    opacityFun->AddPoint(-3024, 0, 0.5, 0.0);
    opacityFun->AddPoint(-16, 0, .49, .61);
    opacityFun->AddPoint(641, .72, .5, 0.0);
    opacityFun->AddPoint(3071, .71, 0.5, 0.0);

    mapper->SetBlendModeToComposite();
    volumeProp->ShadeOn();
    volumeProp->SetAmbient(0.1);

    this->ui->ambientSlider->setValue(1);

    volumeProp->SetDiffuse(0.9);

    this->ui->diffuseSlider->setValue(9);

    volumeProp->SetSpecular(0.2);

    this->ui->specSlider->setValue(2);

    volumeProp->SetSpecularPower(10.0);

    this->ui->specSlider->setValue(10);

    volumeProp->SetScalarOpacityUnitDistance(0.8919);

    this->ui->specSlider->setValue(9);

    break;

    // CT_Muscle
    // Use compositing and functions set to highlight muscle in CT data
    // Not for use on RGB data
  case 5:
    colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
    colorFun->AddRGBPoint(-155, .55, .25, .15, 0.5, .92);
    colorFun->AddRGBPoint(217, .88, .60, .29, 0.33, 0.45);
    colorFun->AddRGBPoint(420, 1, .94, .95, 0.5, 0.0);
    colorFun->AddRGBPoint(3071, .83, .66, 1, 0.5, 0.0);

    opacityFun->AddPoint(-3024, 0, 0.5, 0.0);
    opacityFun->AddPoint(-155, 0, 0.5, 0.92);
    opacityFun->AddPoint(217, .68, 0.33, 0.45);
    opacityFun->AddPoint(420, .83, 0.5, 0.0);
    opacityFun->AddPoint(3071, .80, 0.5, 0.0);

    mapper->SetBlendModeToComposite();
    volumeProp->ShadeOn();
    volumeProp->SetAmbient(0.1);
    volumeProp->SetDiffuse(0.9);
    volumeProp->SetSpecular(0.2);
    volumeProp->SetSpecularPower(10.0);
    volumeProp->SetScalarOpacityUnitDistance(0.8919);
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
    volumeProp->ShadeOff();
    volumeProp->SetScalarOpacityUnitDistance(1.0);
    break;
  default:
    vtkGenericWarningMacro("Unknown blend type.");
    break;
  }
}

vtkVolumeMapper* RenderWindowUISingleInheritance::createVolumeMapper(InputParser * inputParser, vtkVolume* volume)
{

  mapper = vtkSmartPointer<vtkOpenGLGPUMultiVolumeRayCastMapper>::New();
  mapper->SetInputConnection(dataReader->GetOutputPort());

  // Set the sample distance on the ray to be 1/2 the average spacing
//  double spacing[3];
//  this->dataReader->getImageData()->GetSpacing(spacing);

 // mapper->SetSampleDistance((spacing[0] + spacing[1] + spacing[2]) / 12.0);

  volume->SetMapper(mapper);

  return mapper;
}
