#pragma once

class vtkAlgorithm;
class vtkImageData;
class vtkImageResample;
class vtkAlgorithmOutput;

class DataReader
{
public:
  DataReader();
  ~DataReader();

  enum TFileType
  {
    FT_VTI,
    FT_MHA,
    FT_NUM
  };

  bool readFile(TFileType fileType, char* fileName, char* dirname);
  bool readDicomFile(char *dirname);
  bool readXMLFile(char *filename);
  bool readMetaFile(char *filename);

  void resampleData(double resampleFactore);

  vtkAlgorithm * getReader() const { return reader; };
  vtkImageData * getImageData() const { return input; };
  vtkAlgorithmOutput * getOutputPort() const { return outputPort; };
  vtkImageResample * getResample() const { return resample; };

private:
  vtkAlgorithmOutput *outputPort;
  vtkAlgorithm *reader;
  vtkImageData *input;
  vtkImageResample *resample;
 
};