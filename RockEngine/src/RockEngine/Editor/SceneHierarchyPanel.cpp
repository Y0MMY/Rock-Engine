#include "pch.h"
#include "SceneHierarchyPanel.h"

#include "ImGui.h"
#include "imgui/imgui_internal.h"

#include "RockEngine/Utilities/StringUtils.h"
#include "RockEngine/Utilities/FileSystem.h"
#include "RockEngine/Core/Math/Math.h"

#include "RockEngine/Core/Application.h"
#include "RockEngine/Renderer/Mesh.h"
#include <assimp/scene.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>
#include <RockEngine/Scene/Components.h>

#include "RockEngine/ImGui/UICore.h"

#include "RockEngine/Renderer/SceneRenderer.h"

namespace RockEngine
{
	static glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		bool modified = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return modified;
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity* entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity->HasComponent<T>())
		{
			ImGui::PushID((void*)typeid(T).hash_code());
			auto& component = entity->GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx("##dummy_id", treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity->RemoveComponent<T>();

			ImGui::PopID();
		}
	}

//--------------------------------------------------------------------------

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
		: m_Context(context)
	{
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
	}
	
	void SceneHierarchyPanel::DrawComponentsList()
	{
		if (m_Context)
		{
			if (ImGui::Button("Add"))
			{
				ImGui::OpenPopup("AddEntityMenu");
			}

			ImGui::SetNextWindowPos(ImVec2(400, 70));
			if (ImGui::BeginPopup("AddEntityMenu"))
			{
				ImGui::Indent(20.0f); 

				if (ImGui::MenuItem("Empty Entity"))
				{
					decltype(auto) newEntity = m_Context->CreateEntity("Empty Entity");
				}

				ImGui::Spacing();

				if (ImGui::MenuItem("Mesh"))
				{
					decltype(auto) newEntity = m_Context->CreateEntity("Mesh");
					newEntity->AddComponent<MeshComponent>();
					m_Context->SetSelected(newEntity);
				}

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::BeginMenu("Light"))
				{
					if (ImGui::MenuItem("Sky Light"))
					{
						decltype(auto) newEntity = m_Context->CreateEntity("Sky Light");
						newEntity->AddComponent<SkyLightComponent>();
						m_Context->SetSelected(newEntity);
					}

					ImGui::Spacing();

					if (ImGui::MenuItem("Directional Light"))
					{
						decltype(auto) newEntity = m_Context->CreateEntity("Directional Light");
						newEntity->AddComponent<DirectionalLightComponent>();
						m_Context->SetSelected(newEntity);
					}

					ImGui::Spacing();

					if (ImGui::MenuItem("Point Light"))
					{
						decltype(auto) newEntity = m_Context->CreateEntity("Point Light");
						newEntity->AddComponent<PointLightComponent>();
						m_Context->SetSelected(newEntity);
					}

					ImGui::EndMenu();
				}

				ImGui::Unindent();
				ImGui::EndPopup();
			}
		}
	}


	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Outliner");

		uint32_t entityCount = 0, meshCount = 0;
		auto& sceneEntities = m_Context->GetAllEntities();
		for (const auto& [key, entity] : sceneEntities)
			DrawEntityNode(entity, entityCount, meshCount);

		ImGui::End();

		ImGui::Begin("Properties");

		if (m_Context->m_SelectedEntity)
				DrawComponents(m_Context->m_SelectedEntity);
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity* entity, uint32_t& imguiEntityID, uint32_t& imguiMeshID)
	{
		const char* name = "Unnamed Entity";
		if (entity->HasComponent<TagComponent>())
			name = entity->GetComponent<TagComponent>().Tag.c_str();
		
		ImGuiTreeNodeFlags flags = (entity == m_Context->m_SelectedEntity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)entity, flags, name);

		if (ImGui::IsItemClicked())
		{
			m_Context->SetSelected(entity);	
		}

		if (opened)
		{
			// TODO: Children
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::DrawMeshNode(const Ref<Mesh>& mesh, uint32_t& imguiMeshID)
	{
		static char imguiName[128];
		memset(imguiName, 0, 128);
		sprintf(imguiName, "Mesh##%d", imguiMeshID++);

		// Mesh Hierarchy
		if (ImGui::TreeNode(imguiName))
		{
			auto rootNode = mesh->m_Scene->mRootNode;
			MeshNodeHierarchy(mesh, rootNode, mesh->GetSubmeshes()[0].Transform);
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::MeshNodeHierarchy(const Ref<Mesh>& mesh, aiNode* node, const glm::mat4& parentTransform, uint32_t level)
	{
		glm::mat4 localTransform = Mat4FromAssimpMat4(node->mTransformation);
		glm::mat4 transform = parentTransform * localTransform;

		if (ImGui::TreeNode(node->mName.C_Str()))
		{
			

			for (uint32_t i = 0; i < node->mNumChildren; i++)
				MeshNodeHierarchy(mesh, node->mChildren[i], transform, level + 1);

			ImGui::TreePop();
		}

	}

	void SceneHierarchyPanel::DrawComponents(Entity* entity)
	{
		ImGui::AlignTextToFramePadding();
		//auto id = entity->GetComponent<IDComponent>().ID;

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		if (entity->HasComponent<TagComponent>())
		{
			auto& tag = entity->GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, 256);
			memcpy(buffer, tag.c_str(), tag.length());
			ImGui::PushItemWidth(contentRegionAvailable.x * 0.5f);
			if (ImGui::InputText("##Tag", buffer, 256))
			{
				tag = std::string(buffer);
			}
			ImGui::PopItemWidth();
		}

		// ID
		ImGui::SameLine();
		//ImGui::TextDisabled("%llx", id);
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 addTextSize = ImGui::CalcTextSize(" ADD        ");
		addTextSize.x += GImGui->Style.FramePadding.x * 2.0f;

		ImGui::SameLine(contentRegionAvailable.x - (addTextSize.x + GImGui->Style.FramePadding.y));
		if (ImGui::Button(" ADD        "))
			ImGui::OpenPopup("AddComponentPanel");
		if (ImGui::BeginPopup("AddComponentPanel"))
		{

			ImGui::EndPopup();
		}

		DrawComponent<TransformComponent>("Transform", entity, [=](TransformComponent& component) mutable
			{
				
				DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f);
	
			});

		DrawComponent<MeshComponent>("Mesh", entity, [=](MeshComponent& mc) mutable
			{
				UI::BeginPropertyGrid();

				ImGui::Text(entity->GetName().c_str());
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);

				ImVec2 originalButtonTextAlign = ImGui::GetStyle().ButtonTextAlign;
				ImGui::GetStyle().ButtonTextAlign = { 0.0f, 0.5f };
				float width = ImGui::GetContentRegionAvail().x - 0.0f;
				UI::PushID();

				float itemHeight = 28.0f;

				std::string buttonName;
				if (mc.Mesh)
					buttonName = mc.Mesh->GetName();
				else
					buttonName = "Null";

				if (ImGui::Button(buttonName.c_str(), { width, itemHeight }))
				{
					std::string file = Utils::FileSystem::OpenFileDialog().string();
					if (!file.empty())
						mc.Mesh = Ref<Mesh>::Create(file);
				}

				UI::PopID();
				ImGui::GetStyle().ButtonTextAlign = originalButtonTextAlign;
				ImGui::PopItemWidth();

				UI::EndPropertyGrid();
			});

		DrawComponent<SkyLightComponent>("Sky Light", entity, [=](SkyLightComponent& slc) mutable
			{
				
				UI::BeginPropertyGrid();

				ImGui::Text("Sky Light");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);

				ImVec2 originalButtonTextAlign = ImGui::GetStyle().ButtonTextAlign;
				ImGui::GetStyle().ButtonTextAlign = { 0.0f, 0.5f };
				float width = ImGui::GetContentRegionAvail().x - 0.0f;
				UI::PushID();

				float itemHeight = 28.0f;

				std::string buttonName;
				if (slc.SceneEnvironment.IrradianceMap)
					buttonName = slc.Name;
				else
					buttonName = "Null";

				if(ImGui::Button(buttonName.c_str(), { width, itemHeight }))
				{
					std::filesystem::path file = Utils::FileSystem::OpenFileDialog("*.hdr").string();
					if (!file.empty())
					{
						slc.SceneEnvironment = SceneRenderer::CreateEnvironmentMap(file);
						slc.Name = Utils::FileSystem::GetFileName(file);
						slc.FilePath = file.string();
					}
				}

				UI::PopID();
				ImGui::GetStyle().ButtonTextAlign = originalButtonTextAlign;
				ImGui::PopItemWidth();
				ImGui::NextColumn();
				UI::Property("Intensity", slc.Intensity, 0.01f, 0.0f, 5.0f);
				ImGui::Separator();
				
				UI::EndPropertyGrid();

			});

		DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](DirectionalLightComponent& dlc)
			{
				UI::BeginPropertyGrid();
				UI::PropertyColor("Radiance", dlc.Radiance);
				UI::Property("Intensity", dlc.Intensity);
				UI::Property("Cast Shadows", dlc.CastShadows);
				UI::Property("Soft Shadows", dlc.SoftShadows);
				UI::Property("Source Size", dlc.LightSize);
				UI::EndPropertyGrid();
			});

		DrawComponent<PointLightComponent>("Point Light", entity, [](PointLightComponent& dlc)
			{
				UI::BeginPropertyGrid();
				UI::PropertyColor("Radiance", dlc.Radiance);
				UI::Property("Intensity", dlc.Intensity);
				UI::Property("Source Size", dlc.LightSize);
				UI::Property("Min Radius", dlc.MinRadius);
				UI::Property("Radius", dlc.Radius);
				UI::Property("Falloff", dlc.Falloff);
				UI::EndPropertyGrid();
			});
	}

}