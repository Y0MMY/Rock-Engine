#pragma once

#include "RockEngine/Scene/Scene.h"
#include "RockEngine/Scene/Components.h"

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

	class SceneRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnImGuiRender();

		static void SetViewportSize(u32 width, u32 height);

		static void BeginScene(const Scene*	scene, const SceneRendererCamera& camera);
		static void EndScene();

		static void SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), Ref<MaterialInstance> overrideMaterial = nullptr);
		static void SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f));

		static SceneRendererOptions& GetOptions();

		static std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::filesystem::path& filepath);

		static Ref<Texture2D> GetFinalColorBuffer();
		static Ref<RenderPass> GetFinalRenderPass();	

		static void SetFocusPoint(const glm::vec2& point);

		// TODO: Temp
		static uint32_t GetFinalColorBufferRendererID();
	private:
		static void FlushDrawList();
		static void GeometryPass();
		static void CompositePass();
		static void ShadowMapPass();

		Ref<Texture2D> BRDFLUT;

	};
}