#include "GradientPostprocessFilter.h"

#include "vtkObjectFactory.h"
#include <cassert>
#include "vtkRenderState.h"
#include "vtkRenderer.h"
#include "vtkgl.h"
#include "vtkFrameBufferObject.h"
#include "vtkTextureObject.h"
#include "vtkShaderProgram2.h"
#include "vtkShader2.h"
#include "vtkShader2Collection.h"
#include "vtkUniformVariables.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtkOpenGLError.h"
#include "vtkTextureUnitManager.h"

vtkStandardNewMacro(GradientPostprocessFilter);

extern const char *PostProcessGradientFilter;

GradientPostprocessFilter::GradientPostprocessFilter()
{
	this->FrameBufferObject = 0;
	this->Pass = 0;
	this->passTexture = 0;
	this->Program = 0;
}


GradientPostprocessFilter::~GradientPostprocessFilter()
{
	if (this->FrameBufferObject != 0)
	{
		vtkErrorMacro(<< "FrameBufferObject should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->Pass != 0)
	{
		vtkErrorMacro(<< "Pass should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->passTexture != 0)
	{
		vtkErrorMacro(<< "passTexture should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->Program != 0)
	{
		this->Program->Delete();
	}
}

void GradientPostprocessFilter::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

void GradientPostprocessFilter::Render(const vtkRenderState *s)
{
	vtkRenderer *renderer = s->GetRenderer();

	vtkSmartPointer<vtkOpenGLRenderWindow> context = vtkOpenGLRenderWindow::SafeDownCast(renderer->GetRenderWindow());

	GLint drawBuffer;
	glGetIntegerv(GL_DRAW_BUFFER, &drawBuffer);

	// 1. Create a new render state with an FBO.

	int winSize[2];
	s->GetWindowSize(winSize);


	if (this->Pass == 0)
	{
		this->Pass = vtkTextureObject::New();
		this->Pass->SetContext(context);
	}

	if (this->FrameBufferObject == 0)
	{
		this->FrameBufferObject = vtkFrameBufferObject::New();
		this->FrameBufferObject->SetContext(context);
	}

	if (this->passTexture == 0)
	{
		this->passTexture = vtkTextureObject::New();
		this->passTexture->SetContext(this->FrameBufferObject->GetContext());
	}

	if (this->passTexture->GetWidth() != static_cast<unsigned int>(winSize[0]) ||
		this->passTexture->GetHeight() != static_cast<unsigned int>(winSize[1]))
	{
		this->passTexture->Create2D(winSize[0], winSize[1], 4, VTK_UNSIGNED_CHAR, false);
	}
	this->FrameBufferObject->SetNumberOfRenderTargets(1);
	this->FrameBufferObject->SetColorBuffer(0, this->passTexture);
	unsigned int* indices = new unsigned int[1];
  indices[0] = 0;
	this->FrameBufferObject->SetActiveBuffers(1, indices);

	this->FrameBufferObject->Start(winSize[0], winSize[1], false);
	
	if (this->Program == 0)
	{
		this->Program = vtkShaderProgram2::New();
		this->Program->SetContext(
			static_cast<vtkOpenGLRenderWindow *>(
			this->FrameBufferObject->GetContext()));
		vtkShader2 *shader = vtkShader2::New();
		shader->SetType(VTK_SHADER_TYPE_FRAGMENT);
		shader->SetSourceCode(PostProcessGradientFilter);
		shader->SetContext(this->Program->GetContext());
		this->Program->GetShaders()->AddItem(shader);
		shader->Delete();
	}

	this->Program->Build();

	if (this->Program->GetLastBuildStatus()
		!= VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
	{
		vtkErrorMacro("Couldn't build the shader program. At this point , it can be an error in a shader or a driver bug.");

		// restore some state.
		this->FrameBufferObject->UnBind();
		glDrawBuffer(drawBuffer);

		return;
	}


	vtkUniformVariables *var = this->Program->GetUniformVariables();
	vtkTextureUnitManager *tu = context->GetTextureUnitManager();

	int sourceId = tu->Allocate();
	vtkgl::ActiveTexture(vtkgl::TEXTURE0 + sourceId);
	this->Pass->Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	var->SetUniformi("tex", 1, &sourceId);

	this->Program->Use();

	this->FrameBufferObject->RenderQuad(0, winSize[0] - 1, 0, winSize[1] - 1);

	this->Pass->UnBind();
	tu->Free(sourceId);

	this->Program->Restore();

	this->FrameBufferObject->UnBind();

	glDrawBuffer(drawBuffer);

}

void GradientPostprocessFilter::ReleaseGraphicsResources(vtkWindow *w)
{
	assert("pre: w_exists" && w != 0);

	this->Superclass::ReleaseGraphicsResources(w);

	if (this->Program != 0)
	{
		this->Program->ReleaseGraphicsResources();
	}


	if (this->FrameBufferObject != 0)
	{
		this->FrameBufferObject->Delete();
		this->FrameBufferObject = 0;
	}

	if (this->Pass != 0)
	{
		this->Pass->Delete();
		this->Pass = 0;
	}

	if (this->passTexture != 0)
	{
		this->passTexture->Delete();
		this->passTexture = 0;
	}

}