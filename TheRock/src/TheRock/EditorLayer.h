#pragma once

#include <TheRock.h>
#include "RockEngine/Core/Layer.h"
#include "GLFW/include/GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RockEngine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual void OnAttach();
		virtual void OnDetach() { }
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& e) override;
		virtual void OnImGuiRender();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::string& filepath);
		void SaveScene();
		void SaveSceneAs();

		std::pair<float, float> GetMouseViewportSpace();
		std::pair<glm::vec3, glm::vec3> CastRay(float mx, float my);
	public:
		void ShowBoundingBoxes(bool show, bool onTop);

		void UpdateWindowTitle(const std::string& sceneName);
	private:
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	private:
		Ref<RockEngine::Shader> m_Shader;
		Ref<RockEngine::Shader> m_PBRShader;
		Ref<RockEngine::Shader> m_SimplePBRShader;
		Ref<RockEngine::Shader> m_QuadShader;
		Ref<RockEngine::Shader> m_HDRShader;
		Ref<RockEngine::Mesh> m_Mesh;
		Ref<RockEngine::Mesh> m_SphereMesh;
		Entity* m_MeshEntity = nullptr;
		glm::vec2 m_ViewportBounds[2];

		Camera m_Camera;

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
		Ref<Framebuffer> m_Framebuffer, m_FinalPresentBuffer;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<TextureCube> m_EnvironmentCubeMap, m_EnvironmentIrradiance;
		float m_ClearColor[4];
		struct Light
		{
			glm::vec3 Direction;
			glm::vec3 Radiance;
		};
		Light m_Light;
		float m_LightMultiplier = 0.3f;

		EditorCamera m_EditorCamera;

		Ref<Pipeline> m_Pipeline;

		float m_Exposure = 1.0f;
		bool m_RadiancePrefilter = false;

		float m_EnvMapRotation = 0.0f;

		Ref<Material> m_MeshMaterial;

		Ref<Scene> m_EditorScene;
		std::unique_ptr<SceneHierarchyPanel> m_SceneHierarchyPanel;

		float m_SnapValue = 0.5f;

		glm::mat4* m_CurrentlySelectedTransform = nullptr;

		int m_GizmoType = -1; // -1 = no gizmo

		bool m_UIShowBoundingBoxes;
	};
}