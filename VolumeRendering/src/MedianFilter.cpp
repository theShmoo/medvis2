#include "MedianFilter.h"

#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"

vtkStandardNewMacro(MedianFilter);

MedianFilter::MedianFilter()
{
	this->KernelSize = 3;
	this->KernelMiddle = 1;

	this->NumberOfElements = this->KernelSize ^ 3;
}


MedianFilter::~MedianFilter()
{
}

void MedianFilter::setKernelSize(int n)
{
	if (n % n != 0)
		return;

	this->KernelSize = n;
	this->NumberOfElements = this->KernelSize * this->KernelSize * this->KernelSize;
}
void MedianFilter::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

	os << indent << "Kernel Size: " << this->KernelSize << "\n";
}


template <class T>
void MedianFilterExecute(MedianFilter *self,
	vtkImageData *inData, T *inPtr,
	vtkImageData *outData, T *outPtr,
	int outExt[6], int id,
	vtkDataArray *inArray)
{
	
	int* dimensions = inData->GetDimensions();
	int halfkernelsize = (self->KernelSize - 1) / 2;
	int index = 0;
	int elementcount = self->KernelSize * self->KernelSize * self->KernelSize;
	float* kernelvalues = new float[elementcount];

	for (int z = 0; z < dimensions[2]; z++)
	{
		for (int y = 0; y < dimensions[1]; y++)
		{
			for (int x = 0; x < dimensions[0]; x++)
			{

				index = 0;
				for (int kernelz = -1 * halfkernelsize; kernelz <= halfkernelsize; kernelz++)
				{
					for (int kernely = -1 * halfkernelsize; kernely <= halfkernelsize; kernely++)
					{
						for (int kernelx = -1 * halfkernelsize; kernelx <= halfkernelsize; kernelx++)
						{
							int xKoord = x + kernelx;
							int yKoord = y + kernely;
							int zKoord = z + kernelz;
							if (xKoord < 0)
								xKoord = 0;
							if (xKoord >= dimensions[0])
								xKoord = dimensions[0] -1;
							if (yKoord < 0)
								yKoord = 0;
							if (yKoord >= dimensions[1])
								yKoord = dimensions[1] - 1;
							if (zKoord < 0)
								zKoord = 0;
							if (zKoord >= dimensions[2])
								zKoord = dimensions[2] - 1;

							kernelvalues[index] = inData->GetScalarComponentAsFloat(xKoord, yKoord, zKoord, 0);
							index++;
						}
					}
				}
				float median = self->getMedian(kernelvalues);
				outData->SetScalarComponentFromFloat(x, y, z, 0, median);
			}
		}
	}
	delete [] kernelvalues;
}

float MedianFilter::getMedian(float* list)
{
	int elementcount = this->KernelSize * this->KernelSize * this->KernelSize;
	std::sort(list, list + (elementcount));
	int half = (KernelSize + 1) / 2;
	return list[half];
}


void MedianFilter::ThreadedRequestData(
	vtkInformation *vtkNotUsed(request),
	vtkInformationVector **inputVector,
	vtkInformationVector *vtkNotUsed(outputVector),
	vtkImageData ***inData,
	vtkImageData **outData,
	int outExt[6], int id)
{
	void *inPtr;
	void *outPtr = outData[0]->GetScalarPointerForExtent(outExt);

	vtkDataArray *inArray = this->GetInputArrayToProcess(0, inputVector);
	if (id == 0)
	{
		outData[0]->GetPointData()->GetScalars()->SetName(inArray->GetName());
	}

	inPtr = inArray->GetVoidPointer(0);

	// this filter expects that input is the same type as output.
	if (inArray->GetDataType() != outData[0]->GetScalarType())
	{
		vtkErrorMacro(<< "Execute: input data type, " << inArray->GetDataType()
			<< ", must match out ScalarType "
			<< outData[0]->GetScalarType());
		return;
	}

	switch (inArray->GetDataType())
	{
		vtkTemplateMacro(
			MedianFilterExecute(this, inData[0][0],
			static_cast<VTK_TT *>(inPtr),
			outData[0], static_cast<VTK_TT *>(outPtr),
			outExt, id, inArray));
	default:
		vtkErrorMacro(<< "Execute: Unknown input ScalarType");
		return;
	}
}
