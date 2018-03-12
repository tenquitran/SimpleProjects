#include "stdafx.h"
#include "Scene.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace PhongShadingApp;

//////////////////////////////////////////////////////////////////////////


Scene::Scene()
	: m_vao{}, m_vbo{}, m_index{}, m_indexCount{}, m_normals{}
{
}

Scene::~Scene()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (0 != m_normals)
	{
		glDeleteBuffers(1, &m_normals);
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

	glClearColor(0.8f, 0.93f, 0.96f, 1.0f);    // very light blue
	//glClearColor(0.0f, 0.64f, 0.91f, 1.0f);    // light blue

	// Initialize the program wrapper.

	const ShaderCollection shaders = {
		{ GL_VERTEX_SHADER,   "shaders\\phong.vert" },
		{ GL_FRAGMENT_SHADER, "shaders\\phong.frag" }
	};

	m_spProgram = std::make_unique<ProgramGLSL>(shaders);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// A cube.

	float side = 1.0f;
	float side2 = side / 2.0f;

	GLfloat vertices[] = {
		// Front
		-side2, -side2, side2,
		side2, -side2, side2,
		side2, side2, side2,
		-side2, side2, side2,
		// Right
		side2, -side2, side2,
		side2, -side2, -side2,
		side2, side2, -side2,
		side2, side2, side2,
		// Back
		-side2, -side2, -side2,
		-side2, side2, -side2,
		side2, side2, -side2,
		side2, -side2, -side2,
		// Left
		-side2, -side2, side2,
		-side2, side2, side2,
		-side2, side2, -side2,
		-side2, -side2, -side2,
		// Bottom
		-side2, -side2, side2,
		-side2, -side2, -side2,
		side2, -side2, -side2,
		side2, -side2, side2,
		// Top
		-side2, side2, side2,
		side2, side2, side2,
		side2, side2, -side2,
		-side2, side2, -side2
	};

	// Generate VBO and fill it with the data.

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, _countof(vertices) * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

	// Fill in the vertex position attribute.
	const GLuint attrVertexPosition = 0;
	glVertexAttribPointer(attrVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(attrVertexPosition);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Fill the index buffer.

	GLuint indices[] = {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	m_indexCount = _countof(indices);

	glGenBuffers(1, &m_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _countof(indices) * sizeof(indices[0]), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLfloat normals[] = {
		// Front
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		// Right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		// Back
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		// Left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		// Bottom
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		// Top
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	glGenBuffers(1, &m_normals);
	glBindBuffer(GL_ARRAY_BUFFER, m_normals);
	glBufferData(GL_ARRAY_BUFFER, _countof(normals) * sizeof(normals[0]), normals, GL_STATIC_DRAW);

	// Fill in the normal attribute.
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	glUseProgram(m_spProgram->getProgram());

	// Set light vector.
	glm::vec3 lightVector(0.0f, 0.0f, 1.0f);
	glUniform3fv(1, 1, glm::value_ptr(lightVector));

	// Set light and object colors, respectively.
	glUniform3f(2, 1.0f, 1.0f, 1.0f);
	glUniform3f(3, 1.0f, 0.0f, 0.0f);

#if 0
	// Calculate normal to one of our triangles and pass it to the shaders.

	glm::vec3 vectorOne = vertices[0] - vertices[1];
	glm::vec3 vectorTwo = vertices[2] - vertices[1];

	glm::vec3 normal = glm::cross(vectorOne, vectorTwo);

	glUniform3fv(3, 1, glm::value_ptr(normal));
#endif

	glUseProgram(0);

	if (!m_spProgram->validate())
	{
		std::wcerr << L"GLSL program validation failed\n";
		assert(false); return false;
	}

	// Add light source.
	m_spLight = std::make_unique<LightSourceVisible>(*m_spCamera, 0.05f, glm::vec3(1.0));

	return true;
}

void Scene::updateViewMatrices() const
{
	assert(m_spProgram);

	glUseProgram(m_spProgram->getProgram());

	glm::mat4 mvp = m_spCamera->getModelViewProjectionMatrix();

	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

	glm::mat4 modelView = m_spCamera->getModelViewMatrix();

	glUseProgram(0);
}

void Scene::translateCameraX(GLfloat diff)
{
	m_spCamera->translateX(diff);
}

void Scene::translateCameraY(GLfloat diff)
{
	m_spCamera->translateY(diff);
}

void Scene::translateCameraZ(GLfloat diff)
{
	m_spCamera->translateZ(diff);
}

void Scene::rotateCameraX(GLfloat angleDegrees)
{
	m_spCamera->rotateX(angleDegrees);
}

void Scene::rotateCameraY(GLfloat angleDegrees)
{
	m_spCamera->rotateY(angleDegrees);
}

void Scene::rotateCameraZ(GLfloat angleDegrees)
{
	m_spCamera->rotateZ(angleDegrees);
}

void Scene::rotateCameraXY(GLfloat xAngleDegrees, GLfloat yAngleDegrees)
{
	m_spCamera->rotateXY(xAngleDegrees, yAngleDegrees);
}

GLfloat Scene::getCameraScale() const
{
	return m_spCamera->getScale();
}

void Scene::scaleCamera(GLfloat amount)
{
	m_spCamera->scale(amount);
}

void Scene::resize(GLfloat aspectRatio)
{
	m_spCamera->resize(aspectRatio);
}

void Scene::render() const
{
	updateViewMatrices();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	assert(m_spProgram);

	m_spLight->render();

	glUseProgram(m_spProgram->getProgram());
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}
