#pragma once

#include "RockEngine/Scene/Scene.h"
#include "RockEngine/Scene/Components.h"
#include <RockEngine/Core/Timer.h>

#include "RenderPass.h"

namespace RockEngine
{
	struct SceneRendererOptions
	{
		bool ShowGrid = false;
		bool ShowBoundingBoxes = false;
		bool DrawOutline = false;
	};

	struct SceneRendererCamera
	{
		RockEngine::Camera Camera;
		glm::mat4 ViewMatrix;
		float Near, Far;
		float FOV;
	};

	struct CascadeData
	{
		glm::mat4 ViewProj;
		glm::mat4 View;
		float SplitDepth;
	};

	struct SceneRendererStats
	{
		float ShadowPass = 0.0f;
		float GeometryPass = 0.0f;
		float CompositePass = 0.0f;

		Timer ShadowPassTimer;
		Timer GeometryPassTimer;
		Timer CompositePassTimer;
	};

	class SceneRenderer : public RefCounted
	{
	public:
		SceneRenderer(const Ref<Scene>& scene);
		virtual ~SceneRenderer();
		void Init();
		void Shutdown();
		void OnImGuiRender();
		void SetViewportSize(u32 width, u32 height);
		void BeginScene(const SceneRendererCamera& camera);
		void EndScene();

		void SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), Ref<MaterialInstance> overrideMaterial = nullptr, bool depthTest = false);
		void SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), bool depthTest = false);
		void SubmitMeshWithShader(Ref<Mesh> mesh, const glm::mat4& transform, Ref<Shader> shader);

		void SubmitSoliderSphere(const SphereColliderComponent& component, const glm::mat4& transform = glm::mat4(1.0f), bool depthTest = false);

		SceneRendererOptions& GetOptions();
		static Environment CreateEnvironmentMap(const std::filesystem::path& filepath);
		Ref<Texture2D> GetFinalColorBuffer();
		Ref<RenderPass> GetFinalRenderPass();	
		void SetFocusPoint(const glm::vec2& point);

		void SetScene(Ref<Scene> scene) { RE_CORE_ASSERT(!m_Active, "Can't change scenes while rendering"); m_Scene = scene; }

		// TODO: Temp
		uint32_t GetFinalColorBufferRendererID();
	private:
		void FlushDrawList();
		void GeometryPass();
		void CompositePass();
		void ShadowMapPass();

		void CalculateCascades(CascadeData* cascades, const glm::vec3& lightDirection);
	private:
		Ref<Scene> m_Scene;
		bool m_Active = false;

		size_t m_ViewportWidth = 1280, m_ViewportHeight = 720;
		glm::vec2 m_FocusPoint = {};

		SceneRendererStats m_Stats;
	private:
		Ref<Texture2D> m_BRDFLUT;
	private:
		struct SceneInfo
		{
			SceneRendererCamera SceneCamera;

			// Resources
			Ref<MaterialInstance> SkyboxMaterial;
			Environment SceneEnvironment;
			float SceneEnvironmentIntensity;
			LightEnvironment SceneLightEnvironment;
		} m_SceneData;

		Ref<Shader> m_CompositeShader;

		Ref<Shader> m_ShadowMapShader;
		Ref<Shader> m_ShadowMapAnimShader;

		Ref<Shader> m_ColiderSphereSphere;

		Ref<RenderPass> m_ShadowMapRenderPass[4];
		Ref<RenderPass> m_GeoPass;
		Ref<RenderPass> m_CompositePass;

		// Shadows Map
		struct ShadowMapSettings
		{
			Ref<Shader> ShadowMapShader;
			Ref<RenderPass> ShadowMapRenderPass[4];
			float ShadowMapSize = 20.0f;
			float LightDistance = 0.1f;
			glm::mat4 LightMatrices[4];
			glm::mat4 LightViewMatrix;
			float CascadeSplitLambda = 0.91f;
			glm::vec4 CascadeSplits;
			float CascadeFarPlaneOffset = 15.0f, CascadeNearPlaneOffset = -15.0f;
			bool ShowCascades = false;
			bool SoftShadows = true;
			float LightSize = 0.5f;
			float MaxShadowDistance = 200.0f;
			float ShadowFade = 25.0f;
			float CascadeTransitionFade = 1.0f;
			bool CascadeFading = true;

			bool EnableBloom = false;
			float BloomThreshold = 1.5f;

		} ShadowSettings;

		RendererID m_ShadowMapSampler = 0;

		struct DrawCommand
		{
			Ref<Mesh> Mesh;
			Ref<MaterialInstance> Material;
			glm::mat4 Transform;
			bool DepthTest;
		};

		struct DrawCommandWithShader
		{
			Ref<Mesh> Mesh;
			Ref<Shader> Shader;
			glm::mat4 Transform;
		};

		std::vector<DrawCommandWithShader> m_DrawListWithShader;

		std::vector<DrawCommand> m_DrawList;
		std::vector<DrawCommand> m_DrawColiderSphere;
		std::vector<DrawCommand> m_SelectedMeshDrawList;
		std::vector<DrawCommand> m_ShadowPassDrawList;
		SceneRendererOptions m_Options;

		// Grid
		Ref<MaterialInstance> m_GridMaterial;

		// Outline
		Ref<MaterialInstance> m_OutlineMaterial, m_OutlineAnimMaterial;

		// SphereSun
		Ref<MaterialInstance> m_ColiderSphereMaterial;
	private:
		friend class SceneRendererPanel;
	};
}