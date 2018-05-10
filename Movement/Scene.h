#pragma once


namespace MovementApp
{
	// OpenGL scene.
	class Scene
	{
	public:
		Scene();

		virtual ~Scene();

		// Parameters: aspectRatio - aspect ratio of the window's client area;
		//             openGlInfo - OpenGL settings.
		bool initialize(GLfloat aspectRatio, const CommonLibOgl::OpenGLInfo& openGlInfo);

		// Update Model-View-Projection (MVP) and other matrices in the GLSL program.
		void updateMatrices() const;

		void resize(GLfloat aspectRatio);

		void render() const;

		//////////////////////////////////////////////////////////////////////////
		// Camera control.
		//////////////////////////////////////////////////////////////////////////

		void translateCameraX(GLfloat diff);
		void translateCameraY(GLfloat diff);
		void translateCameraZ(GLfloat diff);

		void rotateCameraX(GLfloat angleDegrees);
		void rotateCameraY(GLfloat angleDegrees);
		void rotateCameraZ(GLfloat angleDegrees);

		GLfloat getCameraScale() const;

		void scaleCamera(GLfloat amount);

		//////////////////////////////////////////////////////////////////////////
		// Scene content manipulation.
		//////////////////////////////////////////////////////////////////////////

		void translateCube1(const glm::vec3& diff);

		void rotateXCube1(GLfloat degrees);

		void scaleCube1(GLfloat amount);

		void translateCube2(const glm::vec3& diff);

		void rotateXCube2(GLfloat degrees);

		void scaleCube2(GLfloat amount);

		void translateLight(const glm::vec3& diff);

	private:
		std::unique_ptr<CommonLibOgl::ProgramGLSL> m_spProgram;

		// OpenGL camera.
		std::unique_ptr<CommonLibOgl::Camera> m_spCamera;

		// Cubes.
		std::unique_ptr<CommonLibOgl::Cube> m_spCube1;
		std::unique_ptr<CommonLibOgl::Cube> m_spCube2;

		// Horizontal plane.
		std::unique_ptr<CommonLibOgl::PlaneHorizontal> m_spPlane;

		// Visible light source.
		std::unique_ptr<CommonLibOgl::LightSourceVisible> m_spLight;

		glm::vec4 m_lightPosition;

		GLint m_uLightPosition;
	};
}
