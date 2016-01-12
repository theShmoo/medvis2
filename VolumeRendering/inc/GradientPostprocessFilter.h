#pragma once

#include "vtkImageProcessingPass.h"

//! This class is a 2D gradient Filter for post processing (With Opengl shader). Unfortunately it doesent work in this project
class GradientPostprocessFilter : public vtkImageProcessingPass
{
public:
	//! Creats a new Filterobject and returns it
	static GradientPostprocessFilter *New();

	vtkTypeMacro(GradientPostprocessFilter, vtkImageProcessingPass);

	//! Write relevant parameter of this class in the out-console
	void PrintSelf(ostream& os, vtkIndent indent);

	//! Renderfunktion of the filter. In the Renderstate are all relevant information for postprocessrendering.
	virtual void Render(const vtkRenderState *s);

	//! Delete all used Framebuffers and Textures
	void ReleaseGraphicsResources(vtkWindow *w);

protected:

	//! Constuctor of the postprocess gradient filter
	GradientPostprocessFilter();

	//! Destuctor of the postprocess gradient filter
	virtual ~GradientPostprocessFilter();

	//! Framebuffer for save rendering (if this renderpass failed, the original buffer is not corrupted)
	vtkFrameBufferObject *FrameBufferObject;

	//! Texture from the previous Renderpasses
	vtkTextureObject *Pass; 

	//! Texture for the current renderpass
	vtkTextureObject *passTexture;

	//! Shaderprogramm of the gradient filter shaders
	vtkShaderProgram2 *Program; 

private:
	GradientPostprocessFilter(const GradientPostprocessFilter&);  // Not implemented.
	void operator=(const GradientPostprocessFilter&);  // Not implemented.
};
