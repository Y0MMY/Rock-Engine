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
			//ImGui::Text("Viewport Size: %d, %d", m_Context->m_ViewportWidth, m_Context->m_ViewportHeight);

			const float headerSpacingOffset = -(ImGui::GetStyle().ItemSpacing.y + 1.0f);
			const bool shadersTreeNode = UI::PropertyGridHeader("Shaders", false);
			/*if (UI::Button("Reload All"))
			{
				auto& shaders = Renderer::GetShaderLibrary()->GetShaders();
				for (auto& [name, shader] : shaders)
					shader->Reload();
			}*/

			if (shadersTreeNode)
			{
				static std::string searchedString;
				
				auto& shaders = Renderer::GetShaderLibrary()->GetShaders();
				for (auto& [name, shader] : shaders)
				{
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
			else
				UI::ShiftCursorY(headerSpacingOffset);
		}

		ImGui::End();
	}
}