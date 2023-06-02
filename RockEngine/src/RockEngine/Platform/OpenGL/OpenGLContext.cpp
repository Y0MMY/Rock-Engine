#include "pch.h"
#include "OpenGLContext.h"

#include "RockEngine/Core/Core.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace RockEngine
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
	}

	OpenGLContext::~OpenGLContext()
	{
	}

	void OpenGLContext::Create()
	{
		RE_CORE_INFO("OpenGLContext::Create");

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		RE_CORE_ASSERT(status, "Failed to initialize Glad!");

		/*RE_CORE_INFO("OpenGL Info:");
		RE_CORE_INFO("  Vendor: {}", glGetString(GL_VENDOR));
		RE_CORE_INFO("  Renderer: {}", glGetString(GL_RENDERER));
		RE_CORE_INFO("  Version: {}", glGetString(GL_VERSION));*/

#ifdef RE_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		RE_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "TheRock requires at least OpenGL version 4.5!");
#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}