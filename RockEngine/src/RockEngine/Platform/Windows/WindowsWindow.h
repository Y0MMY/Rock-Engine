#pragma once

#include <Glad/glad.h>
#include <GLFW/glfw3.h>

#include "RockEngine/Core/Window.h"

namespace RockEngine
{
	class WindowsWindow : public Window {
	public:
		WindowsWindow(const WindowProps& props);

		void OnUpdate() override;

		// Windows attributes
		uint32_t GetWidth() override { return m_Data.Width; }
		uint32_t GetHeight() override { return m_Data.Height; }
	private:
		struct WindowData
		{
			std::string Title;
			uint32_t Width;
			uint32_t Height;

			bool VSync = false;
		};

		WindowData m_Data;
		GLFWwindow* m_Window;
	};
}