#include "pch.h"
#include "SceneRendererPanel.h"

#include "imgui/imgui.h"
#include "RockEngine/ImGui/UICore.h"

namespace RockEngine
{
	void SceneRendererPanel::OnImGuiRender(bool isOpen)
	{
		if (ImGui::Begin("Scene Renderer", &isOpen))
		{
			ImGui::Text("Viewport Size: %d, %d", m_Context->m_ViewportWidth, m_Context->m_ViewportHeight);

			UI::BeginPropertyGrid();
			UI::Property("Draw Outline", m_Context->m_Options.DrawOutline);
			UI::Property("Show Bounding Boxes", m_Context->m_Options.ShowBoundingBoxes);
			UI::Property("Show Grid", m_Context->m_Options.ShowGrid);
			UI::Property("SkyBox Lod", m_Context->m_Scene->m_SkyboxLod, 1.0f, 10.0f);
			UI::EndPropertyGrid();

			const float headerSpacingOffset = -(ImGui::GetStyle().ItemSpacing.y + 1.0f);
			const bool shadersTreeNode = UI::PropertyGridHeader("Shaders", false);

			if (shadersTreeNode)
			{
				static std::string searchedString;
				
				auto& shaders = Shader::s_AllShaders;
				for (auto& shader : shaders)
				{
					std::string name = shader->GetName();
					ImGui::Columns(2);
					ImGui::Text(name.c_str());
					ImGui::NextColumn();
					std::string buttonName = fmt::format("Reload##{0}", name);
					if (ImGui::Button(buttonName.c_str()))
						shader->Reload();
					ImGui::Columns(1);
				}
				
				UI::EndTreeNode();
				UI::ShiftCursorY(-10.f);
			}
			/*else
				UI::ShiftCursorY(headerSpacingOffset);*/

			if (UI::PropertyGridHeader("Shadows", false))
			{
				bool a = false;
				UI::BeginPropertyGrid();
				UI::Property("Soft Shadows", m_Context->ShadowSettings.SoftShadows);
				UI::Property("Light Size", m_Context->ShadowSettings.LightSize, 0.01f);
				UI::Property("Max Shadow Distance", m_Context->ShadowSettings.MaxShadowDistance, 1.0f);
				UI::Property("Shadow Fade", m_Context->ShadowSettings.ShadowFade, 5.0f);
				UI::EndPropertyGrid();

				if (UI::BeginTreeNode("Cascade Settings", false))
				{
					UI::BeginPropertyGrid();
					UI::Property("Show Cascades", m_Context->ShadowSettings.ShowCascades);
					UI::Property("Cascade Fading", m_Context->ShadowSettings.CascadeFading);
					UI::Property("Cascade Transition Fade", m_Context->ShadowSettings.CascadeTransitionFade, 0.05f, 0.0f, FLT_MAX);
					UI::Property("Cascade Split", m_Context->ShadowSettings.CascadeSplitLambda, 0.01f);
					UI::Property("CascadeNearPlaneOffset", m_Context->ShadowSettings.CascadeNearPlaneOffset, 0.1f, -FLT_MAX, 0.0f);
					UI::Property("CascadeFarPlaneOffset", m_Context->ShadowSettings.CascadeFarPlaneOffset, 0.1f, 0.0f, FLT_MAX);
					UI::EndPropertyGrid();
					UI::EndTreeNode();
				}

				if (UI::BeginTreeNode("Shadow Map", false))
				{
					static int cascadeIndex = 0;
					auto fb = m_Context->m_ShadowMapRenderPass[cascadeIndex]->GetSpecification().TargetFramebuffer;
					auto id = fb->GetDepthAttachmentRendererID();

					float size = ImGui::GetContentRegionAvailWidth(); // (float)fb->GetWidth() * 0.5f, (float)fb->GetHeight() * 0.5f
					UI::BeginPropertyGrid();
					UI::PropertySlider("Cascade Index", cascadeIndex, 0, 3);
					UI::EndPropertyGrid();
					ImGui::Image((ImTextureID)id, { size, size }, { 0, 1 }, { 1, 0 });
					UI::EndTreeNode();
				}

				UI::EndTreeNode();
			}
		}

		ImGui::End();
	}
}