#pragma once

class InputParser
{
public:
	InputParser(int argc, char *argv[]);
	~InputParser();

  /// Returns the frame rate
  double getFrameRate() const { return frameRate; };

  /// Returns whether the clip option to display a cube widget for clipping the volume is activated or not
  bool isClippingActive() const { return clip; };

  /// Return whether it has independent components or not (default = true)
  bool hasIndependentComponents() const { return independentComponents; };

  /// Returns the directory of the dicom data (can be NULL);
  char * getDirName() const { return dirname; };

  /// Returns the filename of the XML or Meta file (can be NULL);
  char * getFileName() const { return fileName; };

  /// Returns the type of the file which is an enum from the data reader
  int getFileType() const { return fileType; };

  /// Returns the reduction factor of the data this has to be between 0 and 1
  double getReductionFactor() const { return reductionFactor; };

  int getBlendType() const { return blendType; };

  double getOpacityLevel() const { return opacityLevel; }

  double getOpacityWindow() const { return opacityWindow; }

private:

	/// Prints the usage message of the program
	void ItlPrintUsage();

	int count;
	char *dirname;
  double opacityWindow;
  double opacityLevel;

  int blendType;
  bool clip;
  double reductionFactor;
  double frameRate;
  char *fileName;
  int fileType;
  bool independentComponents;
};