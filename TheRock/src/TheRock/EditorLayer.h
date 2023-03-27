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
			auto environment = Environment::Load("assets/env/birchwood_4k.hdr");

			// Model Scene
			{
				m_Scene = Ref<Scene>::Create("Model Scene");
				m_Scene->SetCamera(Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)));

				m_Scene->SetEnvironment(environment);

				// Editor
				m_CheckerboardTex = Texture2D::Create("assets/editor/Checkerboard.tga");

				// Set lights
				m_Light.Direction = { -0.5f, -0.5f, 1.0f };
				m_Light.Radiance = { 1.0f, 1.0f, 1.0f };
			}
		}

		virtual void OnDetach() { }
		virtual void OnUpdate(Timestep ts) override
		{
			
			m_Scene->OnUpdate(ts);
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
			ImGui::Begin("DockSpace Demo", &p_open, window_flags);
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
			ImGui::SameLine();
			ImGui::Separator();
			ImGui::SliderFloat3("Light Dir", glm::value_ptr(m_Light.Direction), -1, 1);
			ImGui::ColorEdit3("Light Radiance", glm::value_ptr(m_Light.Radiance));
			ImGui::SliderFloat("Light Multiplier", &m_LightMultiplier, 0.0f, 5.0f);
			ImGui::SliderFloat("Exposure", &m_Exposure, 0.0f, 10.0f);
			ImGui::Separator();

			ImGui::Text("Shader Parameters");
			ImGui::Checkbox("Radiance Prefiltering", &m_RadiancePrefilter);
			ImGui::SliderFloat("Env Map Rotation", &m_EnvMapRotation, -360.0f, 360.0f);

			ImGui::Separator();
			if (ImGui::TreeNode("Shaders"))
			{
				auto& shaders = Shader::s_AllShaders;
				for (auto& shader : shaders)
				{
					if (ImGui::TreeNode(shader->GetName().c_str()))
					{
						std::string buttonName = "Reload##" + shader->GetName();
						if (ImGui::Button(buttonName.c_str()))
							shader->Reload();
						ImGui::TreePop();

					}
				}
				ImGui::TreePop();
			}
			ImGui::Begin("Environment");
			ImGui::SliderFloat("Mesh Scale", &m_MeshScale, 0.0f, 2.0f);
			ImGui::End();

			ImGui::Separator();
			{
				ImGui::Text("Mesh");
				std::string fullpath = m_SphereMesh ? m_SphereMesh->GetFilePath() : "None";
				size_t found = fullpath.find_last_of("/\\");
				std::string path = found != std::string::npos ? fullpath.substr(found + 1) : fullpath;
				ImGui::Text(path.c_str()); ImGui::SameLine();
				if (ImGui::Button("...##Mesh"))
				{
					std::string filename = RockEngine::Application::Get().OpenFile("");
					if (filename != "")
					{
						auto newMesh = Ref<Mesh>::Create(filename);
						m_Mesh = newMesh;
					}
				}
			}

			ImGui::Separator();

			// Textures ------------------------------------------------------------------------------

			{
				if (ImGui::CollapsingHeader("Albedo", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
					ImGui::Image(m_AlbedoInput.TextureMap ? (void*)m_AlbedoInput.TextureMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
					ImGui::PopStyleVar();

					if (ImGui::IsItemHovered())
					{
						if (m_AlbedoInput.TextureMap)
						{
							ImGui::BeginTooltip();
							ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
							ImGui::TextUnformatted(m_AlbedoInput.TextureMap->GetPath().c_str());
							ImGui::PopTextWrapPos();
							ImGui::Image((void*)m_AlbedoInput.TextureMap->GetRendererID(), ImVec2(384, 384));
							ImGui::EndTooltip();
						}
						if (ImGui::IsItemClicked())
						{
							std::string filename = RockEngine::Application::Get().OpenFile("");
							if (filename != "")
								m_AlbedoInput.TextureMap = (RockEngine::Texture2D::Create(filename, m_AlbedoInput.SRGB));
						}
					}
					ImGui::SameLine();
					ImGui::BeginGroup();
					ImGui::Checkbox("Use##AlbedoMap", &m_AlbedoInput.UseTexture);
					if (ImGui::Checkbox("sRGB##AlbedoMap", &m_AlbedoInput.SRGB))
					{
						if (m_AlbedoInput.TextureMap)
							m_AlbedoInput.TextureMap = (RockEngine::Texture2D::Create(m_AlbedoInput.TextureMap->GetPath(), m_AlbedoInput.SRGB));
					}
					ImGui::EndGroup();
					ImGui::SameLine();
					ImGui::ColorEdit3("Color##Albedo", glm::value_ptr(m_AlbedoInput.Color), ImGuiColorEditFlags_NoInputs);
				}
			}

			{
				// Normals
				if (ImGui::CollapsingHeader("Normals", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
					ImGui::Image(m_NormalInput.TextureMap ? (void*)m_NormalInput.TextureMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
					ImGui::PopStyleVar();
					if (ImGui::IsItemHovered())
					{
						if (m_NormalInput.TextureMap)
						{
							ImGui::BeginTooltip();
							ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
							ImGui::TextUnformatted(m_NormalInput.TextureMap->GetPath().c_str());
							ImGui::PopTextWrapPos();
							ImGui::Image((void*)m_NormalInput.TextureMap->GetRendererID(), ImVec2(384, 384));
							ImGui::EndTooltip();
						}
						if (ImGui::IsItemClicked())
						{
							std::string filename = RockEngine::Application::Get().OpenFile("");
							if (filename != "")
								m_NormalInput.TextureMap = (RockEngine::Texture2D::Create(filename));
						}
					}
					ImGui::SameLine();
					ImGui::Checkbox("Use##NormalMap", &m_NormalInput.UseTexture);
				}
			}

			ImGui::End();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Viewport");
			auto viewportSize = ImGui::GetContentRegionAvail();

			SceneRenderer::SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			m_Scene->GetCamera().SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
			m_Scene->GetCamera().SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			ImGui::Image((void*)SceneRenderer::GetFinalColorBufferRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
			ImGui::End();

			/*ImGui::Begin("Viewport 2");
			m_FinalPresentBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			ImGui::Image((void*)m_FinalPresentBuffer->GetColorAttachmentRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
			ImGui::End();*/

			ImGui::PopStyleVar();

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Docking"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows, 
					// which we can't undo at the moment without finer window depth/z control.
					//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

					if (ImGui::MenuItem("Flag: NoSplit", "", (opt_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 opt_flags ^= ImGuiDockNodeFlags_NoSplit;
					if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (opt_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  opt_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
					if (ImGui::MenuItem("Flag: NoResize", "", (opt_flags & ImGuiDockNodeFlags_NoResize) != 0))                opt_flags ^= ImGuiDockNodeFlags_NoResize;
					if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (opt_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          opt_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
					ImGui::Separator();
					if (ImGui::MenuItem("Close DockSpace", NULL, false, p_open != NULL))
						p_open = false;
					ImGui::EndMenu();
				}


				ImGui::EndMenuBar();
			}

			ImGui::End();
		}
	private:
		Ref<RockEngine::Shader> m_Shader;
		Ref<RockEngine::Shader> m_PBRShader;
		Ref<RockEngine::Shader> m_SimplePBRShader;
		Ref<RockEngine::Shader> m_QuadShader;
		Ref<RockEngine::Shader> m_HDRShader;
		Ref<RockEngine::Mesh> m_Mesh;
		Ref<RockEngine::Mesh> m_SphereMesh;
		Ref<RockEngine::Texture2D> m_BRDFLUT;


		Camera m_Camera = glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f);

		struct AlbedoInput
		{
			glm::vec3 Color = { 0.972f, 0.96f, 0.915f }; // Silver, from https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
			Ref<RockEngine::Texture2D> TextureMap;
			bool SRGB = true;
			bool UseTexture = false;
		};
		AlbedoInput m_AlbedoInput;

		struct NormalInput
		{
			Ref<RockEngine::Texture2D> TextureMap;
			bool UseTexture = false;
		};
		NormalInput m_NormalInput;

		struct MetalnessInput
		{
			float Value = 1.0f;
			Ref<RockEngine::Texture2D> TextureMap;
			bool UseTexture = false;
		};
		MetalnessInput m_MetalnessInput;

		struct RoughnessInput
		{
			float Value = 0.5f;
			Ref<RockEngine::Texture2D> TextureMap;
			bool UseTexture = false;
		};
		RoughnessInput m_RoughnessInput;

		float m_MeshScale = 1.0f;

		// Editor resources
		Ref<Texture2D> m_CheckerboardTex;
		Ref<RockEngine::Framebuffer> m_Framebuffer, m_FinalPresentBuffer;
		Ref<RockEngine::VertexBuffer> m_VertexBuffer;
		Ref<RockEngine::IndexBuffer> m_IndexBuffer;
		Ref<RockEngine::TextureCube> m_EnvironmentCubeMap, m_EnvironmentIrradiance;
		float m_ClearColor[4];
		struct Light
		{
			glm::vec3 Direction;
			glm::vec3 Radiance;
		};
		Light m_Light;
		float m_LightMultiplier = 0.3f;

		Ref<Pipeline> m_Pipeline;

		float m_Exposure = 1.0f;
		bool m_RadiancePrefilter = false;

		float m_EnvMapRotation = 0.0f;

		Ref<Material> m_MeshMaterial;

		Ref<Scene> m_Scene;
	};
}