#include "BilateralFilter.h"

#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"

vtkStandardNewMacro(BilateralFilter);

BilateralFilter::BilateralFilter()
{
  this->GaussFilter = nullptr;
  this->setKernelSize(3);
}


BilateralFilter::~BilateralFilter()
{
  if (this->GaussFilter)
  {
    delete this->GaussFilter;
    this->GaussFilter = nullptr;
  }
}

void BilateralFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Kernel Size: " << this->KernelSize << "\n";
}

inline double BilateralFilter::gauss(double sigma, int x, int y, int z) {
  return exp(-((x * x + y * y + z * z) / (2 * sigma * sigma)));
}

template <class T>
void BilateralFilterExecute(BilateralFilter *self,
  vtkImageData *inData, T *inPtr,
  vtkImageData *outData, T *outPtr,
  int outExt[6], int id,
  vtkDataArray *inArray)
{
  int kernelSize = self->GetKernelSize();
  int* dimensions = inData->GetDimensions();
  int halfkernelsize = (kernelSize - 1) / 2;
  int index = 0;
  int elementcount = pow(kernelSize,3);
  double Sigma = self->GetSigma();
  double Sigmas2 = (2 * Sigma * Sigma);
  double* gaussK = self->GetGaussKernel();

  for (int z = 0; z < dimensions[2]; z++)
  {
    for (int y = 0; y < dimensions[1]; y++)
    {
      for (int x = 0; x < dimensions[0]; x++)
      {
        index = 0;
        double sum = 0.0f;
        double totalWeight = 0.0f;
        float intensityCenter = inData->GetScalarComponentAsFloat(x, y, z, 0);
        for (int kernelz = -halfkernelsize; kernelz <= halfkernelsize; kernelz++)
        {
          for (int kernely = -halfkernelsize; kernely <= halfkernelsize; kernely++)
          {
            for (int kernelx = -halfkernelsize; kernelx <= halfkernelsize; kernelx++)
            {
              int xKoord = x + kernelx;
              int yKoord = y + kernely;
              int zKoord = z + kernelz;
              if (xKoord < 0)
                xKoord = 0;
              if (xKoord >= dimensions[0])
                xKoord = dimensions[0] - 1;
              if (yKoord < 0)
                yKoord = 0;
              if (yKoord >= dimensions[1])
                yKoord = dimensions[1] - 1;
              if (zKoord < 0)
                zKoord = 0;
              if (zKoord >= dimensions[2])
                zKoord = dimensions[2] - 1;
              //int id = (kernelx + halfkernelsize) * kernelSize * kernelSize + (kernely + halfkernelsize) * kernelSize + kernelz + halfkernelsize;
              float v = inData->GetScalarComponentAsFloat(xKoord, yKoord, zKoord, 0);
              double weight = gaussK[index] * exp(-((abs(v - intensityCenter)) / Sigmas2));
              totalWeight += weight;
              sum += (weight * v);
              index++;
            }
          }
        }

        sum /= totalWeight;
        outData->SetScalarComponentFromFloat(x, y, z, 0, static_cast<float>(sum/totalWeight));
      }
    }
  }
}

void BilateralFilter::ThreadedRequestData(
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
      BilateralFilterExecute(this, inData[0][0],
      static_cast<VTK_TT *>(inPtr),
      outData[0], static_cast<VTK_TT *>(outPtr),
      outExt, id, inArray));
  default:
    vtkErrorMacro(<< "Execute: Unknown input ScalarType");
    return;
  }
}

void BilateralFilter::setKernelSize(int n)
{
  if (n != this->KernelSize)
  {
    if (n % 2 == 0)
      n++;

    this->KernelSize = n;
    this->Sigma = ItlComputeSigma(n);
    if (this->GaussFilter)
    {
      delete this->GaussFilter;
      this->GaussFilter = nullptr;
    }
    this->GaussFilter = ItlCreateGaussFilter(this->KernelSize, this->Sigma);
  }
}

double BilateralFilter::ItlComputeSigma(int _KernelSize)
{
  return 0.3*((_KernelSize - 1)*0.5 - 1) + 0.8;
}

double* BilateralFilter::ItlCreateGaussFilter(int kernelSize, double Sigma)
{
  int elementcount = pow(kernelSize, 3);
  int halfkernelsize = (kernelSize - 1) / 2;
  double *kernel = new double[elementcount];

  for (int x = -halfkernelsize; x < -halfkernelsize + kernelSize; x++) {
    for (int y = -halfkernelsize; y < -halfkernelsize + kernelSize; y++) {
      for (int z = -halfkernelsize; z < -halfkernelsize + kernelSize; z++) {
        int id = (x + halfkernelsize) * kernelSize * kernelSize + (y + halfkernelsize) * kernelSize + z + halfkernelsize;
        kernel[id] = this->gauss(Sigma, x, y, z);
      }
    }
  }
  return kernel;
}