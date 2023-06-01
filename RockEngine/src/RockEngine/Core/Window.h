#pragma once

#include <functional>

#include "RockEngine/Core/Core.h"
#include "RockEngine/Core/Events/Event.h"

namespace RockEngine
{
	struct WindowSpecification {
		std::string Title = "TheRock";
		uint32_t Width = 1600, Height = 900;
		bool Decorated = true;
	};

	class Window : public RefCounted
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window(){}
		virtual void Init() = 0;
		virtual void OnUpdate() = 0;

		virtual void Maximize() = 0;
		virtual void CenterWindow() = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void SetResizeble(bool resizeble) = 0;

		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;

		virtual const std::string& GetTitle() const = 0;
		virtual void SetTitle(const std::string& title) = 0;

		inline virtual void* GetNativeWindow() const = 0;

		// Instance of window
		static Window* Create(const WindowSpecification& props = WindowSpecification());
	};
}