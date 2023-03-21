#pragma once

#include <string>

#include "RockEngine/Core/Core.h"
#include "RockEngine/Core/LayerStack.h"
#include <RockEngine/Core/Window.h>

#include <RockEngine/ImGui/ImGuiLayer.h>

#include "RockEngine/Core/Events/ApplicationEvent.h"

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

		void OnEvent(Event& event);

		void RenderImGui();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		virtual void Run();

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		LayerStack m_LayerStack;
		bool m_Running = true;
		ImGuiLayer* m_ImGuiLayer;
		static Application* s_Instance;
	};

	//Iml. by client
	Application* CreateApplication();
}