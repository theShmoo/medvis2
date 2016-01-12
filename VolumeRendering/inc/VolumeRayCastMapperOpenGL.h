#pragma once
/*=========================================================================
Program:   Visualization Toolkit
Module:    vtkOpenGLGPUMultiVolumeRayCastMapper.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Modified by:  Carlos Falcón cfalcon@ctim.es
Karl Krissian karl@ctim.es

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.
=========================================================================*/
// .NAME vtkOpenGLGPUMultiVolumeRayCastMapper - OpenGL subclass that draws the
// image to the screen
// .SECTION Description
// This is the concrete implementation of a ray cast image display helper -
// a helper class responsible for drawing the image to the screen.

// .SECTION see also
// vtkGPUVolumeRayCastMapper


#include "VolumeRayCastMapper.h"

class vtkVolume;
class vtkRenderer;
class vtkOpenGLExtensionManager;
class vtkMatrix4x4;
class vtkUnsupportedRequiredExtensionsStringStream; // Pimpl
class vtkMapDataArrayTextureId; // Pimpl
class vtkMapMaskTextureId; // Pimpl
class vtkPolyData;
class vtkClipConvexPolyData;
class vtkClipPolyData;
class vtkTessellatedBoxSource;

class vtkOpacityTable; // internal class.
class vtkRGBTable; // internal class.
class vtkKWScalarField; // internal class.
class vtkKWMask; // internal class.

class vtkOpacityTables; // Pimpl
class vtkDensifyPolyData;
class vtkStdString;

class vtkShaderProgram2;//?
class vtkShader2;//?

class vtkTransform;

#include "vtkSmartPointer.h"

//! Class for raycasting with opengl shaders. Copyright by Ken Martin, Will Schroeder, Bill Lorensen, Carlos Falcón and Karl Krissian. Modified by us so we can use it for our pupose
class VolumeRayCastMapperOpenGL : public VolumeRayCastMapper
{
public:
	//! Creates a new raycastmapper and returns it
	static VolumeRayCastMapperOpenGL *New();

	vtkTypeMacro(VolumeRayCastMapperOpenGL, VolumeRayCastMapper);

	//! Write relevant parameter of this class in the out-console
	virtual void PrintSelf(ostream& os, vtkIndent indent);

	
	/** Based on hardware and properties, we may or may not be able to
	* render using 3D texture mapping. This indicates if 3D texture
	* mapping is supported by the hardware, and if the other extensions
	* necessary to support the specific properties are available.
	*/
	virtual int IsRenderSupported(vtkRenderWindow *window,
		vtkVolumeProperty *property);


	//! Delete OpenGL objects.
	virtual void ReleaseGraphicsResources(vtkWindow *window);

	//! Return a string matching the OpenGL errorCode.
	static const char *OpenGLErrorMessage(unsigned int errorCode);

	//! Display headerMessage on the standard output and the last OpenGL error message if any.
	static void PrintError(const char *headerMessage);

	//! Adds a clipping pane cut into the volume
	void AddClippingPlane(int vol, vtkPlane *plane); 

	//! Removes the clipping pane
	void RemoveClippingPlane(int vol); 

	//! Returns the clipping pane
	vtkPlane* GetClippingPlane(int vol); 

	//! Adds a cropping plane
	void SetCroppingRegionPlanes(int vol, double xMin, double xMax, double yMin, double yMax, double zMin, double zMax); 

	//! Adds a cropping plane
	void SetCroppingRegionPlanes(int vol, double *args); 

	//! Adds a cropping values
	void SetCropping(int vol, int arg);

protected:
	//! stores the clipping plane if exists
	vtkPlane* ClippingPlane;

	//! Stores if clipped enabled
	int Clipped;

	//! Stores if cropped enabled
	int Cropped;

	bool ClippedModified;

	double CroppingLowBounds[3]; 
	double CroppingHighBounds[3];

	void SetClippingPlane();


	VolumeRayCastMapperOpenGL();
	~VolumeRayCastMapperOpenGL();

	//! The render method called by the superclass
	virtual void GPURender(vtkRenderer *ren,
		vtkVolume *vol);

	//! Methods called by the AMR Volume Mapper.
	virtual void PreRender(vtkRenderer *ren,
		vtkVolume *vol,
		double datasetBounds[6],
		double scalarRange[2],
		int numberOfScalarComponents,
		unsigned int numberOfLevels);

	virtual void RenderBlock(vtkRenderer *ren,
		vtkVolume *vol,
		unsigned int level);

	virtual void PostRender(vtkRenderer *ren,
		int numberOfScalarComponents);


	//! Return if the required OpenGL extension `extensionName' is supported. If not, its name is added to the string of unsupported but required extensions.

	int TestRequiredExtension(vtkOpenGLExtensionManager *extensions,
		const char *extensionName);


	/** Attempt to load required and optional OpenGL extensions for the current
	* context window. Variable LoadExtensionsSucceeded is set if all required
	* extensions has been loaded. In addition, variable
	* Supports_GL_ARB_texture_float is set if this extension has been loaded.
	*/
	void LoadExtensions(vtkRenderWindow *window);


	/** Create OpenGL objects such as textures, buffers and fragment program Ids.
	* It only registers Ids, there is no actual initialization of textures or
	* fragment program.
	*/
	void CreateOpenGLObjects(vtkRenderer *ren);


	/** Allocate memory on the GPU for the framebuffers according to the size of
	* the window or reallocate if the size has changed. Return true if
	* allocation succeeded.
	*/
	int AllocateFrameBuffers(vtkRenderer *ren);


	/** Load the two scalar fields (one or four component scalar field), cell or point
	* based for a given subextent of the whole extent (can be the whole extent)
	* as a 3D texture on the GPU.
	* Extents are expressed in point if the cell flag is false or in cells of
	* the cell flag is true.
	* It returns true if it succeeded, false if there is not enough memory on
	* the GPU.
	* If succeeded, it updates the LoadedExtent, LoadedBounds, LoadedCellFlag
	* and LoadedTime. It also succeed if the scalar field is already loaded
	* (ie since last load, input has not changed and cell flag has not changed
	* and requested texture extents are enclosed in the loaded extent).
	*/
	int LoadScalarField(vtkImageData *input,
		int type,
		int textureExtent[6],
		vtkVolume *volume);

	//! Allocate memory and load color table on the GPU or reload it if the transfer function changed.
	int UpdateColorTransferFunction(vtkVolume *vol,
		int numberOfScalarComponents);

	//! Allocate memory and load opacity table on the GPU or reload it if the transfer functions changed.
	int UpdateOpacityTransferFunction(vtkVolume *vol,
		int numberOfScalarComponents,
		unsigned int level);


	//! Prepare rendering in the offscreen framebuffer.
	void SetupRender(vtkRenderer *ren, vtkVolume *vol);


	//! Clip the bounding box with all clipping planes and the near and far plane
	void ClipBoundingBox(vtkRenderer *ren,
		double worldBounds[6],
		vtkVolume *vol);


	//! Render the bounding box. The flag indicates whether or not tcoords are rendered too. Return abort status (true==abort).
	int RenderClippedBoundingBox(int tcoordFlag,
		size_t currentBlock,
		size_t numberOfBlocks,
		vtkRenderWindow *renWin);


	//! Method used to copy the state of the color buffer (which is in a frame buffer object) to a texture.
	void CopyFBOToTexture();

	//! Restore OpenGL state after rendering of the dataset.
	void CleanupRender();

	//! Render the offscreen buffer to the screen.
	void RenderTextureToScreen(vtkRenderer *ren);

	//! Compute y=2^n such that x<=y.
	int PowerOfTwoGreaterOrEqual(int x);

	//! Display the status of the current framebuffer on the standard output.
	void CheckFrameBufferStatus();

	//! Create a string from a buffer id. The result has to be free by the caller.
	vtkStdString BufferToString(int buffer);

	//! Display the buffers assigned for drawing and reading operations.
	void DisplayReadAndDrawBuffers();

	//! Display all the attachments of the current framebuffer object.
	void DisplayFrameBufferAttachments();

	//! Display a given attachment for the current framebuffer object.
	void DisplayFrameBufferAttachment(unsigned int uattachment);

	//! Concatenate the header string, projection type code and method to the final fragment code in this->FragmentCode.
	void BuildProgram(vtkRenderWindow *w,
		int parallelProjection,
		int raycastMethod,
		int shadeMethod,
		int componentMethod);

	//! Return the current OpenGL state about lighting.
	void GetLightingStatus();

	/** Update the reduction factor of the render viewport (this->ReductionFactor)
	* according to the time spent in seconds to render the previous frame
	* (this->TimeToDraw) and a time in seconds allocated to render the next
	* frame (allocatedTime).
	*/
	void ComputeReductionFactor(double allocatedTime);

	//! Render a subvolume.
	void RenderWholeVolume(vtkRenderer *ren,
		vtkVolume *vol);

	//! Render a subvolume.
	void RenderRegions(vtkRenderer *ren,
		vtkVolume *vol);

	//! Return abort status (true==abort)
	int RenderSubVolume(vtkRenderer *ren,
		double bounds[6],
		vtkVolume *vol);

	void LoadProjectionParameters(vtkRenderer *ren,
		vtkVolume *vol);

	//! Compute and return the number of cropping regions
	void ComputeNumberOfCroppingRegions();

	void GetTextureFormat(vtkImageData *input,
		unsigned int *internalFormat,
		unsigned int *format,
		unsigned int *type,
		int *componentSize);

	bool TestLoadingScalar(unsigned int internalFormat,
		unsigned int format,
		unsigned int type,
		int textureSize[3],
		int componentSize);

	void SlabsFromDatasetToIndex(int volNumber,
		double slabsDataSet[6],
		double slabsPoints[6]);

	void SlabsFromIndexToDataset(double slabsPoints[6],
		double slabsDataSet[6]);

	const char *GetEnabledString(unsigned char value);
	void GetOpenGLState();

	void DebugDisplayBox(vtkPolyData *box);

	void UpdateNoiseTexture();

	
	/** Compute how each axis of a cell is projected on the viewport in pixel.
	* This requires to have information about the camera and about the volume.
	* It set the value of IgnoreSampleDistancePerPixel to true in case of
	* degenerated case (axes aligned with the view)
	*/
	double ComputeMinimalSampleDistancePerPixel(vtkRenderer *renderer,
		vtkVolume *volume);


	/** Return how much the dataset has to be reduced in each dimension to
	* fit on the GPU. If the value is 1.0, there is no need to reduce the
	* dataset.
	* \pre the calling thread has a current OpenGL context.
	* \pre mapper_supported: IsRenderSupported(renderer->GetRenderWindow(),0)
	* The computation is based on hardware limits (3D texture indexable size)
	* and MaxMemoryInBytes.
	*/
	virtual void GetReductionRatio(double ratio[3]);

	int NumberOfCroppingRegions;

	//! World coordinates of each corner of the dataset.
	double BoundingBox[8][3];

	//! Used during the clipping process.
	vtkPolyData *PolyDataBoundingBox;
	vtkPlaneCollection *Planes;
	vtkPlane *NearPlane;

	vtkClipConvexPolyData *Clip;
	vtkMatrix4x4 *InvVolumeMatrix;

	vtkDensifyPolyData *Densify;

	int OpenGLObjectsCreated;
	int NumberOfFrameBuffers;

	unsigned int FrameBufferObject;
	unsigned int DepthRenderBufferObject;

	//! 3D scalar texture +1D color+1D opacity+2D grabbed depth buffer +1 2D colorbuffer.
	unsigned int TextureObjects[5];
	//! used in MIP Mode (2 needed for ping-pong technique)
	unsigned int MaxValueFrameBuffer;
	unsigned int MaxValueFrameBuffer2;
	int ReducedSize[2];

	vtkPolyData *ClippedBoundingBox;

	int LastSize[2];

	double ReductionFactor;

	//! Supported extensions List of unsupported required extensions.
	vtkUnsupportedRequiredExtensionsStringStream *UnsupportedRequiredExtensions;
	int LoadExtensionsSucceeded;

	int Supports_GL_ARB_texture_float;
	int SupportsPixelBufferObjects;

	vtkTimeStamp DataBufferTime;

	//! Matrices used in internal computation. As a member variable, only one memory allocation is performed.
	vtkMatrix4x4 *TempMatrix[3];

	double TableRange[2];

	int ErrorLine;
	int ErrorColumn;
	char *ErrorString;

	//! Store the last projection an raycast method in order to not rebuild the fragment code at every call.
	int LastParallelProjection;
	int LastRayCastMethod;
	int LastCroppingMode;
	int LastComponent;
	int LastShade;

	vtkImageData *SmallInput;
	vtkTimeStamp SmallInputBuildTime;


	//! Build the fragment shader program that scale and bias a texture for window/level purpose.
	void BuildScaleBiasProgram(vtkRenderWindow *w);
	unsigned int SavedFrameBuffer; // some offscreen mode use a framebuffer too.

	vtkTessellatedBoxSource *BoxSource;

	float *NoiseTexture;
	int NoiseTextureSize; //! size of one dimension of the noisetexture
	unsigned int NoiseTextureId; 

	bool IgnoreSampleDistancePerPixel;

	vtkMapDataArrayTextureId *ScalarsTextures; 
	vtkMapMaskTextureId *MaskTextures; 

	vtkRGBTable *RGBTable;


	vtkRGBTable *Mask1RGBTable;
	vtkRGBTable *Mask2RGBTable;

	vtkOpacityTables *OpacityTables;

	vtkKWScalarField *CurrentScalar;

	vtkKWMask *CurrentMask;



	float ActualSampleDistance;

	double LastProgressEventTime; 

	bool PreserveOrientation;

	vtkShaderProgram2 *Program;
	vtkShader2 *Main;
	vtkShader2 *Projection;
	vtkShader2 *Trace;
	vtkShader2 *CroppingShader;
	vtkShader2 *Component;
	vtkShader2 *Shade;



	//! Internal Variable used to keep track of whether or render window's size hanged and therefore we need re-allocation.
	bool        SizeChanged;

	vtkShaderProgram2 *ScaleBiasProgram;

private:
	VolumeRayCastMapperOpenGL(const VolumeRayCastMapperOpenGL&);  // Not implemented.
	void operator=(const VolumeRayCastMapperOpenGL&);  // Not implemented.
};

