#ifndef SeparableGradientFilter_h
#define SeparableGradientFilter_h

#include "vtkImageSpatialAlgorithm.h"

class SeparableGradientFilter : public vtkImageSpatialAlgorithm
{
public:
  static SeparableGradientFilter *New();
  vtkTypeMacro(SeparableGradientFilter, vtkImageSpatialAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  SeparableGradientFilter();
  ~SeparableGradientFilter() {}

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
