#pragma once

#include "vtkImageProcessingPass.h"

class GradientPostprocessFilter : public vtkImageProcessingPass
{
public:
	static GradientPostprocessFilter *New();
	vtkTypeMacro(GradientPostprocessFilter, vtkImageProcessingPass);
	void PrintSelf(ostream& os, vtkIndent indent);

	virtual void Render(const vtkRenderState *s);

	void ReleaseGraphicsResources(vtkWindow *w);

protected:

	GradientPostprocessFilter();
	virtual ~GradientPostprocessFilter();

	vtkFrameBufferObject *FrameBufferObject;
	vtkTextureObject *Pass; 
	vtkTextureObject *passTexture;
	vtkShaderProgram2 *Program; 

private:
	GradientPostprocessFilter(const GradientPostprocessFilter&);  // Not implemented.
	void operator=(const GradientPostprocessFilter&);  // Not implemented.
};
