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
		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		uint32_t GetWidth() override { return m_Data.Width; }
		uint32_t GetHeight() override { return m_Data.Height; }

		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void Maximize() override;

		virtual const std::string& GetTitle() const override { return m_Data.Title; }
		virtual void SetTitle(const std::string& title) override;

		inline void* GetNativeWindow() const { return m_Window; }

	private:
		struct WindowData
		{
			std::string Title;
			uint32_t Width;
			uint32_t Height;

			bool VSync = false;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		GLFWwindow* m_Window;
	};
}