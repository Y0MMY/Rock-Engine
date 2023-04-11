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

	class Window : public RefCounted
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window(){}

		virtual void OnUpdate() = 0;

		virtual void Maximize() = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;

		virtual const std::string& GetTitle() const = 0;
		virtual void SetTitle(const std::string& title) = 0;

		inline virtual void* GetNativeWindow() = 0;

		// Instance of window
		static Window* Create(const WindowProps& props = WindowProps());
	};
}