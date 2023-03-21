#pragma once

#include <TheRock.h>
#include "RockEngine/Core/Layer.h"
#include "GLFW/include/GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RockEngine
{
	class Editor : public Layer
	{
		virtual void OnAttach() 
		{
			static float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
			};

			static unsigned int indices[] = {
				0, 1, 2
			};

			m_VB = VertexBuffer::Create(vertices, sizeof(vertices));
		

			m_IB = IndexBuffer::Create(indices, sizeof(indices));

			m_Shader = Shader::Create("assets/shaders/shader.glsl");
			m_Shader->Bind();

			FramebufferSpec spec;
			spec.Width = Application::Get().GetWindow().GetWidth();
			spec.Height = Application::Get().GetWindow().GetHeight();
			m_Framebuffer = Framebuffer::Create(spec);

			m_Texture = Texture2D::Create("assets/textures/logo.png",false);
			
		}
		virtual void OnDetach() { }
		virtual void OnUpdate() 
		{
			m_Framebuffer->Bind();
			RockEngine::Renderer::Clear(1.0f,1.0f,1.0f,1.0f);
			m_Shader->SetFloat3("u_Color", m_ClearColor);
			m_IB->Bind();
			m_VB->Bind();
			Renderer::DrawIndexed(3);
			m_Framebuffer->Unbind();
		}
		virtual void OnImGuiRender()
		{
			static bool p_open = true;

			static bool opt_fullscreen_persistant = true;
			static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
			bool opt_fullscreen = opt_fullscreen_persistant;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}



			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Viewport", &p_open, window_flags);
			auto viewportSize = ImGui::GetContentRegionAvail();
			m_Framebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			ImGui::Image((void*)m_Framebuffer->GetColorAttachmentRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// Dockspace
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
			}

			// Editor Panel ------------------------------------------------------------------------------
			ImGui::Begin("Settings");
			ImGui::ColorEdit3("Clear Color", glm::value_ptr(m_ClearColor));
			ImGui::End();
			ImGui::End();
		}
	private:
		Ref<VertexBuffer> m_VB;
		Ref<IndexBuffer> m_IB;
		Ref<Framebuffer> m_Framebuffer;
		glm::vec3 m_ClearColor;
		Ref<Texture2D> m_Texture;
		Ref<Shader> m_Shader;
	};
}