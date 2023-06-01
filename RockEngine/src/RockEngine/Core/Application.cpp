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

#include "RockEngine/Script/ScriptEngine.h"

namespace RockEngine
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationProps& props)
	{
		s_Instance = this;

		m_Window = Ref<Window>(Window::Create({ props.Name, props.WindowWidth, props.WindowHeight }));
		m_Window->SetEventCallback(BIND_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer("ImGuiLayer");
		PushLayer(m_ImGuiLayer);

		ScriptEngine::Init();
		Renderer::Init();
		Renderer::WaitAndRender();

		m_Window->Maximize();
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
		//PushLayer(object);
		while (m_Running)
		{
			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(m_TimeStep);

				Application* app = this;
				Renderer::Submit([app]() { app->RenderImGui(); });

				Renderer::WaitAndRender();
			}
			m_Window->OnUpdate();

			float time = (float)glfwGetTime();
			m_TimeStep = time - m_LastFrameTime;
			m_LastFrameTime = time;
		}
	}

	std::string Application::OpenFileDialog(const char* filter) const
	{
		OPENFILENAMEA ofn;       // common dialog box structure
		CHAR szFile[260] = { 0 };       // if using TCHAR macros

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)m_Window->GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}

	std::filesystem::path Application::SaveFileDialog(const char* filter) const
	{
		OPENFILENAMEA ofn;       // common dialog box structure
		CHAR szFile[260] = { 0 };       // if using TCHAR macros

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			std::string fp = ofn.lpstrFile;
			std::replace(fp.begin(), fp.end(), '\\', '/');
			return std::filesystem::path(fp);
		}

		return std::filesystem::path();
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
		ImGui::Begin("Renderer");
		auto& caps = RendererAPI::GetCapabilities();
		ImGui::Text("Vendor: %s", caps.Vendor.c_str());
		ImGui::Text("Renderer: %s", caps.Renderer.c_str());
		ImGui::Text("Version: %s", caps.Version.c_str());
		ImGui::Text("Frame Time: %.2fms\n", m_TimeStep.GetMilliseconds());
		ImGui::End();

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