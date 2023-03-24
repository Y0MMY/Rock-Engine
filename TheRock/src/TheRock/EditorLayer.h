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
			m_ModelShader = (RockEngine::Shader::Create("assets/shaders/shader.glsl"));
			m_Shader = (RockEngine::Shader::Create("assets/shaders/quad.glsl"));
			m_Model = (new RockEngine::Mesh("assets/meshes/Sphere1m.fbx"));

			m_Texture = (RockEngine::TextureCube::Create("assets/textures/environments/Arches_E_PineTree_Irradiance.tga"));

			FramebufferSpec spec;
			spec.Width = 1920;
			spec.Height = 1080;
			spec.Format = FramebufferTextureFormat::RGBA8;
			m_Framebuffer = Framebuffer::Create(spec);

			// Create Quad
			float x = -1, y = -1;
			float width = 2, height = 2;
			struct QuadVertex
			{
				glm::vec3 Position;
				glm::vec2 TexCoord;
			};

			QuadVertex* data = new QuadVertex[4];

			data[0].Position = glm::vec3(x, y, 0);
			data[0].TexCoord = glm::vec2(0, 0);

			data[1].Position = glm::vec3(x + width, y, 0);
			data[1].TexCoord = glm::vec2(1, 0);

			data[2].Position = glm::vec3(x + width, y + height, 0);
			data[2].TexCoord = glm::vec2(1, 1);

			data[3].Position = glm::vec3(x, y + height, 0);
			data[3].TexCoord = glm::vec2(0, 1);

			m_VB = (RockEngine::VertexBuffer::Create(data, 4 * sizeof(QuadVertex)));

			uint32_t* indices = new uint32_t[6]{ 0, 1, 2, 2, 3, 0, };
			m_IB = (RockEngine::IndexBuffer::Create(indices, 6 * sizeof(unsigned int)));

			PipelineSpecification layout;
			layout.Layout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
			};
			m_Pipeline = Pipeline::Create(layout);

		}
		virtual void OnDetach() { }
		virtual void OnUpdate() 
		{
			m_Camera.Update();
			auto viewProjection = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();

			m_Framebuffer->Bind();
			RockEngine::Renderer::Clear(1.0f, 1.0f, 1.0f, 1.0f);

			m_Shader->SetMat4("u_InverseVP", inverse(viewProjection));
			m_Shader->Bind();
			m_Texture->Bind();
			m_IB->Bind();
			m_VB->Bind();
			m_Pipeline->Bind();
			Renderer::DrawIndexed(m_IB->GetCount(),false);

			m_ModelShader->SetMat4("u_MVP", viewProjection);
			m_Model->Render();
			m_ModelShader->Bind();

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
			m_Camera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
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
			auto cameraForward = m_Camera.GetForwardDirection();
			ImGui::Text("Camera Forward: %.2f, %.2f, %.2f", cameraForward.x, cameraForward.y, cameraForward.z);
			ImGui::End();
			ImGui::End();
		}
	private:
		Ref<VertexBuffer> m_VB;
		Ref<IndexBuffer> m_IB;
		Ref<Framebuffer> m_Framebuffer;
		glm::vec3 m_ClearColor;
		Ref<TextureCube> m_Texture;
		Ref<Shader> m_Shader;
		Ref<Shader> m_ModelShader;
		Ref<Mesh> m_Model;
		Ref<Pipeline> m_Pipeline;

		Camera m_Camera = glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f);
	};
}