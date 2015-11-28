#pragma once

#include <vtkSmartPointer.h>
#include <vtkShader2Collection.h>
#include <vtkShader2.h>
#include <vtkShaderProgram2.h>
#include <vtkProperty.h>
#include <iostream>
#include <fstream>
#include <string>

/*! Dies ist eine statische Klasse die, speziell auf vtk ausgerichtet, Shader aus Files lädt und als vtkShaderProgram2 zurueckgibt.
*/
class LoadShader
{
public:
	//! Methode die ein ShaderProgram mit einen Vertex und einen Fragmentshader aus den gegeben Pfaden erzeugt.
	/*!
	\param vertexPath der Pfad zur Vertexshader-datei.
	\param fragmentPath der Pfad zur Fragmentshader-datei.
	\param window ein Pointer zum Ausgabefenster zu dem das ShaderProgram eine Beziehung haben soll.
	\param messages gibt an, ob Ausgabenachrichten geschrieben werden sollen (optional. default = false).
	\return das ShaderProgram.
	*/
	static vtkShaderProgram2* loadShaders(std::string vertexPath, std::string fragmentPath, vtkRenderWindow* window, bool messages = false);

	//! Methode die ein ShaderProgram mit einen Vertex, eine Geometry und einen Fragmentshader aus den gegeben Pfaden erzeugt.
	/*!
	\param vertexPath der Pfad zur Vertexshader-datei.
	\param geometryPath der Pfad zur Geometryshader-datei.
	\param fragmentPath der Pfad zur Fragmentshader-datei.
	\param window ein Pointer zum Ausgabefenster zu dem das ShaderProgram eine Beziehung haben soll.
	\param messages gibt an, ob Ausgabenachrichten geschrieben werden sollen (optional. default = false).
	\return das ShaderProgram.
	*/
	static vtkShaderProgram2* loadShaders(std::string vertexPath, std::string geometryPath, std::string fragmentPath, vtkRenderWindow* window, bool messages = false);

private:

	//! Konstruktor wird nicht benoetigt da die Klasse static ist
	LoadShader();
	//! Destruktor wird nicht benoetigt da die Klasse static ist
	~LoadShader();

	//! Laedt den ShaderCode aus der angegeben Datei und gibt diesen als char Pointer zurueck.
	/*!
	\param path der Pfad zum File.
	\param messages gibt an, ob Ausgabenachrichten geschrieben werden sollen (optional. default = false).
	\return den ShaderCode als Char Pointer.
	*/
	static const char* getShadercode(std::string path, bool messages = false);

	//! Erzeugt einen Shader eines uebergebenen Typs mit uebergebenen Code als string und gibt diesen zurueck.
	/*!
	\param shaderCode der Code des Shaders als string.
	\param program das ShaderProgram zu welchen der Shader eine Beziehung haben soll.
	\param type der Shadertyp (VTK_SHADER_TYPE_XXX, wobei XXX fuer "VERTEX","GEOMETRY" oder "FRAGMENT" steht).
	\param messages gibt an, ob Ausgabenachrichten geschrieben werden sollen (optional. default = false).
	\return den erzeugten Shader
	*/
	static vtkShader2* getShader(std::string shaderCode, vtkShaderProgram2* program, enum vtkShader2Type type, bool messages = false);
};

