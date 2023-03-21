#pragma once

#include <functional>

#include "RockEngine/Core/Core.h"
#include "RockEngine/Core/Events/Event.h"

namespace RockEngine
{
	struct WindowProps{
		std::string Title;
		uint32_t Width, Height;

		WindowProps(const std::string& title = "Window", unsigned int width = 900, unsigned int height = 900)
			: Title(title), Width(width), Height(height) {}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window(){}

		virtual void OnUpdate() = 0;

		// Windows attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;

		inline virtual void* GetNativeWindow() = 0;

		// Instance of window
		static Window* Create(const WindowProps& props = WindowProps());
	};
}