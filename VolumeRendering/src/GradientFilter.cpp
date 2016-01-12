#include "GradientFilter.h"

#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <math.h>

vtkStandardNewMacro(GradientFilter);

//----------------------------------------------------------------------------
// Construct an instance of GradientFilter fitler.
GradientFilter::GradientFilter()
{
  this->KernelSize[0] = 3;
  this->KernelSize[1] = 3;
  this->KernelSize[2] = 3;
  this->KernelMiddle[0] = 1;
  this->KernelMiddle[1] = 1;
  this->KernelMiddle[2] = 1;
  this->HandleBoundaries = 1;
}

//----------------------------------------------------------------------------
void GradientFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
// This execute method handles boundaries.
// it handles boundaries. Pixels are just replicated to get values
// out of extent.
template <class T>
void GradientFilterExecute(GradientFilter *self,
	vtkImageData *inData, T *inPtr,
	vtkImageData *outData, T *outPtr,
	int outExt[6], int id,
	vtkDataArray *inArray)
{
  double r0, r1, r2, *r;
  // For looping though output (and input) pixels.
  int min0, max0, min1, max1, min2, max2;
  int outIdx0, outIdx1, outIdx2;
  vtkIdType outInc0, outInc1, outInc2;
  T *outPtr0, *outPtr1, *outPtr2, *outPtrV;
  vtkIdType inInc0, inInc1, inInc2;
  T *inPtr0, *inPtr1, *inPtr2;
  // For sobel function convolution (Left Right incs for each axis)
  vtkIdType inInc0L, inInc0R, inInc1L, inInc1R, inInc2L, inInc2R;
  T *inPtrL, *inPtrR;
  double sum;
  double x,y,z;
  // Boundary of input image
  int inWholeMin0, inWholeMax0, inWholeMin1, inWholeMax1;
  int inWholeMin2, inWholeMax2;
  int *inWholeExt;
  unsigned long count = 0;
  unsigned long target;

  // Get boundary information
  inWholeExt = inData->GetExtent();
  inWholeMin0 = inWholeExt[0];
  inWholeMax0 = inWholeExt[1];
  inWholeMin1 = inWholeExt[2];
  inWholeMax1 = inWholeExt[3];
  inWholeMin2 = inWholeExt[4];
  inWholeMax2 = inWholeExt[5];

  // Get information to march through data (skip component)
  inData->GetIncrements(inInc0, inInc1, inInc2);
  outData->GetIncrements(outInc0, outInc1, outInc2);
  min0 = outExt[0];   max0 = outExt[1];
  min1 = outExt[2];   max1 = outExt[3];
  min2 = outExt[4];   max2 = outExt[5];

  // We want the input pixel to correspond to output
  inPtr = static_cast<T *>(inData->GetScalarPointer(min0, min1, min2));

  // The data spacing is important for computing the gradient.
  // Scale so it has the same range as gradient.
  r = inData->GetSpacing();
  r0 = 0.060445 / r[0];
  r1 = 0.060445 / r[1];
  r2 = 0.060445 / r[2];

  target = static_cast<unsigned long>((max2 - min2 + 1)*(max1 - min1 + 1) / 50.0);
  target++;

  // loop through pixels of output
  outPtr2 = outPtr;
  inPtr2 = inPtr;
  for (outIdx2 = min2; outIdx2 <= max2; ++outIdx2)
  {
    inInc2L = (outIdx2 == inWholeMin2) ? 0 : -inInc2;
    inInc2R = (outIdx2 == inWholeMax2) ? 0 : inInc2;

    outPtr1 = outPtr2;
    inPtr1 = inPtr2;
    for (outIdx1 = min1; !self->AbortExecute && outIdx1 <= max1; ++outIdx1)
    {
      if (!id)
      {
        if (!(count%target))
        {
          self->UpdateProgress(count / (50.0*target));
        }
        count++;
      }
      inInc1L = (outIdx1 == inWholeMin1) ? 0 : -inInc1;
      inInc1R = (outIdx1 == inWholeMax1) ? 0 : inInc1;

      outPtr0 = outPtr1;
      inPtr0 = inPtr1;
      for (outIdx0 = min0; outIdx0 <= max0; ++outIdx0)
      {
        inInc0L = (outIdx0 == inWholeMin0) ? 0 : -inInc0;
        inInc0R = (outIdx0 == inWholeMax0) ? 0 : inInc0;

        // compute vector.
        outPtrV = outPtr0;
        // 12 Plane
        inPtrL = inPtr0 + inInc0L;
        inPtrR = inPtr0 + inInc0R;
        sum = 2.0 * (*inPtrR - *inPtrL);
        sum += static_cast<double>(inPtrR[inInc1L] + inPtrR[inInc1R]
          + inPtrR[inInc2L] + inPtrR[inInc2R]);
        sum += static_cast<double>(0.586 * (inPtrR[inInc1L + inInc2L] + inPtrR[inInc1L + inInc2R]
          + inPtrR[inInc1R + inInc2L] + inPtrR[inInc1R + inInc2R]));
        sum -= static_cast<double>(inPtrL[inInc1L] + inPtrL[inInc1R]
          + inPtrL[inInc2L] + inPtrL[inInc2R]);
        sum -= static_cast<double>(0.586 * (inPtrL[inInc1L + inInc2L] + inPtrL[inInc1L + inInc2R]
          + inPtrL[inInc1R + inInc2L] + inPtrL[inInc1R + inInc2R]));
        x = sum * r0;
        // 02 Plane
        inPtrL = inPtr0 + inInc1L;
        inPtrR = inPtr0 + inInc1R;
        sum = 2.0 * (*inPtrR - *inPtrL);
        sum += static_cast<double>(inPtrR[inInc0L] + inPtrR[inInc0R]
          + inPtrR[inInc2L] + inPtrR[inInc2R]);
        sum += static_cast<double>(0.586 * (inPtrR[inInc0L + inInc2L] + inPtrR[inInc0L + inInc2R]
          + inPtrR[inInc0R + inInc2L] + inPtrR[inInc0R + inInc2R]));
        sum -= static_cast<double>(inPtrL[inInc0L] + inPtrL[inInc0R]
          + inPtrL[inInc2L] + inPtrL[inInc2R]);
        sum -= static_cast<double>(0.586 * (inPtrL[inInc0L + inInc2L] + inPtrL[inInc0L + inInc2R]
          + inPtrL[inInc0R + inInc2L] + inPtrL[inInc0R + inInc2R]));
        y = sum * r1;
        // 01 Plane
        inPtrL = inPtr0 + inInc2L;
        inPtrR = inPtr0 + inInc2R;
        sum = 2.0 * (*inPtrR - *inPtrL);
        sum += static_cast<double>(inPtrR[inInc0L] + inPtrR[inInc0R]
          + inPtrR[inInc1L] + inPtrR[inInc1R]);
        sum += static_cast<double>(0.586 * (inPtrR[inInc0L + inInc1L] + inPtrR[inInc0L + inInc1R]
          + inPtrR[inInc0R + inInc1L] + inPtrR[inInc0R + inInc1R]));
        sum -= static_cast<double>(inPtrL[inInc0L] + inPtrL[inInc0R]
          + inPtrL[inInc1L] + inPtrL[inInc1R]);
        sum -= static_cast<double>(0.586 * (inPtrL[inInc0L + inInc1L] + inPtrL[inInc0L + inInc1R]
          + inPtrL[inInc0R + inInc1L] + inPtrL[inInc0R + inInc1R]));
		z = sum * r2;
		*outPtrV = static_cast<double>(sqrt(x*x + y*y + z*z));
		++outPtrV;

        outPtr0 += outInc0;
        inPtr0 += inInc0;
      }
      outPtr1 += outInc1;
      inPtr1 += inInc1;
    }
    outPtr2 += outInc2;
    inPtr2 += inInc2;
  }
}

//----------------------------------------------------------------------------
// This method contains a switch statement that calls the correct
// templated function for the input Data type.  The output Data
// must be of type double.  This method does handle boundary conditions.
// The third axis is the component axis for the output.
void GradientFilter::ThreadedRequestData(
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
			GradientFilterExecute(this, inData[0][0],
			static_cast<VTK_TT *>(inPtr),
			outData[0], static_cast<VTK_TT *>(outPtr),
			outExt, id, inArray));
	default:
		vtkErrorMacro(<< "Execute: Unknown input ScalarType");
		return;
	}
}
