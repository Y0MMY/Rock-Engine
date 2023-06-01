#include "pch.h"
#include "Application.h"

#include <GLAD/glad.h>

#include <RockEngine/Renderer/Renderer.h>
#include "RockEngine/Renderer/Framebuffer.h"

#include "ImGui/imgui.h"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>

namespace RockEngine
{
	using LoadingFlags = ApplicationSpecification::ApplicationLoadFlags;

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		s_Instance = this;

		WindowSpecification windowSpec;
		windowSpec.Title = specification.Name;
		windowSpec.Width = specification.WindowWidth; 
		windowSpec.Height = specification.WindowHeight;
		windowSpec.Decorated = (specification.LoadFlags & LoadingFlags::WindowDecorated);
		m_Window = Ref<Window>( Window::Create(windowSpec));
		m_Window->Init();
		m_Window->SetEventCallback(BIND_FN(OnEvent));

		if (specification.LoadFlags & LoadingFlags::EnableImGui)
		{
			m_ImGuiLayer = new ImGuiLayer("ImGuiLayer");
			PushLayer(m_ImGuiLayer);
		}
		
		Renderer::Init(); // TODO: This should be removed in launcher mode. Tempory we have a assets files in launcher's dirictory 
		Renderer::WaitAndRender();

		if (specification.LoadFlags & LoadingFlags::StartMaximized)
			m_Window->Maximize();
		else m_Window->CenterWindow();

		m_Window->SetResizeble(!(LoadingFlags::WindowResizeble & specification.LoadFlags));
	}

	Application::~Application()
	{
		for (Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
			delete layer;
		}

		FramebufferPool::GetGlobal()->GetAll().clear();
		Renderer::WaitAndRender();
		Renderer::Shutdown();
	}

	void Application::Run()
	{
		OnInit();
		while (m_Running)
		{
			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(m_TimeStep);

				Application* app = this;
				if(m_Specification.LoadFlags & LoadingFlags::EnableImGui)
					Renderer::Submit([app]() { app->RenderImGui(); });

				Renderer::WaitAndRender();
			}
			m_Window->OnUpdate();

			float time = (float)glfwGetTime();
			m_TimeStep = time - m_LastFrameTime;
			m_LastFrameTime = time;
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
		/*ImGui::Begin("Renderer");
		auto& caps = RendererAPI::GetCapabilities();
		ImGui::Text("Vendor: %s", caps.Vendor.c_str());
		ImGui::Text("Renderer: %s", caps.Renderer.c_str());
		ImGui::Text("Version: %s", caps.Version.c_str());
		ImGui::Text("Frame Time: %.2fms\n", m_TimeStep.GetMilliseconds());
		ImGui::End();*/

		for (Layer* layer : m_LayerStack)
			layer->OnImGuiRender();

		m_ImGuiLayer->End();
	}

	//---------------------------

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_FN(OnWindowResize));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_FN(OnMouseMoved));

		for (Layer* layer : m_LayerStack)
		{
			layer->OnEvent(event);
			if (event.m_Handled) break;
		}
	}

	bool Application::OnMouseMoved(MouseMovedEvent& e)
	{
		return true;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		int width = e.GetWidth(), height = e.GetHeight();
		if (width == 0 || height == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		Renderer::Submit([=]() { glViewport(0, 0, width, height); });

		auto& fbs = FramebufferPool::GetGlobal()->GetAll();
		for (auto& fb : fbs)
		{
			if (!fb->GetSpecification().NoResize)
				fb->Resize(width, height);
		}

		return false;
	}

	const char* Application::GetConfigurationName()
	{
#if defined(RE_DEBUG)
		return "Debug";
#elif defined(RE_RELEASE)
		return "Release";
#else
#error Undefined configuration?
#endif
	}

	const char* Application::GetPlatformName()
	{
#if defined(RE_PLATFORM_WINDOWS)
		return "Windows x64";
#else
#error Undefined platform?
#endif
	}

}