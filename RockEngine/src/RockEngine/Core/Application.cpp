#include "pch.h"
#include "Application.h"
#include <RockEngine/Renderer/RendererAPI.h>

namespace RockEngine
{
	Application::Application(const ApplicationProps& props)
	{
		m_Window = std::unique_ptr<Window>( Window::Create({ props.Name, props.WindowWidth, props.WindowHeight }));
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
}