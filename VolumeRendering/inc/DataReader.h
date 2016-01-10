#pragma once

class vtkAlgorithm;
class vtkImageData;
class vtkImageResample;
class vtkAlgorithmOutput;

//! Reads volume data from a file
class DataReader
{
public:
  //! Constructor
  DataReader();
  //! Destructor
  ~DataReader();

  //! The type of the file
  enum TFileType
  {
    FT_VTI, //! A VTI File
    FT_MHA, //! A MHA File
    FT_NUM  
  };

  /*! Reads the file. if a directory name is set then a DICOM file gets loaded
  if not the reader tries to read the file with the specified filename and filetype.*/
  bool readFile(TFileType fileType, char* fileName, char* dirname);
  //! Reads a DICOM file from the directory
  bool readDicomFile(char *dirname);
  //! Reads a VTI file
  bool readXMLFile(char *filename);
  //! Reads a MHA File
  bool readMetaFile(char *filename);
  //! reduce the size of the volume data the resample factor needs to be smaller than 1.0
  void resampleData(double resampleFactore);

  //! Get the reader
  vtkAlgorithm * GetReader() const { return reader; };
  //! Get the image data
  vtkImageData * GetImageData() const { return input; };
  //! Get the output port. if it is resampled this is the resampled output port
  vtkAlgorithmOutput * GetOutputPort() const { return outputPort; };
  //! Get the resampled data.
  vtkImageResample * GetResample() const { return resample; };

private:
  vtkAlgorithmOutput *outputPort;
  vtkAlgorithm *reader;
  vtkImageData *input;
  vtkImageResample *resample;
 
};