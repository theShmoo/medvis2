#ifndef SeparableBilateralFilter_h
#define SeparableBilateralFilter_h


#include "vtkImagingGeneralModule.h" // For export macro
#include "vtkThreadedImageAlgorithm.h"

//! Class for filtering 3D data with a bilateral filter (speeded up, according to the separable property of this filter type)
class SeparableBilateralFilter : public vtkThreadedImageAlgorithm
{
public:
  vtkTypeMacro(SeparableBilateralFilter, vtkThreadedImageAlgorithm);

  //! Write relevant parameter of this class in the out-console
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Creates an instance of SeparableBilateralFilter with the following
  // defaults: Dimensionality 3, StandardDeviations( 2, 2, 2),
  // Radius Factors ( 1.5, 1.5, 1.5)
  // ! Creats a new Filterobject and returns it
  static SeparableBilateralFilter *New();

  // Description:
  // Set/Get the kernel Size of the filter
  //! Returns the size of the filter kernel
  int GetKernelSize() const { return KernelSize; }

  //! Size of the bilateral filter kernel (all axis have the same size)
  void SetKernelSize(int _KernelSize);

  // Description:
  // Set/Get the sigma of the filter
  //! returns the sigma value (standart deviation) of the bilateral filter
  double GetSigma() const { return Sigma; }

  //! Sets the sigma value (standart deviation) of the bilateral filter
  void SetSigma(double val) { Sigma = val; }

protected:
  //! Constuctor of the bilateral filter
  SeparableBilateralFilter();

  //! Destuctor of the bilateral filter
  ~SeparableBilateralFilter();

  //! Size of the bilateral filter kernel (all axis have the same size)
  int KernelSize;

  //! The Sigma (standart deviation) of the gauss kernel
  double Sigma;

  //! Creates the filter kernel with weights using the given kernel size
  void ComputeKernel(double *kernel, int min, int max);

  //! Function used from the vtk pipeline for updating extend
  virtual int RequestUpdateExtent(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  //! Function used from the vtk pipeline for updating kernelsize
  void InternalRequestUpdateExtent(int *, int*);

  //! Convolutes over a single axis with boundary conditions
  void ExecuteAxis(int axis, vtkImageData *inData, int inExt[6],
    vtkImageData *outData, int outExt[6],
    int *pcycle, int target, int *pcount, int total,
    vtkInformation *inInfo);

  //! The Requestfunction of the bilateral filter called by the vtk-pipeline. This function calls BilateralFilterExecute who does the filteroperation and returns the output  
  void ThreadedRequestData(vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector,
    vtkImageData ***inData, vtkImageData **outData,
    int outExt[6], int id);

  //! Computes the sigma value (standart deviation) of the bilateral filter out of the kernelsize and returns it
  double ItlComputeSigma(int _KernelSize);

private:
  SeparableBilateralFilter(const SeparableBilateralFilter&);  // Not implemented.
  void operator=(const SeparableBilateralFilter&);  // Not implemented.
};

#endif










