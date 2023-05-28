#pragma once

#include <string>

#include "RockEngine/Core/Core.h"
#include "RockEngine/Core/LayerStack.h"
#include <RockEngine/Core/Window.h>

#include <RockEngine/ImGui/ImGuiLayer.h>

#include "RockEngine/Core/Events/ApplicationEvent.h"
#include "RockEngine/Core/Events/MouseEvent.h"

namespace RockEngine
{
	
	struct ApplicationSpecification
	{
		std::string Name = "TheRock";
		uint32_t WindowWidth = 1600, WindowHeight = 900;
		uint32_t LoadFlags = 1;

		enum ApplicationLoadFlags
		{
			None = BIT(0),
			WindowDecorated = BIT(1),
			StartMaximized = BIT(2),
			WindowResizeble = BIT(3),
			EnableImGui = BIT(4)
		};

		enum struct ApplicationMode
		{
			None = 0,
			Debug,
			Runtime,
			Launcher
		} Mode;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& props);
		virtual ~Application();

		virtual void OnInit(){}
		virtual void OnShutdown(){}
		virtual void OnUpdate(Timestep ts){}

		void OnEvent(Event& event);

		void RenderImGui();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		std::string OpenFileDialog(const char* filter = "All\0*.*\0") const;
		std::filesystem::path SaveFileDialog(const char* filter = "All\0*.*\0") const; // TODO: Move to Windows File System

		virtual void Run();

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

		static const char* GetConfigurationName();
		static const char* GetPlatformName();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
	private:
		ApplicationSpecification m_Specification;

		Ref<Window> m_Window;
		LayerStack m_LayerStack;
		bool m_Running = true;
		bool m_Minimized = false;
		ImGuiLayer* m_ImGuiLayer;
		static Application* s_Instance;

		Timestep m_TimeStep;
		float m_LastFrameTime = 0.0f;

	};

	//Iml. by client
	Application* CreateApplication(int argc, char** argv);
}