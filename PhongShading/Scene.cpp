#include "stdafx.h"
#include "Scene.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace PhongShadingApp;

//////////////////////////////////////////////////////////////////////////


Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::initialize(GLfloat aspectRatio, const OpenGLInfo& openGlInfo)
{
	// Initial scale factor for the camera.
	const GLfloat CameraScaleFactor = 1.0f;

	m_spCamera = std::make_unique<Camera>(aspectRatio, CameraScaleFactor,
		openGlInfo.FieldOfView, openGlInfo.FrustumNear, openGlInfo.FrustumFar);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_MULTISAMPLE);

	glClearColor(0.8f, 0.93f, 0.96f, 1.0f);    // very light blue

	// Initialize the program wrapper.

	const ShaderCollection shaders = {
		{ GL_VERTEX_SHADER,   "..\\..\\CommonLibOgl\\CommonLibOgl\\shaders\\phong.vert" },
		{ GL_FRAGMENT_SHADER, "..\\..\\CommonLibOgl\\CommonLibOgl\\shaders\\phong.frag" }
	};

	m_spProgram = std::make_unique<ProgramGLSL>(shaders);

	// Add cube.
	MaterialPhong cubeMat({1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 32.0f);
	m_spCube = std::make_unique<Cube>(m_spProgram->getProgram(), *m_spCamera, 1.0f, cubeMat);

	// Set light properties.

	GLuint program = m_spProgram->getProgram();

	glUseProgram(program);

	glm::vec3 ambient(0.15f, 0.15f, 0.15f);
	glm::vec3 diffuse(0.6f, 0.6f, 0.6f);
	glm::vec3 specular(0.25f, 0.25f, 0.25f);

	GLuint mAmbient = glGetUniformLocation(program, "Light.ambient");
	if (-1 != mAmbient)
	{
		glUniform3fv(mAmbient, 1, glm::value_ptr(ambient));
	}

	GLuint mDiffuse = glGetUniformLocation(program, "Light.diffuse");
	if (-1 != mDiffuse)
	{
		glUniform3fv(mDiffuse, 1, glm::value_ptr(diffuse));
	}

	GLuint mSpecular = glGetUniformLocation(program, "Light.specular");
	if (-1 != mSpecular)
	{
		glUniform3fv(mSpecular, 1, glm::value_ptr(specular));
	}

	glm::vec4 lightPos(0.0f, 5.0f, 0.0f, 0.0f);

	GLuint mPosition = glGetUniformLocation(program, "Light.position");
	if (-1 != mPosition)
	{
		glUniform4fv(mPosition, 1, glm::value_ptr(lightPos));
	}

	glUseProgram(0);

	if (!m_spProgram->validate())
	{
		std::wcerr << L"GLSL program validation failed\n";
		assert(false); return false;
	}

	return true;
}

void Scene::updateViewMatrices() const
{
	m_spCube->updateViewMatrices();
}

void Scene::translateCameraX(GLfloat diff)
{
	m_spCamera->translateX(diff);

	updateViewMatrices();
}

void Scene::translateCameraY(GLfloat diff)
{
	m_spCamera->translateY(diff);

	updateViewMatrices();
}

void Scene::translateCameraZ(GLfloat diff)
{
	m_spCamera->translateZ(diff);

	updateViewMatrices();
}

void Scene::rotateCameraX(GLfloat angleDegrees)
{
	m_spCamera->rotateX(angleDegrees);

	updateViewMatrices();
}

void Scene::rotateCameraY(GLfloat angleDegrees)
{
	m_spCamera->rotateY(angleDegrees);

	updateViewMatrices();
}

void Scene::rotateCameraZ(GLfloat angleDegrees)
{
	m_spCamera->rotateZ(angleDegrees);

	updateViewMatrices();
}

void Scene::rotateCameraXY(GLfloat xAngleDegrees, GLfloat yAngleDegrees)
{
	m_spCamera->rotateXY(xAngleDegrees, yAngleDegrees);

	updateViewMatrices();
}

GLfloat Scene::getCameraScale() const
{
	return m_spCamera->getScale();
}

void Scene::scaleCamera(GLfloat amount)
{
	m_spCamera->scale(amount);

	updateViewMatrices();
}

void Scene::resize(GLfloat aspectRatio)
{
	m_spCamera->resize(aspectRatio);

	updateViewMatrices();
}

void Scene::render() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_spCube->render();
}
