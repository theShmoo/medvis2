#ifndef SeparableGradientFilter_h
#define SeparableGradientFilter_h

#include "vtkImageSpatialAlgorithm.h"

//! Class for filtering 3D data with a gradient filter (speeded up, according to the separable property of this filter type)
class SeparableGradientFilter : public vtkImageSpatialAlgorithm
{
public:
	// ! Creats a new Filterobject and returns it
  static SeparableGradientFilter *New();

  vtkTypeMacro(SeparableGradientFilter, vtkImageSpatialAlgorithm);

  //! Write relevant parameter of this class in the out-console
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
	//! Constuctor of the gradient filter
  SeparableGradientFilter();

  //! Destuctor of the gradient filter
  ~SeparableGradientFilter() {}

  //! The Requestfunction of the gradient filter called by the vtk-pipeline. This function calls GradientFilterExecute who does the filteroperation and returns the output  
  void ThreadedRequestData(vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector,
    vtkImageData ***inData, vtkImageData **outData,
    int outExt[6], int id);

private:
  SeparableGradientFilter(const SeparableGradientFilter&);  // Not implemented.
  void operator=(const SeparableGradientFilter&);  // Not implemented.
};

#endif
