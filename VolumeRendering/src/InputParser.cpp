// Project Includes
#include "InputParser.h"
#include "DataReader.h"

// Lib Includes
#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>

// VTK Includes
#include "vtkIOStream.h"

InputParser::InputParser(int argc, char *argv[])
{
	count = 1;
	dirname = nullptr;
	opacityWindow = 4096;
	opacityLevel = 2048;
	blendType = 0;
	clip = false;
	reductionFactor = 1.0;
	frameRate = 10.0;
	fileName = 0;
  fileType = DataReader::FT_NUM;
  independentComponents = true;

	while (count < argc)
	{
		if (!strcmp(argv[count], "?"))
		{
			ItlPrintUsage();
			exit(EXIT_SUCCESS);
		}
		else if (!strcmp(argv[count], "-DICOM"))
		{
			dirname = new char[strlen(argv[count + 1]) + 1];
			sprintf(dirname, "%s", argv[count + 1]);
			count += 2;
		}
		else if (!strcmp(argv[count], "-VTI"))
		{
			fileName = new char[strlen(argv[count + 1]) + 1];
      fileType = DataReader::FT_VTI;
			sprintf(fileName, "%s", argv[count + 1]);
			count += 2;
		}
		else if (!strcmp(argv[count], "-MHA"))
		{
			fileName = new char[strlen(argv[count + 1]) + 1];
      fileType = DataReader::FT_MHA;
			sprintf(fileName, "%s", argv[count + 1]);
			count += 2;
		}
		else if (!strcmp(argv[count], "-Clip"))
		{
			clip = true;
			count++;
		}
		else if (!strcmp(argv[count], "-MIP"))
		{
			opacityWindow = atof(argv[count + 1]);
			opacityLevel = atof(argv[count + 2]);
			blendType = 0;
			count += 3;
		}
		else if (!strcmp(argv[count], "-CompositeRamp"))
		{
			opacityWindow = atof(argv[count + 1]);
			opacityLevel = atof(argv[count + 2]);
			blendType = 1;
			count += 3;
		}
		else if (!strcmp(argv[count], "-CompositeShadeRamp"))
		{
			opacityWindow = atof(argv[count + 1]);
			opacityLevel = atof(argv[count + 2]);
			blendType = 2;
			count += 3;
		}
		else if (!strcmp(argv[count], "-CT_Skin"))
		{
			blendType = 3;
			count += 1;
		}
		else if (!strcmp(argv[count], "-CT_Bone"))
		{
			blendType = 4;
			count += 1;
		}
		else if (!strcmp(argv[count], "-CT_Muscle"))
		{
			blendType = 5;
			count += 1;
		}
		else if (!strcmp(argv[count], "-RGB_Composite"))
		{
			blendType = 6;
			count += 1;
		}
		else if (!strcmp(argv[count], "-FrameRate"))
		{
			frameRate = atof(argv[count + 1]);
			if (frameRate < 0.01 || frameRate > 60.0)
			{
				cout << "Invalid frame rate - use a number between 0.01 and 60.0" << endl;
				cout << "Using default frame rate of 10 frames per second." << endl;
				frameRate = 10.0;
			}
			count += 2;
		}
		else if (!strcmp(argv[count], "-ReductionFactor"))
		{
			reductionFactor = atof(argv[count + 1]);
			if (reductionFactor <= 0.0 || reductionFactor >= 1.0)
			{
				cout << "Invalid reduction factor - use a number between 0 and 1 (exclusive)" << endl;
				cout << "Using the default of no reduction." << endl;
				reductionFactor = 1.0;
			}
			count += 2;
		}
		else if (!strcmp(argv[count], "-DependentComponents"))
		{
			independentComponents = false;
			count += 1;
		}
		else
		{
			cout << "Unrecognized option: " << argv[count] << endl;
			cout << endl;
			ItlPrintUsage();
			exit(EXIT_FAILURE);
		}
	}
}

/************************************************************************/
/************************************************************************/
InputParser::~InputParser()
{

}

/************************************************************************/
/************************************************************************/
void InputParser::ItlPrintUsage()
{
	cout << "Usage: " << endl;
	cout << endl;
	cout << "  GPURenderDemo <options>" << endl;
	cout << endl;
	cout << "where options may include: " << endl;
	cout << endl;
	cout << "  -DICOM <directory>" << endl;
	cout << "  -VTI <filename>" << endl;
	cout << "  -MHA <filename>" << endl;
	cout << "  -DependentComponents" << endl;
	cout << "  -Clip" << endl;
	cout << "  -MIP <window> <level>" << endl;
	cout << "  -CompositeRamp <window> <level>" << endl;
	cout << "  -CompositeShadeRamp <window> <level>" << endl;
	cout << "  -CT_Skin" << endl;
	cout << "  -CT_Bone" << endl;
	cout << "  -CT_Muscle" << endl;
	cout << "  -FrameRate <rate>" << endl;
	cout << "  -DataReduction <factor>" << endl;
	cout << endl;
	cout << "You must use either the -DICOM option to specify the directory where" << endl;
	cout << "the data is located or the -VTI or -MHA option to specify the path of a .vti file." << endl;
	cout << endl;
	cout << "By default, the program assumes that the file has independent components," << endl;
	cout << "use -DependentComponents to specify that the file has dependent components." << endl;
	cout << endl;
	cout << "Use the -Clip option to display a cube widget for clipping the volume." << endl;
	cout << "Use the -FrameRate option with a desired frame rate (in frames per second)" << endl;
	cout << "which will control the interactive rendering rate." << endl;
	cout << "Use the -DataReduction option with a reduction factor (greater than zero and" << endl;
	cout << "less than one) to reduce the data before rendering." << endl;
	cout << "Use one of the remaining options to specify the blend function" << endl;
	cout << "and transfer functions. The -MIP option utilizes a maximum intensity" << endl;
	cout << "projection method, while the others utilize compositing. The" << endl;
	cout << "-CompositeRamp option is unshaded compositing, while the other" << endl;
	cout << "compositing options employ shading." << endl;
	cout << endl;
	cout << "Note: MIP, CompositeRamp, CompositeShadeRamp, CT_Skin, CT_Bone," << endl;
	cout << "and CT_Muscle are appropriate for DICOM data. MIP, CompositeRamp," << endl;
	cout << "and RGB_Composite are appropriate for RGB data." << endl;
	cout << endl;
	cout << "Example: GPURenderDemo -DICOM CTNeck -MIP 4096 1024" << endl;
	cout << endl;
}
