#pragma once

#include <string>

#include "RockEngine/Core/Core.h"
#include "RockEngine/Core/LayerStack.h"
#include <RockEngine/Core/Window.h>

namespace RockEngine
{
	struct ApplicationProps {
		uint32_t WindowWidth, WindowHeight;
		std::string Name;
	};

	class Application
	{
	public:
		Application(const ApplicationProps& props);
		virtual ~Application();

		virtual void OnInit(){}
		virtual void OnShutdown(){}
		virtual void OnUpdate(){}

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		virtual void Run();
	private:

		std::unique_ptr<Window> m_Window;

		LayerStack m_LayerStack;
		bool m_Running = true;
	};

	//Iml. by client
	Application* CreateApplication();
}