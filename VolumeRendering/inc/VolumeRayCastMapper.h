#pragma once

/*=========================================================================
Program:   Visualization Toolkit
Module:    vtkGPUMultiVolumeRayCastMapper.h
Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Modified by:  Carlos Falc�n cfalcon@ctim.es
Karl Krissian karl@ctim.es

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.
=========================================================================*/
// .NAME vtkGPUMultiVolumeRayCastMapper - Ray casting performed on the GPU.
// .SECTION Description
// vtkGPUMultiVolumeRayCastMapper is a volume mapper that performs ray casting on
// the GPU using fragment programs, using two input datasets.

#include "vtkVolumeMapper.h"

class vtkVolumeProperty;
class vtkRenderWindow;
class vtkImageData;
class vtkTransform;

#define VTK_VOLUMERENDERING_EXPORT

//! Class for raycasting with opengl shaders. Copyright by Ken Martin, Will Schroeder, Bill Lorensen, Carlos Falc�n and Karl Krissian. Modified by us so we can use it for our pupose
class VTK_VOLUMERENDERING_EXPORT VolumeRayCastMapper : public vtkVolumeMapper
{
public:
	//! This Value stores the number of additional volumes. In our case its allways zero
	int NUMBER_OF_ADDITIONAL_VOLUMES; 

	//! Creates a new raycastmapper and returns it
	static VolumeRayCastMapper *New();

	vtkTypeMacro(VolumeRayCastMapper, vtkVolumeMapper);

	//! Write relevant parameter of this class in the out-console
	void PrintSelf(ostream& os, vtkIndent indent);

	// Define the Input for additional datasets 
	//! Sets the input of the vtk pipeline and its port
	void SetInput(int port, vtkImageData *input);

	//! Sets the input of the vtk pipeline and its port
	void SetInput(int port, vtkDataSet *genericInput);

	//! Returns the imagedata
	vtkImageData * GetInput(int port = 0);
	
	//! Sets number of additional volumes (in our case its allways zero)
	void setNumberOfAdditionalVolumes(int numAddVolumes);

	/** If AutoAdjustSampleDistances is on, the the ImageSampleDistance
	* will be varied to achieve the allocated render time of this
	* prop (controlled by the desired update rate and any culling in
	* use).
	*/
	vtkSetClampMacro(AutoAdjustSampleDistances, int, 0, 1);
	vtkGetMacro(AutoAdjustSampleDistances, int);
	vtkBooleanMacro(AutoAdjustSampleDistances, int);

	/** Set/Get the distance between samples used for rendering
	* when AutoAdjustSampleDistances is off, or when this mapper
	* has more than 1 second allocated to it for rendering.
	* Initial value is 1.0.
	*/
	vtkSetMacro(SampleDistance, float);
	vtkGetMacro(SampleDistance, float);

	/** Sampling distance in the XY image dimensions. Default value of 1 meaning
	* 1 ray cast per pixel. If set to 0.5, 4 rays will be cast per pixel. If
	* set to 2.0, 1 ray will be cast for every 4 (2 by 2) pixels. This value
	* will be adjusted to meet a desired frame rate when AutoAdjustSampleDistances
	* is on.
	*/
	vtkSetClampMacro(ImageSampleDistance, float, 0.1f, 100.0f);
	vtkGetMacro(ImageSampleDistance, float);


	//! This is the minimum image sample distance allow when the image sample distance is being automatically adjusted.
	vtkSetClampMacro(MinimumImageSampleDistance, float, 0.1f, 100.0f);
	vtkGetMacro(MinimumImageSampleDistance, float);

	//! This is the maximum image sample distance allow when the image sample distance is being automatically adjusted.
	vtkSetClampMacro(MaximumImageSampleDistance, float, 0.1f, 100.0f);
	vtkGetMacro(MaximumImageSampleDistance, float);


	/** Set/Get the window / level applied to the final color.
	* This allows brightness / contrast adjustments on the
	* final image.
	* window is the width of the window.
	* level is the center of the window.
	* Initial window value is 1.0
	* Initial level value is 0.5
	* window cannot be null but can be negative, this way
	* values will be reversed.
	* |window| can be larger than 1.0
	* level can be any real value.
	*/
	vtkSetMacro(FinalColorWindow, float);
	vtkGetMacro(FinalColorWindow, float);
	vtkSetMacro(FinalColorLevel, float);
	vtkGetMacro(FinalColorLevel, float);

	/** Maximum size of the 3D texture in GPU memory.
	* Will default to the size computed from the graphics
	* card. Can be adjusted by the user.
	*/
	vtkSetMacro(MaxMemoryInBytes, vtkIdType);
	vtkGetMacro(MaxMemoryInBytes, vtkIdType);

	
	/** Maximum fraction of the MaxMemoryInBytes that should
	* be used to hold the texture. Valid values are 0.1 to
	* 1.0.
	*/
	vtkSetClampMacro(MaxMemoryFraction, float, 0.1f, 1.0f);
	vtkGetMacro(MaxMemoryFraction, float);

	/** Tells if the mapper will report intermediate progress.
	* Initial value is true. As the progress works with a GL blocking
	* call (glFinish()), this can be useful for huge dataset but can
	* slow down rendering of small dataset. It should be set to true
	* for big dataset or complex shading and streaming but to false for
	* small datasets.
	*/
	vtkSetMacro(ReportProgress, bool);
	vtkGetMacro(ReportProgress, bool);


	/** Based on hardware and properties, we may or may not be able to
	* render using 3D texture mapping. This indicates if 3D texture
	* mapping is supported by the hardware, and if the other extensions
	* necessary to support the specific properties are available.
	*/
	virtual int IsRenderSupported(vtkRenderWindow *vtkNotUsed(window),
		vtkVolumeProperty *vtkNotUsed(property))
	{
		return 0;
	}

	void CreateCanonicalView(vtkRenderer *ren,
		vtkVolume *volume,
		vtkImageData *image,
		int blend_mode,
		double viewDirection[3],
		double viewUp[3]);


	/** Optionally, set a mask input. This mask may be a binary mask or a label
	* map. This must be specified via SetMaskType.
	*
	* If the mask is a binary mask, the volume rendering is confined to regions
	* within the binary mask. The binary mask is assumed to have a datatype of
	* UCHAR and values of 255 (inside) and 0 (outside).
	*
	* The mask may also be a label map. The label map is allowed to contain only
	* 3 labels (values of 0, 1 and 2) and must have a datatype of UCHAR. In voxels
	* with label value of 0, the color transfer function supplied by component
	* 0 is used.
	*   In voxels with label value of 1, the color transfer function supplied by
	* component 1 is used and blended with the transfer function supplied by
	* component 0, with the blending weight being determined by
	* MaskBlendFactor.
	*    In voxels with a label value of 2, the color transfer function supplied
	*  by component 2 is used and blended with the transfer function supplied by
	* component 0, with the blending weight being determined by
	* MaskBlendFactor.
	*/
	void SetMaskInput(vtkImageData *mask);
	vtkGetObjectMacro(MaskInput, vtkImageData);

	enum { BinaryMaskType = 0, LabelMapMaskType };

	//! Set the mask type, if mask is to be used. See documentation for SetMaskInput(). The default is a LabelMapMaskType.
	vtkSetMacro(MaskType, int);
	vtkGetMacro(MaskType, int);
	void SetMaskTypeToBinary();
	void SetMaskTypeToLabelMap();

	/** Tells how much mask color transfer function is used compared to the
	* standard color transfer function when the mask is true. This is relevant
	* only for the label map mask.
	*   0.0 means only standard color transfer function.
	*   1.0 means only mask color tranfer function.
	* The default value is 1.0.
	*/
	vtkSetClampMacro(MaskBlendFactor, float, 0.0f, 1.0f);
	vtkGetMacro(MaskBlendFactor, float);


	/** WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
	* Initialize rendering for this volume.
	*/
	void Render(vtkRenderer *, vtkVolume *);

	//! Handled in the subclass - the actual render method
	virtual void GPURender(vtkRenderer *, vtkVolume *) {}

	/** WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
	* Release any graphics resources that are being consumed by this mapper.
	* The parameter window could be used to determine which graphic
	* resources to release.
	*/
	void ReleaseGraphicsResources(vtkWindow *) {};

	/** Return how much the dataset has to be reduced in each dimension to
	* fit on the GPU. If the value is 1.0, there is no need to reduce the
	* dataset.
	* The computation is based on hardware limits (3D texture indexable size)
	* and MaxMemoryInBytes.
	*/
	virtual void GetReductionRatio(double ratio[3]) = 0;

protected:
	VolumeRayCastMapper();
	~VolumeRayCastMapper();

	//! Check to see that the render will be OK
	int ValidateRender(vtkRenderer *, vtkVolume *);


	//! Special version of render called during the creation of a canonical view.
	void CanonicalViewRender(vtkRenderer *, vtkVolume *);

	//! Methods called by the AMR Volume Mapper.
	virtual void PreRender(vtkRenderer *ren,
		vtkVolume *vol,
		double datasetBounds[6],
		double scalarRange[2],
		int numberOfScalarComponents,
		unsigned int numberOfLevels) = 0;

	virtual void RenderBlock(vtkRenderer *ren,
		vtkVolume *vol,
		unsigned int level) = 0;

	virtual void PostRender(vtkRenderer *ren,
		int numberOfScalarComponents) = 0;

	/** Called by the AMR Volume Mapper.
	* Set the flag that tells if the scalars are on point data (0) or
	* cell data (1).
	*/
	void SetCellFlag(int cellFlag);

	//! The distance between sample points along the ray
	float  SampleDistance;


	float  ImageSampleDistance;
	float  MinimumImageSampleDistance;
	float  MaximumImageSampleDistance;
	int    AutoAdjustSampleDistances;

	int    SmallVolumeRender;
	double BigTimeToDraw;
	double SmallTimeToDraw;

	float FinalColorWindow;
	float FinalColorLevel;

	vtkIdType MaxMemoryInBytes;
	float MaxMemoryFraction;

	//! 1 if we are generating the canonical image, 0 otherwise
	int   GeneratingCanonicalView;
	vtkImageData *CanonicalViewImageData;

	//! Set the mapper in AMR Mode or not. Initial value is false. Called only by the vtkKWAMRVolumeMapper
	vtkSetClampMacro(AMRMode, int, 0, 1);
	vtkGetMacro(AMRMode, int);
	vtkBooleanMacro(AMRMode, int);

	int AMRMode;
	int CellFlag; //! point data or cell data (or field data, not handled) 

	/** Compute the cropping planes clipped by the bounds of the volume.
	* The result is put into this->ClippedCroppingRegionPlanes.
	* NOTE: IT WILL BE MOVED UP TO vtkVolumeMapper after bullet proof usage
	* in this mapper. Other subclasses will use the ClippedCroppingRegionsPlanes
	* members instead of CroppingRegionPlanes.
	*/
	virtual void ClipCroppingRegionPlanes();

	double         ClippedCroppingRegionPlanes[6];
	bool           ReportProgress;
	vtkImageData * MaskInput;
	float          MaskBlendFactor;
	int            MaskType;

	vtkImageData * TransformedInput;

	vtkGetObjectMacro(TransformedInput, vtkImageData);
	void SetTransformedInput(vtkImageData*);


#define vtkGetObjectMacro2(name,type)                                   \
	virtual type *Get##name(int _arg)                                      \
	{                                                                     \
	return this->name[_arg/*M*/];/* //? */                                \
	}


	//! This is needed only to check if the input data has been changed since the last Render() call.
	vtkImageData* LastInput;

private:
	VolumeRayCastMapper(const VolumeRayCastMapper&);  // Not implemented.
	void operator=(const VolumeRayCastMapper&);  // Not implemented.
};

