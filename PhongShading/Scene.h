#pragma once


namespace PhongShadingApp
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
		void updateViewMatrices() const;

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

		void rotateCameraXY(GLfloat xAngleDegrees, GLfloat yAngleDegrees);

		GLfloat getCameraScale() const;

		void scaleCamera(GLfloat amount);

	private:
		std::unique_ptr<CommonLibOgl::ProgramGLSL> m_spProgram;

		// OpenGL camera.
		std::unique_ptr<CommonLibOgl::Camera> m_spCamera;

#if 0
		GLuint m_vao;
		GLuint m_vbo;

		GLuint m_index;          // index buffer
		GLsizei m_indexCount;    // number of indices

		GLuint m_normals;
#else

		std::vector< std::unique_ptr<CommonLibOgl::Renderable> > m_contents;

		//std::unique_ptr<CommonLibOgl::Cube> m_spCube;
#endif

		// TODO: uncomment
#if 0
		std::unique_ptr<CommonLibOgl::LightSourceVisible> m_spLight;
#endif
	};
}
