#pragma once
#include "vtkImagingGeneralModule.h" 
#include "vtkImageSpatialAlgorithm.h"
#include <algorithm>

class MedianFilter : public vtkImageSpatialAlgorithm
{
public:
	static MedianFilter *New();
	vtkTypeMacro(MedianFilter, vtkImageSpatialAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Return the number of elements in the median mask
	vtkGetMacro(NumberOfElements, int);
	
  int KernelSize;

	void setKernelSize(int n);

  float getMedian(float* list);

protected:
	MedianFilter();
	~MedianFilter();

	int NumberOfElements;

	void ThreadedRequestData(vtkInformation *request,
		vtkInformationVector **inputVector,
		vtkInformationVector *outputVector,
		vtkImageData ***inData, vtkImageData **outData,
		int extent[6], int id);

private:

	MedianFilter(const MedianFilter&);  // Not implemented.
	void operator=(const MedianFilter&);  // Not implemented.
};

