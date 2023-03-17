#include "pch.h"
#include "Application.h"
#include <RockEngine/Renderer/RendererAPI.h>

namespace RockEngine
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationProps& props)
	{
		s_Instance = this;
		m_Window = std::unique_ptr<Window>( Window::Create({ props.Name, props.WindowWidth, props.WindowHeight }));

		m_ImGuiLayer = new ImGuiLayer("ImGuiLayer");
		PushLayer(m_ImGuiLayer);
	}

	Application::~Application()
	{
		for (Layer* layer : m_LayerStack)
			layer->OnDetach();
	}

	void Application::Run()
	{
		OnInit();
		while (m_Running)
		{
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();
			RenderImGui();
			m_Window->OnUpdate();
			RendererAPI::Clear(0.2f, 0.3f, 0.8f, 1);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
		layer->OnDetach();
	}

	void Application::RenderImGui()
	{
		m_ImGuiLayer->Begin();

		for (Layer* layer : m_LayerStack)
			layer->OnImGuiRender();

		m_ImGuiLayer->End();
	}
}