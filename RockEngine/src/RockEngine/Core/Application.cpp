#include "pch.h"
#include "Application.h"
#include "RockEngine/Core/Window.h"

namespace RockEngine
{
	Application::Application(const ApplicationProps& props)
	{
		Window::Create({ props.Name, props.WindowWidth, props.WindowHeight });
	}

	void Application::Run()
	{
		while (1)
		{
			//RE_CORE_TRACE("Hello")
		}
	}
}