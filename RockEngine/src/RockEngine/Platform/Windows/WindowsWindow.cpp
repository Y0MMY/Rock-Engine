#include "pch.h"
#include "WindowsWindow.h"

namespace RockEngine
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		RE_CORE_ERROR("GLFW Error: ({}: {})", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Height = props.Height;
		m_Data.Width = props.Width;

		RE_CORE_TRACE("Creating window: {} - {} - {}", props.Title, props.Width, props.Height);

		if (auto isInit = glfwInit())
		{
			RE_CORE_ASSERT(isInit, "Could not intialize GLFW!");
		}
		glfwSetErrorCallback(GLFWErrorCallback);
		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);

		if (auto isInit = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			RE_CORE_ASSERT(isInit, "Could not intialize Glad!");
		}
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}
}