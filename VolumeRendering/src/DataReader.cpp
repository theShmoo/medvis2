// Project Includes
#include "DataReader.h"

// VTK includes
#include "vtkAlgorithm.h"
#include "vtkImageData.h"
#include "vtkDICOMImageReader.h"
#include "vtkXMLImageDataReader.h"
#include "vtkMetaImageReader.h"
#include "vtkImageResample.h"

DataReader::DataReader()
{
  reader = nullptr;
  input = nullptr;
  outputPort = nullptr;

}

/************************************************************************/
/************************************************************************/
bool DataReader::readFile(TFileType fileType, char* filename, char* dirname)
{
  bool bOk = true;
  if (dirname)
  {
    bOk = readDicomFile(dirname);
  }
  else if (fileType == FT_VTI)
  {
    bOk = readXMLFile(filename);
  }
  else if (fileType == FT_MHA)
  {
    bOk = readMetaFile(filename);
  }
  else
  {
    cout << "Error! Not VTI or MHA!" << endl;
    exit(EXIT_FAILURE);
  }

  // Verify that we actually have a volume
  int dim[3];
  input->GetDimensions(dim);
  if (dim[0] < 2 ||
    dim[1] < 2 ||
    dim[2] < 2)
  {
    cout << "Error loading data!" << endl;
    exit(EXIT_FAILURE);
  }

  outputPort = reader->GetOutputPort();

  return bOk;
}

/************************************************************************/
/************************************************************************/
bool DataReader::readDicomFile(char *dirname)
{
  vtkDICOMImageReader *dicomReader = vtkDICOMImageReader::New();
  dicomReader->SetDirectoryName(dirname);
  dicomReader->Update();
  input = dicomReader->GetOutput();
  reader = dicomReader;
  return true;
}

/************************************************************************/
/************************************************************************/
bool DataReader::readXMLFile(char *filename)
{
  vtkXMLImageDataReader *xmlReader = vtkXMLImageDataReader::New();
  xmlReader->SetFileName(filename);
  xmlReader->Update();
  input = xmlReader->GetOutput();
  reader = xmlReader;
  return true;
}

/************************************************************************/
/************************************************************************/
bool DataReader::readMetaFile(char *filename)
{
  vtkMetaImageReader *metaReader = vtkMetaImageReader::New();
  metaReader->SetFileName(filename);
  metaReader->Update();
  input = metaReader->GetOutput();
  reader = metaReader;
  return true;
}

void DataReader::resampleData(double reductionFactor)
{

  resample = nullptr;
  if (reductionFactor < 1.0)
  {
    resample = vtkImageResample::New();
    resample->SetInputConnection(reader->GetOutputPort());
    resample->SetAxisMagnificationFactor(0, reductionFactor);
    resample->SetAxisMagnificationFactor(1, reductionFactor);
    resample->SetAxisMagnificationFactor(2, reductionFactor);
    outputPort = resample->GetOutputPort();
    input = resample->GetOutput();
  }
}

DataReader::~DataReader()
{
  reader->Delete();
  if(resample)
    resample->Delete();
}

