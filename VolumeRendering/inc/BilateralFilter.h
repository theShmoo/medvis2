#pragma once
#include "vtkImagingGeneralModule.h" 
#include "vtkImageSpatialAlgorithm.h"
#include <algorithm>

//! Class for filtering 3D data with a bilateral filter
class BilateralFilter : public vtkImageSpatialAlgorithm
{
public:
	//! Creats a new Filterobject and returns it
  static BilateralFilter *New();


  vtkTypeMacro(BilateralFilter, vtkImageSpatialAlgorithm);

  //! Write relevant parameter of this class in the outout-console
  void PrintSelf(ostream& os, vtkIndent indent);

  //! Size of the bilateral filter kernel (all axis have the same size)
  void setKernelSize(int n);

  //! Returns the size of the filter kernel
  int GetKernelSize() const { return KernelSize; };

  //! returns the sigma value (standart deviation) of the gaussian filter
  double GetSigma() const { return Sigma; };

  //! Returns the actual values of the filter kernel
  double * GetGaussKernel() const { return GaussFilter; };

protected:
  //! Constuctor of the bilateral filter
  BilateralFilter();

  //! Destuctor of the bilateral filter
  ~BilateralFilter();

  //void CreateGaussFilter(int elementcount, int halfkernelsize, int kernelSize, BilateralFilter * self);

  //! The Requestfunction of the bilateral filter called by the vtk-pipeline. This function calls BilateralFilterExecute who does the filteroperation and returns the output  
  void ThreadedRequestData(vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector,
    vtkImageData ***inData, vtkImageData **outData,
    int extent[6], int id);

private:
  //! Computes the sigma value (standart deviation) of the bilateral filter out of the kernelsize and returns it
  double ItlComputeSigma(int _KernelSize);

  //! Creates the filter kernel with weights using the given kernel size and sigma (standart deviation) and returns it as an array
  double* ItlCreateGaussFilter(int kernelSize, double Sigma);

  //! Compute the value of the 3D Gaussian function equation and returns it
  inline double gauss(double sigma, int x, int y, int z);

  BilateralFilter(const BilateralFilter&);  // Not implemented.
  void operator=(const BilateralFilter&);  // Not implemented.

  //! Size of the bilateral filter kernel (all axis have the same size)
  int KernelSize;

  //! The Sigma (standart deviation) of the gauss kernel
  double Sigma;  

  //! The current values of the filter kernel
  double *GaussFilter;

  //int NumberElements;

};

