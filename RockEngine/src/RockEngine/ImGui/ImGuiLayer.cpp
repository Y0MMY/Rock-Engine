#include "pch.h"
#include "ImGuiLayer.h"

#define IMGUI_IMPL_API
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "RockEngine/Core/Application.h"
#include <GLFW/glfw3.h>

#include "ImGuiFonts.h"

namespace RockEngine {

	ImGuiLayer::ImGuiLayer()
	{

	}

	ImGuiLayer::ImGuiLayer(const std::string& name)
		: Layer(name)
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		UI::Fonts::Add("Bold", "Resources/Fonts/Roboto/Roboto-Bold.ttf", 18.0f);
		UI::Fonts::Add("Large", "Resources/Fonts/Roboto/Roboto-Regular.ttf", 30.0f);
		UI::Fonts::Add("Default", "Resources/Fonts/Roboto/Roboto-SemiMedium.ttf", 15.0f, true);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 430");
	}

	void ImGuiLayer::SetDarkTheme()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// ImGui Colors
		ImVec4* colors = style.Colors;

		// Main
		colors[ImGuiCol_WindowBg] = ImVec4(0.078f, 0.078f, 0.078f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.156f, 0.156f, 0.156f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.086f, 0.086f, 0.086f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.156f, 0.156f, 0.156f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);

		// Headers
		colors[ImGuiCol_Header] = ImVec4(0.180f, 0.180f, 0.180f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.240f, 0.240f, 0.240f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.156f, 0.156f, 0.156f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.400f, 0.400f, 0.400f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.800f, 0.800f, 0.800f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.16f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.780f, 0.780f, 0.780f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.760f, 0.760f, 0.760f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 0.86f);
		colors[ImGuiCol_TabActive] = ImVec4(0.196f, 0.196f, 0.196f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.196f, 0.196f, 0.196f, 1.00f);

		// Buttons
		colors[ImGuiCol_Button] = ImVec4(0.180f, 0.180f, 0.180f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.360f, 0.360f, 0.360f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.200f, 0.200f, 0.200f, 1.00f);

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.260f, 0.260f, 0.260f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.200f, 0.200f, 0.200f, 1.00f);

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4(0.110f, 0.110f, 0.110f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.360f, 0.360f, 0.360f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.200f, 0.200f, 0.200f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.110f, 0.110f, 0.110f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.160f, 0.160f, 0.160f, 1.00f);

		// Title BG
		colors[ImGuiCol_TitleBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.200f, 0.200f, 0.200f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.160f, 0.160f, 0.160f, 1.00f);

		// Windows BG
		colors[ImGuiCol_WindowBg] = ImVec4(0.090f, 0.090f, 0.090f, 1.00f);

		// Separator
		colors[ImGuiCol_Separator] = ImVec4(0.200f, 0.200f, 0.200f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.360f, 0.360f, 0.360f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.200f, 0.200f, 0.200f, 1.00f);

		// Resizing
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.400f, 0.400f, 0.400f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.500f, 0.500f, 0.500f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.800f, 0.800f, 0.800f, 1.00f);

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4(0.200f, 0.200f, 0.200f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.500f, 0.500f, 0.500f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.300f, 0.300f, 0.300f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.200f, 0.200f, 0.200f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.300f, 0.300f, 0.300f, 1.00f);

		// Separator
		colors[ImGuiCol_Separator] = ImVec4(0.500f, 0.500f, 0.500f, 0.500f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.600f, 0.600f, 0.600f, 0.780f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.700f, 0.700f, 0.700f, 1.00f);

		// Checkboxes
		colors[ImGuiCol_CheckMark] = ImVec4(0.900f, 0.900f, 0.900f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.500f, 0.500f, 0.500f, 1.00f);

		// Sliders
		colors[ImGuiCol_SliderGrab] = ImVec4(0.368f, 0.458f, 0.827f, 0.8f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.368f, 0.458f, 0.827f, 1.0f);

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.400f, 0.400f, 0.400f, 0.800f);
		colors[ImGuiCol_TabActive] = ImVec4(0.180f, 0.180f, 0.180f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.098f, 0.098f, 0.098f, 1.00f);

		// Popup
		colors[ImGuiCol_PopupBg] = ImVec4(0.098f, 0.098f, 0.098f, 0.900f);

		// Modals
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.200f, 0.200f, 0.200f, 0.350f);

		// Debug
		colors[ImGuiCol_NavHighlight] = ImVec4(0.260f, 0.590f, 0.980f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.270f, 0.530f, 0.930f, 1.00f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.200f, 0.200f, 0.200f, 0.200f);
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()), static_cast<float>(app.GetWindow().GetHeight()));

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::OnImGuiRender()
	{

	}


}