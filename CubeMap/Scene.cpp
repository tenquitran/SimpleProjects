#include "stdafx.h"
#include "Scene.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace CubemapApp;

//////////////////////////////////////////////////////////////////////////


Scene::Scene()
	: m_vao{}, m_vbo{}, m_index{}, m_indexCount{}, m_texture{}
{
}

Scene::~Scene()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	if (0 != m_texture)
	{
		glDeleteTextures(1, &m_texture);
	}

	if (0 != m_index)
	{
		glDeleteBuffers(1, &m_index);
	}

	if (0 != m_vbo)
	{
		glDeleteBuffers(1, &m_vbo);
	}

	if (0 != m_vao)
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_vao);
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
		{ GL_VERTEX_SHADER,   "shaders\\cubemap.vert" },
		{ GL_FRAGMENT_SHADER, "shaders\\cubemap.frag" }
	};

	m_spProgram = std::make_unique<ProgramGLSL>(shaders);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	const float CubemapSide = 2.5f; //1.5f;

	const float HalfSide = CubemapSide / 2.0f;

	float vertices[] = {
		// Front
		-HalfSide, -HalfSide, HalfSide,
		HalfSide, -HalfSide, HalfSide,
		HalfSide, HalfSide, HalfSide,
		-HalfSide, HalfSide, HalfSide,
		// Right
		HalfSide, -HalfSide, HalfSide,
		HalfSide, -HalfSide, -HalfSide,
		HalfSide, HalfSide, -HalfSide,
		HalfSide, HalfSide, HalfSide,
		// Back
		-HalfSide, -HalfSide, -HalfSide,
		-HalfSide, HalfSide, -HalfSide,
		HalfSide, HalfSide, -HalfSide,
		HalfSide, -HalfSide, -HalfSide,
		// Left
		-HalfSide, -HalfSide, HalfSide,
		-HalfSide, HalfSide, HalfSide,
		-HalfSide, HalfSide, -HalfSide,
		-HalfSide, -HalfSide, -HalfSide,
		// Bottom
		-HalfSide, -HalfSide, HalfSide,
		-HalfSide, -HalfSide, -HalfSide,
		HalfSide, -HalfSide, -HalfSide,
		HalfSide, -HalfSide, HalfSide,
		// Top
		-HalfSide, HalfSide, HalfSide,
		HalfSide, HalfSide, HalfSide,
		HalfSide, HalfSide, -HalfSide,
		-HalfSide, HalfSide, -HalfSide
	};

	// Set up the vertex buffer.

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, _countof(vertices) * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

	// Fill in the vertex position attribute.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint indices[] = {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	// Set up the index buffer.

	m_indexCount = _countof(indices);

	glGenBuffers(1, &m_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _countof(indices) * sizeof(indices[0]), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

	// Initialize matrices according to the camera state.
	updateViewMatrices();

	return true;
}

bool Scene::loadCubemapTextures()
{
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

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

	//glUniform1i(2, 0);
	glUniform1i(1, 0);

	glUseProgram(0);

	return true;
}

void Scene::updateViewMatrices() const
{
	assert(m_spProgram->getProgram());

	glUseProgram(m_spProgram->getProgram());

	glm::mat4 view = glm::mat4(glm::mat3(m_spCamera->getViewMatrix()));    // remove translation from the view matrix

#if 1
	glm::mat4 mvp = m_spCamera->getProjectionMatrix() * view * m_spCamera->getModelMatrix();
	//glm::mat4 mvp = m_spCamera->getProjectionMatrix() * view;

	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));
#else
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(m_spCamera->getProjectionMatrix()));
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view));
#endif

	glUseProgram(0);
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

	glDepthFunc(GL_LEQUAL);

	glUseProgram(m_spProgram->getProgram());

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	glDepthFunc(GL_LESS);
}
