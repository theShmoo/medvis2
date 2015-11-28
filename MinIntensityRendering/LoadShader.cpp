#include "LoadShader.h"


LoadShader::LoadShader()
{
}


LoadShader::~LoadShader()
{
}

/*!
Laedt ein ShaderProgram mit Vertex und Fragment Shader.
*/
vtkShaderProgram2* LoadShader::loadShaders(std::string vertexPath, std::string fragmentPath, vtkRenderWindow* window, bool messages)
{
	vtkSmartPointer<vtkShaderProgram2> program = vtkShaderProgram2::New();
	program->SetContext(window);

	std::string vertexCode, fragmentCode;

	vertexCode = getShadercode(vertexPath, messages);
	fragmentCode = getShadercode(fragmentPath, messages);

	vtkSmartPointer<vtkShader2> vertexShader = getShader(vertexCode, program, VTK_SHADER_TYPE_VERTEX, messages);
	vtkSmartPointer<vtkShader2> fragmentShader = getShader(fragmentCode, program, VTK_SHADER_TYPE_FRAGMENT, messages);

	if (vertexShader == nullptr)
	{
		if (messages)
			cout << "Unable to create Shaderprogram (source: Vertex Shader)" << std::endl;

		return nullptr;
	}
	if (fragmentShader == nullptr)
	{
		if (messages)
			cout << "Unable to create Shaderprogram (source: Fragment Shader)" << std::endl;

		return nullptr;
	}

	program->GetShaders()->AddItem(vertexShader);
	program->GetShaders()->AddItem(fragmentShader);

	return program;
}
/*!
Laedt ein ShaderProgram mit Vertex, Geometry und Fragment Shader.
*/
vtkShaderProgram2* LoadShader::loadShaders(std::string vertexPath, std::string geometryPath, std::string fragmentPath, vtkRenderWindow* window, bool messages)
{
	vtkSmartPointer<vtkShaderProgram2> program = loadShaders(vertexPath, fragmentPath, window, messages);

	if (program == nullptr)
		return nullptr;

	std::string geometryCode;

	geometryCode = getShadercode(geometryPath, messages);

	vtkSmartPointer<vtkShader2> geometryShader = getShader(geometryCode, program, VTK_SHADER_TYPE_GEOMETRY, messages);
	if (geometryShader == nullptr)
	{
		if (messages)
			cout << "Unable to create Shaderprogram (source: Geometry Shader)" << std::endl;

		return nullptr;
	}

	program->GetShaders()->AddItem(geometryShader);

	return program;
}

/*!
Gibt den in einen File enthaltenen Code zurueck.
*/
const char* LoadShader::getShadercode(std::string path, bool messages)
{
	std::ifstream file(path);
	std::string str;
	std::string file_contents;

	if (file.is_open())
	{
		while (std::getline(file, str))
		{
			file_contents += str;
			file_contents.push_back('\n');
		}
		file.close();

		return file_contents.c_str();
	}
	else{
		if (messages)
		{
			cout << "Unable to open file: " << path << std::endl;
		}
		return "";
	}
}
/*!
Erzeugt einen Shader und gibt diesen zurueck.
*/
vtkShader2* LoadShader::getShader(std::string shaderCode, vtkShaderProgram2* program, enum vtkShader2Type type, bool messages)
{
	if (shaderCode == "")
	{
		if (messages)
		{
			if (type == VTK_SHADER_TYPE_VERTEX)
				cout << "Unable to create Vertexshader" << type << std::endl;
			if (type == VTK_SHADER_TYPE_GEOMETRY)
				cout << "Unable to create Geometryshader" << type << std::endl;
			if (type == VTK_SHADER_TYPE_FRAGMENT)
				cout << "Unable to create Fragmentshader" << type << std::endl;
		}
		return nullptr;
	}

	vtkSmartPointer<vtkShader2> shader = vtkShader2::New();
	shader->SetType(type);
	shader->SetSourceCode(shaderCode.c_str());

	shader->SetContext(program->GetContext());

	return shader;
}



// Vorlage
/*
vtkSmartPointer<vtkConeSource> cone = vtkConeSource::New();
vtkSmartPointer<vtkPolyDataMapper> coneMapper = vtkPolyDataMapper::New();
coneMapper->SetInputConnection(cone->GetOutputPort());
vtkSmartPointer<vtkActor> coneActor = vtkActor::New();
coneActor->SetMapper(coneMapper);
vtkSmartPointer<vtkRenderer> ren = vtkRenderer::New();
ren->AddActor(coneActor);

vtkSmartPointer<vtkRenderWindow> renWin = vtkRenderWindow::New();
renWin->AddRenderer(ren);
const char* frag = "void propFuncFS(void){ gl_FragColor = vec4(255,0,0,1);}";

vtkSmartPointer<vtkShaderProgram2> pgm = vtkShaderProgram2::New();
pgm->SetContext(renWin);

vtkSmartPointer<vtkShader2> shader = vtkShader2::New();
shader->SetType(VTK_SHADER_TYPE_FRAGMENT);
shader->SetSourceCode(frag);
shader->SetContext(pgm->GetContext());

pgm->GetShaders()->AddItem(shader);
vtkSmartPointer<vtkOpenGLProperty> openGLproperty =
static_cast<vtkOpenGLProperty*>(coneActor->GetProperty());
openGLproperty->SetPropProgram(pgm);
openGLproperty->ShadingOn();
int i;
for (i = 0; i < 360; ++i)
{
renWin->Render();
ren->GetActiveCamera()->Azimuth(1);
}*/