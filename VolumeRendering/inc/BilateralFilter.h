#pragma once
#include "vtkImagingGeneralModule.h" 
#include "vtkImageSpatialAlgorithm.h"
#include <algorithm>

class BilateralFilter : public vtkImageSpatialAlgorithm
{
public:
  static BilateralFilter *New();
  vtkTypeMacro(BilateralFilter, vtkImageSpatialAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  void setKernelSize(int n);
  int GetKernelSize() const { return KernelSize; };
  double GetSigma() const { return Sigma; };
  double * GetGaussKernel() const { return GaussFilter; };

protected:
  BilateralFilter();
  ~BilateralFilter();

  

  void CreateGaussFilter(int elementcount, int halfkernelsize, int kernelSize, BilateralFilter * self);

  void ThreadedRequestData(vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector,
    vtkImageData ***inData, vtkImageData **outData,
    int extent[6], int id);

private:
  double ItlComputeSigma(int _KernelSize);
  double* ItlCreateGaussFilter(int kernelSize, double Sigma);
  inline double gauss(double sigma, int x, int y, int z);

  BilateralFilter(const BilateralFilter&);  // Not implemented.
  void operator=(const BilateralFilter&);  // Not implemented.

  int KernelSize;
  double Sigma;  
  double *GaussFilter;
  int NumberElements;

};

