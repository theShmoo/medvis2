#pragma once
#include "vtkImagingGeneralModule.h" 
#include "vtkImageSpatialAlgorithm.h"
#include <algorithm>

//! Class for filtering 3D data with a median filter
class MedianFilter : public vtkImageSpatialAlgorithm
{
public:
	//! Creats a new Filterobject and returns it
	static MedianFilter *New();

	vtkTypeMacro(MedianFilter, vtkImageSpatialAlgorithm);

	//! Write relevant parameter of this class in the out-console
	void PrintSelf(ostream& os, vtkIndent indent);

	//! Returns the number of elements in the median filter kernel
	vtkGetMacro(NumberOfElements, int);
	
	//! Size of the median filter kernel (all axis have the same size)
	int KernelSize;

	//! Set the kernelsize to n(all axis have the same size)
	void SetKernelSize(int n);

	//! Calculate and return the median of the elements of a given list
	float getMedian(float* list);

protected:
	//! Constuctor of the median filter
	MedianFilter();

	//! Destructor of the median filter
	~MedianFilter();

	//! Number of elements in the filter kernel
	int NumberOfElements;

	//! The Requestfunction of the median filter called by the vtk-pipeline. This function calls MedianFilterExecute who does the filteroperation and returns the output  
	void ThreadedRequestData(vtkInformation *request,
		vtkInformationVector **inputVector,
		vtkInformationVector *outputVector,
		vtkImageData ***inData, vtkImageData **outData,
		int extent[6], int id);

private:

	MedianFilter(const MedianFilter&);  //! Not implemented.
	void operator=(const MedianFilter&);  //! Not implemented.
};

