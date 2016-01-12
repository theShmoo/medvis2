#ifndef GaussianFilter_h
#define GaussianFilter_h


#include "vtkImagingGeneralModule.h" // For export macro
#include "vtkThreadedImageAlgorithm.h"

//! Class for filtering 3D data with a gaussian filter
class GaussianFilter : public vtkThreadedImageAlgorithm
{
public:
  vtkTypeMacro(GaussianFilter,vtkThreadedImageAlgorithm);

  //! Write relevant parameter of this class in the out-console
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Creates an instance of GaussianFilter with the following
  // defaults: Dimensionality 3, StandardDeviations( 2, 2, 2),
  // Radius Factors ( 1.5, 1.5, 1.5)
  //! Creats a new Filterobject and returns it
  static GaussianFilter *New();

  // Description:
  // Set/Get the kernel Size of the filter
  //! Returns the kernelsize (all axis have the same size)
  int GetKernelSize() const { return KernelSize; }

  //! Set the kernelsize to n(all axis have the same size)
  void SetKernelSize(int _KernelSize);
  
  // Description:
  // Set/Get the sigma of the filter
  //! Returns the sigma value (standart deviation) of the gaussian filter
  double GetSigma() const { return Sigma; }

  //! Sets the sigma value (standart deviation) of the gaussian filter
  void SetSigma(double val) { Sigma = val; }

protected:
  //! Constuctor of the gaussian filter
  GaussianFilter();

  //! Destuctor of the bilateral filter
  ~GaussianFilter();

  //! Size of the gaussian filter kernel (all axis have the same size)
  int KernelSize;

  //! The Sigma (standart deviation) of the gauss kernel
  double Sigma;
  
  //! Computes the filter kernel and the weights for the gaussian filter
  void ComputeKernel(double *kernel, int min, int max);

  //! Function used from the vtk pipeline for updating extend
  virtual int RequestUpdateExtent (vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  //! Function used from the vtk pipeline for updating kernelsize
  void InternalRequestUpdateExtent(int *, int*);

  //! Convolutes over a single axis with boundary conditions
  void ExecuteAxis(int axis, vtkImageData *inData, int inExt[6],
                   vtkImageData *outData, int outExt[6],
                   int *pcycle, int target, int *pcount, int total,
                   vtkInformation *inInfo);

  //! The Requestfunction of the gaussian filter called by the vtk-pipeline. This function calls GaussianFilterExecute who does the filteroperation and returns the output  
  void ThreadedRequestData(vtkInformation *request,
                           vtkInformationVector **inputVector,
                           vtkInformationVector *outputVector,
                           vtkImageData ***inData, vtkImageData **outData,
                           int outExt[6], int id);

  //! Computes the sigma value (standart deviation) of the gaussian filter out of the kernelsize and returns it
  double ItlComputeSigma(int _KernelSize);

private:
  GaussianFilter(const GaussianFilter&);  // Not implemented.
  void operator=(const GaussianFilter&);  // Not implemented.
};

#endif










