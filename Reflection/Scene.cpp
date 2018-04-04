#include "stdafx.h"
#include "Scene.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace PhongShadingApp;

//////////////////////////////////////////////////////////////////////////


Scene::Scene()
	: m_cubemapTexture{}
{
}

Scene::~Scene()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	if (0 != m_cubemapTexture)
	{
		glDeleteTextures(1, &m_cubemapTexture);
	}
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
		{ GL_VERTEX_SHADER,   "shaders\\reflection.vert" },
		{ GL_FRAGMENT_SHADER, "shaders\\reflection.frag" }
	};

	m_spProgram = std::make_unique<ProgramGLSL>(shaders);

	// Add cube.
	MaterialPhong cubeMat({0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, 32.0f);
	m_spCube = std::make_unique<Cube>(m_spProgram->getProgram(), *m_spCamera, 1.0f, cubeMat);

#if 0
	// Set up texture coordinates buffer for the cube.

	float texCoords[] = {
		// Front
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Right
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Back
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Left
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Bottom
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Top
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f };

	glGenBuffers(1, &m_cubeTexCoords);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeTexCoords);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _countof(texCoords) * sizeof(texCoords[0]), texCoords, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(2);
#endif

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

	if (!loadCubemapTextures())
	{
		std::wcerr << L"Failed to load skybox textures\n";
		assert(false); return false;
	}

	if (!m_spProgram->validate())
	{
		std::wcerr << L"GLSL program validation failed\n";
		assert(false); return false;
	}

	return true;
}

bool Scene::loadCubemapTextures()
{
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &m_cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);

	GLuint targets[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

	std::string cubemapFiles[] = { "posx.png", "negx.png", "posy.png", "negy.png", "posz.png", "negz.png" };

	const std::string folder = "data";

	auto del = [](unsigned char* pBuff) {
		SOIL_free_image_data(pBuff);
	};

	for (int i = 0; i < 6; ++i)
	{
		int width = {}, height = {};

		std::string filePath = folder + "/" + cubemapFiles[i];

		std::unique_ptr<unsigned char[], decltype(del)> spData(
			SOIL_load_image(filePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB));
		if (!spData)
		{
			std::wcerr << L"Failed to load texture " << filePath.c_str() << ": " << SOIL_last_result() << '\n';
			assert(false); return false;
		}

		glTexImage2D(targets[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, spData.get());
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glUseProgram(m_spProgram->getProgram());

	glUniform1i(3, 0);

	glUseProgram(0);

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
