#pragma once

#include <TheRock.h>
#include "RockEngine/Core/Layer.h"
#include "GLFW/include/GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TextEditor/TextEditor.h"

namespace RockEngine
{
	using namespace Memory;
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
		void OpenScene(const std::filesystem::path& filepath);
		void SaveScene();
		void SaveSceneAs();

		std::pair<float, float> GetMouseViewportSpace();
		std::pair<glm::vec3, glm::vec3> CastRay(float mx, float my);
	public:
		void ShowBoundingBoxes(bool show, bool onTop);

		void UpdateWindowTitle(const std::string& sceneName);
	private:
		float GetSnapValue();

		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	private:
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

		EditorCamera m_EditorCamera;

		float m_Exposure = 1.0f;
		bool m_RadiancePrefilter = false;

		float m_EnvMapRotation = 0.0f;

		Ref<Scene> m_EditorScene;
		std::unique_ptr<SceneHierarchyPanel> m_SceneHierarchyPanel;

		int m_GizmoType = -1; // -1 = no gizmo

		bool m_UIShowBoundingBoxes;

		TextEditor m_TextEditor;
		bool m_EditShaderText = false;

		enum class TextEditorLang
		{
			GLSL = 0, HLSL = 1, CPP = 2
		};

		struct ShaderFileProps
		{
			Ref<RockEngine::Shader> Shader;
			std::filesystem::path Path;
			std::string Text;
		};

		ShaderFileProps m_ShaderStruct;

		auto GetLang(TextEditorLang lang)
		{
			switch (lang)
			{
				case TextEditorLang::GLSL:
						return TextEditor::LanguageDefinition::GLSL();

				case TextEditorLang::HLSL:
					return TextEditor::LanguageDefinition::HLSL();

				case TextEditorLang::CPP:
					return TextEditor::LanguageDefinition::CPlusPlus();
			}
			RE_CORE_ASSERT(false);
			return TextEditor::LanguageDefinition();
		}

	};
}