#include "stdafx.h"
#include "Scene.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace MovementApp;

//////////////////////////////////////////////////////////////////////////


Scene::Scene()
	: m_uLightPosition(-1)
{
}

Scene::~Scene()
{
}

bool Scene::initialize(GLfloat aspectRatio, const OpenGLInfo& openGlInfo)
{
	// Initial scale factor for the camera.
	const GLfloat CameraScaleFactor = 1.0f;

#if 0
	m_spCamera = std::make_unique<Camera>(aspectRatio, 1.0f);
#else
	m_spCamera = std::make_unique<Camera>(
		glm::vec3(0.0f, 0.0f, 2.5f),
		aspectRatio, CameraScaleFactor, openGlInfo.FieldOfView, openGlInfo.FrustumNear, openGlInfo.FrustumFar);
#endif

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

	// Add light source.
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	m_lightPosition = glm::vec4(0.0f, 0.9f, 0.0f, 0.0f);
	GLfloat sideLen = 0.2f;
	m_spLight = std::make_unique<LightSourceVisible>(*m_spCamera, m_lightPosition, sideLen, lightColor);

	// Add cubes.

	glm::vec3 cubePos1(0.0f, 0.1f, 0.0f);
	MaterialPhong cubeMat1({ 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 32.0f);
	m_spCube1 = std::make_unique<Cube>(m_spProgram->getProgram(), *m_spCamera, cubePos1, 0.5f, cubeMat1);

	glm::vec3 cubePos2(0.6f, 0.1f, 0.0f);
	MaterialPhong cubeMat2({ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, 7.0f);
	m_spCube2 = std::make_unique<Cube>(m_spProgram->getProgram(), *m_spCamera, cubePos2, 0.3f, cubeMat2);

	// Add horizontal plane.
	glm::vec3 planeCenter(0.2f, -0.2f, 0.0f);
	MaterialPhong planeMat({ 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 32.0f);
	m_spPlane = std::make_unique<PlaneHorizontal>(m_spProgram->getProgram(), *m_spCamera, planeCenter, 1.0f, planeMat);

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

	//glm::vec4 lightPos(0.0f, 5.0f, 0.0f, 0.0f);

	m_uLightPosition = glGetUniformLocation(program, "Light.position");
	if (-1 == m_uLightPosition)
	{
		std::wcerr << "Failed to find the \"Light.position\" uniform\n";
		assert(false); return false;
	}
	glUniform4fv(m_uLightPosition, 1, glm::value_ptr(m_lightPosition));

	glUseProgram(0);

	if (!m_spProgram->validate())
	{
		std::wcerr << L"GLSL program validation failed\n";
		assert(false); return false;
	}

	return true;
}

void Scene::updateMatrices() const
{
	m_spLight->updateMatrices();

	m_spCube1->updateMatrices();
	m_spCube2->updateMatrices();
	
	m_spPlane->updateMatrices();
}

void Scene::translateCameraX(GLfloat diff)
{
	m_spCamera->translateX(diff);

	updateMatrices();
}

void Scene::translateCameraY(GLfloat diff)
{
	m_spCamera->translateY(diff);

	updateMatrices();
}

void Scene::translateCameraZ(GLfloat diff)
{
	m_spCamera->translateZ(diff);

	updateMatrices();
}

void Scene::rotateCameraX(GLfloat angleDegrees)
{
	m_spCamera->rotateX(angleDegrees);

	updateMatrices();
}

void Scene::rotateCameraY(GLfloat angleDegrees)
{
	m_spCamera->rotateY(angleDegrees);

	updateMatrices();
}

#if 1
void Scene::rotateCameraZ(GLfloat angleDegrees)
{
	m_spCamera->rotateZ(angleDegrees);

	updateMatrices();
}
#endif

#if 0
void Scene::rotateCameraXY(GLfloat xAngleDegrees, GLfloat yAngleDegrees)
{
	m_spCamera->rotateXY(xAngleDegrees, yAngleDegrees);

	updateMatrices();
}
#endif

#if 0
GLfloat Scene::getCameraScale() const
{
	return m_spCamera->getScale();
}
#endif

#if 1
void Scene::scaleCamera(GLfloat amount)
{
	m_spCamera->scale(amount);

	updateMatrices();
}
#endif

void Scene::resize(GLfloat aspectRatio)
{
	m_spCamera->resize(aspectRatio);

	updateMatrices();
}

void Scene::translateCube1(const glm::vec3& diff)
{
	m_spCube1->translate(diff);
}

void Scene::rotateXCube1(GLfloat degrees)
{
	m_spCube1->rotateX(degrees);
}

void Scene::scaleCube1(GLfloat amount)
{
	m_spCube1->scale(amount);
}

void Scene::translateCube2(const glm::vec3& diff)
{
	m_spCube2->translate(diff);
}

void Scene::rotateXCube2(GLfloat degrees)
{
	m_spCube2->rotateX(degrees);
}

void Scene::scaleCube2(GLfloat amount)
{
	m_spCube2->scale(amount);
}

void Scene::translateLight(const glm::vec3& diff)
{
	m_spLight->translate(diff);

	glUniform4fv(m_uLightPosition, 1, glm::value_ptr(m_spLight->getPosition()));
}

void Scene::render() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_spLight->render();

	m_spCube1->render();
	m_spCube2->render();

	m_spPlane->render();
}
