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

		if (!glfwInit())
		{
			RE_CORE_ERROR("Could not intialize GLFW!");
			abort();
		}
		glfwSetErrorCallback(GLFWErrorCallback);
		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
	}
}