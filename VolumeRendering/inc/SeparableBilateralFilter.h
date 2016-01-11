#ifndef SeparableBilateralFilter_h
#define SeparableBilateralFilter_h


#include "vtkImagingGeneralModule.h" // For export macro
#include "vtkThreadedImageAlgorithm.h"

class SeparableBilateralFilter : public vtkThreadedImageAlgorithm
{
public:
  vtkTypeMacro(SeparableBilateralFilter, vtkThreadedImageAlgorithm);

  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Creates an instance of SeparableBilateralFilter with the following
  // defaults: Dimensionality 3, StandardDeviations( 2, 2, 2),
  // Radius Factors ( 1.5, 1.5, 1.5)
  static SeparableBilateralFilter *New();

  // Description:
  // Set/Get the kernel Size of the filter
  int GetKernelSize() const { return KernelSize; }
  void SetKernelSize(int _KernelSize);

  // Description:
  // Set/Get the sigma of the filter
  double GetSigma() const { return Sigma; }
  void SetSigma(double val) { Sigma = val; }

protected:
  SeparableBilateralFilter();
  ~SeparableBilateralFilter();

  int KernelSize;
  double Sigma;

  void ComputeKernel(double *kernel, int min, int max);
  virtual int RequestUpdateExtent(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  void InternalRequestUpdateExtent(int *, int*);
  void ExecuteAxis(int axis, vtkImageData *inData, int inExt[6],
    vtkImageData *outData, int outExt[6],
    int *pcycle, int target, int *pcount, int total,
    vtkInformation *inInfo);
  void ThreadedRequestData(vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector,
    vtkImageData ***inData, vtkImageData **outData,
    int outExt[6], int id);
  double ItlComputeSigma(int _KernelSize);

private:
  SeparableBilateralFilter(const SeparableBilateralFilter&);  // Not implemented.
  void operator=(const SeparableBilateralFilter&);  // Not implemented.
};

#endif










