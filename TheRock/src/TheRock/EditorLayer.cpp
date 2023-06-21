#include "pch.h"
#include "EditorLayer.h"

#include "RockEngine/ImGui/UICore.h"
#include "RockEngine/Core/Math/Ray.h"

#include "RockEngine/ImGui/ImGuizmo.h"
#include "RockEngine/Utilities/StringUtils.h"

namespace RockEngine
{
	EditorLayer::EditorLayer()
		: m_EditorCamera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 1000.0f))

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
				m_ViewportRenderer->GetOptions().ShowGrid = !m_ViewportRenderer->GetOptions().ShowGrid;
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
		auto [mx, my] = Input::GetMousePosition();
		if (e.GetMouseButton() == MouseButton::Left && !Input::IsKeyPressed(KeyCode::LeftAlt) && !ImGuizmo::IsOver())
		{
			auto [mouseX, mouseY] = GetMouseViewportSpace();
			if (mouseX > -1.0f && mouseX < 1.0f && mouseY > -1.0f && mouseY < 1.0f)
			{
				auto [origin, direction] = CastRay(mouseX, mouseY);
				m_EditorScene->m_SelectionContext.clear();
				m_EditorScene->SetSelected({});

				auto& meshEntities = m_EditorScene->GetAllEntitiesSceneWith<MeshComponent>();

				for (auto e : meshEntities)
				{
					auto mesh = e->GetComponent<MeshComponent>().Mesh;
					if (!mesh) continue;
					auto& submeshes = mesh->GetSubmeshes();
					float lastT = std::numeric_limits<float>::max();

					for (uint32_t i = 0; i < submeshes.size(); i++)
					{
						auto& submesh = submeshes[i];
						Math::Ray ray = {
							glm::inverse(e->GetTransform() * submesh.Transform) * glm::vec4(origin, 1.0f),
							glm::inverse(glm::mat3(e->GetTransform()) * glm::mat3(submesh.Transform)) * direction
						};

						float t;
						bool intersects = ray.IntersectsAABB(submesh.BoundingBox, t);
						if (intersects)
						{
							const auto& triangleCache = mesh->GetTriangleCache(i);
							for (const auto& triangle : triangleCache)
							{
								if (ray.IntersectsTriangle(triangle.V0.Position, triangle.V1.Position, triangle.V2.Position, t))
								{
									RE_WARN("INTERSECTION: {0}, t={1}", submesh.NodeName, t);
									m_EditorScene->m_SelectionContext.push_back({ e, &submesh, t });
									break;
								}
							}
						}
					}
				}
				std::sort(m_EditorScene->m_SelectionContext.begin(), m_EditorScene->m_SelectionContext.end(), [](auto& a, auto& b) { return a.Distance < b.Distance; });
				if (m_EditorScene->m_SelectionContext.size())
				{
					m_EditorScene->SetSelected(m_EditorScene->m_SelectionContext[0].Entity);
				}
			}
		}
		return false;
	}

	void EditorLayer::ShowBoundingBoxes(bool show, bool onTop)
	{
		m_ViewportRenderer->GetOptions().ShowBoundingBoxes = show;
	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher disp(e);
		disp.Dispatch<MouseButtonPressedEvent>(BIND_FN(EditorLayer::OnMouseButtonPressed));
		disp.Dispatch<KeyPressedEvent>(BIND_FN(EditorLayer::OnKeyPressedEvent));
	}

	void EditorLayer::OnAttach()
	{

		m_SceneRendererPanel = std::make_unique<SceneRendererPanel>();
		// Editor
		m_CheckerboardTex = Texture2D::Create("assets/editor/Checkerboard.tga");
		m_SceneHierarchyPanel = std::make_unique<SceneHierarchyPanel>(m_EditorScene);

		m_TextEditor.SetLanguageDefinition(GetLang(TextEditorLang::GLSL));
		m_TextEditor.SetShowWhitespaces(false);

		NewScene();

		m_CurrentScene = m_EditorScene;
		m_ViewportRenderer = Ref<SceneRenderer>::Create(m_CurrentScene);
		m_SceneRendererPanel->SetContext(m_ViewportRenderer);
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
		auto [x, y] = GetMouseViewportSpace();
		m_ViewportRenderer->SetFocusPoint({ x * 0.5f + 0.5f, y * 0.5f + 0.5f });

		m_EditorScene->OnUpdate(ts);

		m_EditorCamera.OnUpdate(ts);
		m_EditorScene->OnRenderEditor(m_ViewportRenderer, ts, m_EditorCamera);

		if (m_EditorScene->m_SelectionContext.size())
		{
			auto& selection = m_EditorScene->m_SelectionContext[0];

			RockEngine::Renderer::BeginRenderPass(m_ViewportRenderer->GetFinalRenderPass(), false);
			auto viewProj = m_EditorCamera.GetViewProjection();
			RockEngine::Renderer2D::BeginScene(viewProj, false);
			auto& submesh = m_EditorScene->m_SelectionContext[0];
			Renderer::DrawAABB(selection.Mesh->BoundingBox, selection.Entity->GetTransform() * selection.Mesh->Transform);
			RockEngine::Renderer2D::EndScene();
			RockEngine::Renderer::EndRenderPass();
		}

	}

	float EditorLayer::GetSnapValue()
	{
		switch (m_GizmoType)
		{
		case  ImGuizmo::OPERATION::TRANSLATE: return 0.5f;
		case  ImGuizmo::OPERATION::ROTATE: return 45.0f;
		case  ImGuizmo::OPERATION::SCALE: return 0.5f;
		}
		return 0.0f;
	}

	void EditorLayer::OnImGuiRender()
	{
		// ImGui + Dockspace Setup ------------------------------------------------------------------------------
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		auto boldFont = io.Fonts->Fonts[0];
		auto largeFont = io.Fonts->Fonts[1];

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || (ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
		{
		}

		io.ConfigWindowsResizeFromEdges = io.BackendFlags & ImGuiBackendFlags_HasMouseCursors;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		bool isMaximized = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::Begin("DockSpace Demo", nullptr, window_flags);
		ImGui::PopStyleColor(); // MenuBarBg
		ImGui::PopStyleVar(2);

		ImGui::PopStyleVar(2);

		{

		}
		UI_DrawMenubar();

		ImGui::SetCursorPosY(25); //TODO: 25 is random value, set a calculated value
		auto cursorPos = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(400);

		m_SceneHierarchyPanel->DrawComponentsList();

		ImGui::SetCursorPosY(70); //TODO: 70 is random value, set a calculated value
		ImGui::SetCursorPosX(cursorPos);

		// Dockspace
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		ImGui::DockSpace(ImGui::GetID("MyDockspace"));
		style.WindowMinSize.x = minWinSizeX;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		{
			auto viewportOffset = ImGui::GetCursorPos(); // includes tab bar
			auto viewportSize = ImGui::GetContentRegionAvail();

			m_ViewportRenderer->SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

			m_EditorCamera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 1000.0f));
			m_EditorCamera.SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

			ImGui::Image((void*)m_ViewportRenderer->GetFinalColorBufferRendererID(), viewportSize, { 0, 1 }, { 1, 0 });

			static int counter = 0;
			auto windowSize = ImGui::GetWindowSize();
			ImVec2 minBound = ImGui::GetWindowPos();
			minBound.x += viewportOffset.x;
			minBound.y += viewportOffset.y;

			ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
			m_ViewportBounds[0] = { minBound.x, minBound.y };
			m_ViewportBounds[1] = { maxBound.x, maxBound.y };
		}

		UI_DrawGizmos();

		ImGui::End();
		ImGui::PopStyleVar();

		m_SceneHierarchyPanel->OnImGuiRender();
		m_SceneRendererPanel->OnImGuiRender(false);

		ImGui::End();
	}

	void EditorLayer::SaveSceneAs()
	{
		/*auto& app = Application::Get();
		auto filepath = app.SaveFileDialog("TheRock Scene (*.sctr)\0*.sctr\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(filepath.string());

			UpdateWindowTitle(Utils::GetFilename(filepath.string()));
		}*/
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = Utils::FileSystem::OpenFileDialog("TheRock Scene (*.sctr)\0*.sctr\0").string();
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& filepath)
	{
		Ref<Scene> newScene = Ref<Scene>::Create("New Scene");
		SceneSerializer serializer(newScene);
		serializer.Deserialize(filepath);

		m_EditorScene = newScene;

		UpdateWindowTitle(Utils::FileSystem::GetFileName(filepath));
		m_SceneHierarchyPanel->SetContext(m_EditorScene);

		m_EditorScene->SetSelected({});
		m_EditorScene->m_SelectionContext.clear();
	}

	void EditorLayer::SaveScene()
	{

	}

	void EditorLayer::UI_DrawMenubar()
	{
		ImGui::BeginGroup();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();
				if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
					OpenScene();
				ImGui::Separator();
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					SaveScene();
				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::Separator();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::EndGroup();

	}

	void EditorLayer::UI_DrawGizmos()
	{
		if (m_GizmoType != -1 && m_EditorScene->m_SelectionContext.size())
		{
			auto& selection = m_EditorScene->m_SelectionContext[0];

			float rw = (float)ImGui::GetWindowWidth();
			float rh = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);

			bool snap = Input::IsKeyPressed(KeyCode::LeftControl);

			TransformComponent& entityTransform = selection.Entity->GetComponent<TransformComponent>();
			glm::mat4 transform = entityTransform.GetTransform();

			float snapValue = GetSnapValue();
			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(m_EditorCamera.GetViewMatrix()),
				glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),
				(ImGuizmo::OPERATION)m_GizmoType,
				ImGuizmo::LOCAL,
				glm::value_ptr(selection.Mesh ? selection.Mesh->Transform : transform),
				nullptr,
				snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing() && selection.Mesh)
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - entityTransform.Rotation;
				entityTransform.Translation = translation;
				entityTransform.Rotation += deltaRotation;
				entityTransform.Scale = scale;
			}

		}

	}
}