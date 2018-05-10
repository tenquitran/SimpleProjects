#include "stdafx.h"
#include "MainWindow.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace MovementApp;

//////////////////////////////////////////////////////////////////////////


MainWindow::MainWindow(HINSTANCE hInstance, const WindowInfo& wndInfo, const OpenGLInfo& openGLInfo)
	: WindowBase(hInstance, wndInfo, openGLInfo)
{
}

MainWindow::~MainWindow()
{
}

LRESULT MainWindow::windowProcDerived(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, aboutProc);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_KEYDOWN:
		{
			int key = (int)wParam;
			//LPARAM keyData = lParam;

			switch (key)
			{
			case VK_ESCAPE:
				DestroyWindow(hWnd);
				break;
			//////////////////////////////////////////////////////////////////////////
			// Translate the camera.
			//////////////////////////////////////////////////////////////////////////
			case 0x57:    // W key
				m_scene.translateCameraY(0.1f);
				break;
			case 0x53:    // S key
				m_scene.translateCameraY(-0.1f);
				break;
			case 0x41:    // A key
				m_scene.translateCameraX(-0.1f);
				break;
			case 0x44:    // D key
				m_scene.translateCameraX(0.1f);
				break;
			//////////////////////////////////////////////////////////////////////////
			// Rotate the camera.
			//////////////////////////////////////////////////////////////////////////
			case VK_UP:
				m_scene.rotateCameraX(2.0f);
				break;
			case VK_DOWN:
				m_scene.rotateCameraX(-2.0f);
				break;
			case VK_LEFT:
				m_scene.rotateCameraY(-2.0f);
				break;
			case VK_RIGHT:
				m_scene.rotateCameraY(2.0f);
				break;
			//////////////////////////////////////////////////////////////////////////
			// Zoom in/out for the camera.
			//////////////////////////////////////////////////////////////////////////
			case VK_NUMPAD0:
				m_scene.scaleCamera(-0.05f);
				break;
			case VK_NUMPAD1:
				m_scene.scaleCamera(0.05f);
				break;
			//////////////////////////////////////////////////////////////////////////
			// Translate the scene contents.
			//////////////////////////////////////////////////////////////////////////
			case 0x42:    // B key
				m_scene.translateCube1(glm::vec3(-0.1f, 0.0f, 0.0f));    // backward
				break;
			case 0x43:    // C key
				m_scene.translateCube1(glm::vec3(0.1f, 0.0f, 0.0f));     // forward
				break;
			case 0x46:    // F key
				m_scene.translateCube2(glm::vec3(-0.1f, 0.0f, 0.0f));    // backward
				break;
			case 0x47:    // G key
				m_scene.translateCube2(glm::vec3(0.1f, 0.0f, 0.0f));    // backward
				break;
			case 0x48:    // H key
				m_scene.rotateXCube1(2.0f);    // rotation around X
				break;
			case 0x4A:    // J key
				m_scene.rotateXCube2(2.0f);    // rotation around X
				break;
			case 0x4B:    // K key
				m_scene.scaleCube1(-0.01f);    // downscale
				break;
			case 0x4C:    // L key
				m_scene.scaleCube1(0.01f);    // upscale
				break;
			case 0x4D:    // M key
				m_scene.scaleCube2(-0.01f);    // downscale
				break;
			case 0x4E:    // N key
				m_scene.scaleCube2(0.01f);    // upscale
				break;
			case 0x59:    // Y key
				m_scene.translateLight(glm::vec3(-0.1f, 0.0f, 0.0f));    // backward
				break;
			case 0x5A:    // Z key
				m_scene.translateLight(glm::vec3(0.1f, 0.0f, 0.0f));    // forward
				break;
			}
		}
		break;
	}

	return 0;
}

bool MainWindow::initialize()
{
	if (!m_hRC)
	{
		std::wcerr << L"No OpenGL rendering context\n";
		assert(false); return false;
	}

	GLfloat aspectRatio = m_wndInfo.m_clientWidth / (GLfloat)m_wndInfo.m_clientHeight;

	return m_scene.initialize(aspectRatio, m_openGlInfo);
}

void MainWindow::onResizeDerived(int clientWidth, int clientHeight)
{
	GLfloat aspectRatio = clientWidth / (GLfloat)clientHeight;

	m_scene.resize(aspectRatio);
}

void MainWindow::render() const
{
	m_scene.render();

	SwapBuffers(GetDC(m_hWnd));
}
