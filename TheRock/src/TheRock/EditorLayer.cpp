#include "pch.h"
#include "EditorLayer.h"

#include "RockEngine/ImGui/ImGui.h"
#include "RockEngine/Core/Math/Ray.h"

#include "RockEngine/ImGui/ImGuizmo.h"

namespace RockEngine
{
	EditorLayer::EditorLayer()
		: m_Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))

	{

	}

	std::pair<float, float> EditorLayer::GetMouseViewportSpace()
	{
		auto [mx, my] = ImGui::GetMousePos(); // Input::GetMousePosition();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		auto viewportWidth = m_ViewportBounds[1].x - m_ViewportBounds[0].x;
		auto viewportHeight = m_ViewportBounds[1].y - m_ViewportBounds[0].y;

		return { (mx / viewportWidth) * 2.0f - 1.0f, ((my / viewportHeight) * 2.0f - 1.0f) * -1.0f };
	}

	std::pair<glm::vec3, glm::vec3> EditorLayer::CastRay(float mx, float my)
	{
		glm::vec4 mouseClipPos = { mx, my, -1.0f, 1.0f };

		auto inverseProj = glm::inverse(m_EditorCamera.GetProjectionMatrix());
		auto inverseView = glm::inverse(glm::mat3(m_EditorCamera.GetViewMatrix()));

		glm::vec4 ray = inverseProj * mouseClipPos;
		glm::vec3 rayPos = m_EditorCamera.GetPosition();
		glm::vec3 rayDir = inverseView * glm::vec3(ray);

		return { rayPos, rayDir };
	}

	bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case KeyCode::Q:
			m_GizmoType = -1;
			break;
		case KeyCode::W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case KeyCode::E:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case KeyCode::R:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		case KeyCode::G:
			// Toggle grid
			if (RockEngine::Input::IsKeyPressed(KeyCode::LeftControl))
				SceneRenderer::GetOptions().ShowGrid = !SceneRenderer::GetOptions().ShowGrid;
			break;
		case KeyCode::B:
			// Toggle bounding boxes 
			if (RockEngine::Input::IsKeyPressed(KeyCode::LeftControl))
			{
				m_UIShowBoundingBoxes = !m_UIShowBoundingBoxes;
				ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxes);
			}
			break;
		case KeyCode::A:
			
			if (RockEngine::Input::IsKeyPressed(KeyCode::A))
				m_EditorScene->SetSelected(m_MeshEntity);
			break;
		}
		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		//auto [mouseX, mouseY] = GetMouseViewportSpace();
		//if (e.GetMouseButton() == MouseButton::Left && !Input::IsKeyPressed(KeyCode::LeftAlt) && !ImGuizmo::IsOver())
		//{
		//	if (mouseX > -1.0f && mouseX < 1.0f && mouseY > -1.0f && mouseY < 1.0f)
		//	{

		//		auto [origin, direction] = CastRay(mouseX, mouseY);

		//		m_EditorScene->m_SelectionContext.clear();
		//		auto mesh = m_MeshEntity->GetComponent<MeshComponent>().Mesh;
		//		auto& submeshes = mesh->GetSubmeshes();
		//		float lastT = std::numeric_limits<float>::max();
		//		for (uint32_t i = 0; i < submeshes.size(); i++)
		//		{
		//			auto& submesh = submeshes[i];
		//			Ray ray = {
		//				glm::inverse(m_MeshEntity->GetTransform() * submesh.Transform) * glm::vec4(origin, 1.0f),
		//				glm::inverse(glm::mat3(m_MeshEntity->GetTransform()) * glm::mat3(submesh.Transform)) * direction
		//			};

		//			float t;
		//			bool intersects = ray.IntersectsAABB(submesh.BoundingBox, t);
		//			if (intersects)
		//			{
		//				const auto& triangleCache = mesh->GetTriangleCache(i);
		//				for (const auto& triangle : triangleCache)
		//				{
		//					if (ray.IntersectsTriangle(triangle.V0.Position, triangle.V1.Position, triangle.V2.Position, t))
		//					{
		//						RE_CORE_WARN("INTERSECTION: {0}, t={1}", submesh.NodeName, t);
		//						m_EditorScene->m_SelectionContext.push_back({ m_MeshEntity, &submesh, t });
		//						m_EditorScene->SetSelected(m_MeshEntity);
		//						break;
		//					}
		//				}
		//			}
		//		}
		//		std::sort(m_EditorScene->m_SelectionContext.begin(), m_EditorScene->m_SelectionContext.end(), [](auto& a, auto& b) { return a.Distance < b.Distance; });

		//		// TODO: Handle mesh being deleted, etc.
		//		if (m_EditorScene->m_SelectionContext.size())
		//			m_CurrentlySelectedTransform = &m_EditorScene->m_SelectionContext[0].Mesh->Transform;
		//		else
		//		{
		//			m_CurrentlySelectedTransform = &m_MeshEntity->Transform().GetTransform();
		//			//m_EditorScene->SetSelected(nullptr);
		//		}
		//	}
		//}
		return false;
	}

	void EditorLayer::ShowBoundingBoxes(bool show, bool onTop)
	{
		SceneRenderer::GetOptions().ShowBoundingBoxes = show;
	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher disp(e);
		disp.Dispatch<MouseButtonPressedEvent>(BIND_FN(EditorLayer::OnMouseButtonPressed));
		disp.Dispatch<KeyPressedEvent>(BIND_FN(EditorLayer::OnKeyPressedEvent));
	}

	void EditorLayer::OnAttach()
	{
		// ImGui Colors
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f); // Window background
		colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.5f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.3f, 0.3f, 0.3f, 0.5f); // Widget backgrounds
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.4f, 0.4f, 0.4f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 0.6f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.4f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.0f);
		colors[ImGuiCol_Header] = ImVec4(0.7f, 0.7f, 0.7f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.7f, 0.7f, 0.7f, 0.8f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.5f, 0.52f, 1.0f);
		colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.5f, 0.5f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.0f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.43f, 0.35f, 1.0f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.6f, 0.15f, 1.0f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
		colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.6f, 0.6f, 1.0f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);


		//auto environment = Environment::Load("assets/env/birchwood_4k.hdr");

		// Model Scene
		{
			// Editor
			m_CheckerboardTex = Texture2D::Create("assets/editor/Checkerboard.tga");
			m_SceneHierarchyPanel = std::make_unique<SceneHierarchyPanel>(m_EditorScene);

			NewScene();
		}
	}

	void EditorLayer::UpdateWindowTitle(const std::string& sceneName)
	{
		std::string title = sceneName + " - TheRock - " + Application::GetPlatformName() + " (" + Application::GetConfigurationName() + ")";
		Application::Get().GetWindow().SetTitle(title);
	}

	void EditorLayer::NewScene()
	{
		m_EditorScene = Ref<Scene>::Create("Empty Scene");
		m_SceneHierarchyPanel->SetContext(m_EditorScene);
		UpdateWindowTitle("Empty Scene");

		m_EditorCamera = EditorCamera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 1000.0f));
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		m_EditorScene->OnUpdate(ts);

		m_EditorCamera.OnUpdate(ts);
		m_EditorScene->OnRenderEditor(ts, m_EditorCamera);

		if (m_EditorScene->m_SelectionContext.size())
		{
			auto& selection = m_EditorScene->m_SelectionContext[0];

			RockEngine::Renderer::BeginRenderPass(RockEngine::SceneRenderer::GetFinalRenderPass(), false);
			auto viewProj = m_EditorCamera.GetViewProjection();
			RockEngine::Renderer2D::BeginScene(viewProj, false);
			auto& submesh = m_EditorScene->m_SelectionContext[0];
			Renderer::DrawAABB(selection.Mesh->BoundingBox, selection.Entity->Transform().GetTransform() * selection.Mesh->Transform);
			RockEngine::Renderer2D::EndScene();
			RockEngine::Renderer::EndRenderPass();
		}

	}

	void EditorLayer::OnImGuiRender()
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

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		//if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
		//	window_flags |= ImGuiWindowFlags_NoBackground;
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
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		auto viewportOffset = ImGui::GetCursorPos(); // includes tab bar
		auto viewportSize = ImGui::GetContentRegionAvail();
		SceneRenderer::SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		m_EditorCamera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
		m_EditorCamera.SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		ImGui::Image((void*)SceneRenderer::GetFinalColorBufferRendererID(), viewportSize, { 0, 1 }, { 1, 0 });

		static int counter = 0;
		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBound = ImGui::GetWindowPos();
		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;

		ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
		m_ViewportBounds[0] = { minBound.x, minBound.y };
		m_ViewportBounds[1] = { maxBound.x, maxBound.y };
		//m_AllowViewportCameraEvents = ImGui::IsMouseHoveringRect(minBound, maxBound);

		// Gizmos
		if (m_GizmoType != -1 && m_EditorScene->m_SelectionContext.size())
		{
			auto& selection = m_EditorScene->m_SelectionContext[0];

			float rw = (float)ImGui::GetWindowWidth();
			float rh = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);

			bool snap = Input::IsKeyPressed(KeyCode::LeftControl);

			TransformComponent& entityTransform = selection.Entity->Transform();


			ImGuizmo::Manipulate(glm::value_ptr(m_EditorCamera.GetViewMatrix() * m_MeshEntity->Transform().GetTransform()),
				glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),
				(ImGuizmo::OPERATION)m_GizmoType,
				ImGuizmo::LOCAL,
				glm::value_ptr(*m_CurrentlySelectedTransform),
				nullptr,
				snap ? &m_SnapValue : nullptr);
		}

		ImGui::End();
		ImGui::PopStyleVar();
		// Settings ImGui
		{
			ImGui::Begin("Settings");

			UI::BeginPropertyGrid();
			ImGui::AlignTextToFramePadding();
			UI::PropertySlider("Skybox LOD", m_EditorScene->GetSkyboxLod(), 0.0f, Utils::CalculateMipCount(viewportSize.x, viewportSize.y));

			ImGui::Columns(2);
			ImGui::AlignTextToFramePadding();

			
			UI::PropertySlider("Exposure", m_Camera.GetExposure(), 0.0f, 5.0f);
			UI::PropertySlider("Env Map Rotation", m_EnvMapRotation, -360.0f, 360.0f);
			UI::Property("Show Bounding Boxes", SceneRenderer::GetOptions().ShowBoundingBoxes);
			UI::Property("Show grid", SceneRenderer::GetOptions().ShowGrid);
			UI::EndPropertyGrid();

			ImGui::Separator();

			ImGui::Text("Renderer Settings");

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

			ImGui::End();
		} // End Of Setting ImGui

		m_SceneHierarchyPanel->OnImGuiRender();

		ImGui::End();

	}
}