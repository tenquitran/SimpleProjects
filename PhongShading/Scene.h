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

#if 1
		void rotateCameraZ(GLfloat angleDegrees);
#endif

#if 0
		void rotateCameraXY(GLfloat xAngleDegrees, GLfloat yAngleDegrees);
#endif

#if 0
		GLfloat getCameraScale() const;
#endif

#if 1
		void scaleCamera(GLfloat amount);
#endif

	private:
		std::unique_ptr<CommonLibOgl::ProgramGLSL> m_spProgram;

		// OpenGL camera.
		std::unique_ptr<CommonLibOgl::Camera> m_spCamera;

		std::unique_ptr<CommonLibOgl::Cube> m_spCube;

		// TODO: temp
		std::unique_ptr<CommonLibOgl::PlaneHorizontal> m_spPlane;
	};
}
