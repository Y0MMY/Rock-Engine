#pragma once

#include "RockEngine/Editor/EditorCamera.h"

#include "RockEngine/Renderer/Mesh.h"
#include "RockEngine/Renderer/SceneEnvironment.h"

#include "Components.h"
#include <RockEngine/Core/REVector.h>

namespace RockEngine
{
	struct DirectionalLight
	{
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };
		float Multiplier = 0.0f;

		// C++ only
		bool CastShadows = true;
	};

	struct PointLight
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Intensity = 0.0f;
		glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };
		float MinRadius = 0.001f;
		float Radius = 25.0f;
		float Falloff = 1.f;
		float SourceSize = 0.1f;
	};

	struct LightEnvironment
	{
		DirectionalLight DirectionalLights[4];
		std::vector<PointLight> PointLights;
	};

	class Entity;
	class SceneRenderer;
	using EntityMap = std::unordered_map<UUID, Entity*>;

	class Scene : public RefCounted 
	{
	public:
		Scene(const std::string& debugName = "Scene");
		~Scene();

		void Init();

		void OnUpdate(Timestep ts);
		void OnRenderRuntime(Ref<SceneRenderer> renderer, Timestep ts);
		void OnRenderEditor(Ref<SceneRenderer> renderer, Timestep ts, const EditorCamera& editorCamera);

		void SetName(const std::string& name) { m_DebugName = name; }
		const std::string& GetName() const { return m_DebugName; }

		void SetEnvironment(const Environment& environment);
		void SetSkybox(const Ref<TextureCube> skybox);
		void SetSelected(Entity* entity);

		float& GetSkyboxLod() { return m_SkyboxLod; }

		const Environment& GetEnvironment() const { return m_Environment; }
		const LightEnvironment& GetLight() const { return m_LightEnvironment; }
		LightEnvironment& GetLight() { return m_LightEnvironment; }

		template<typename T>
		decltype(auto) GetAllEntitiesSceneWith()
		{
			std::vector<Entity*> out;
			for (const auto& [key, e] : m_EntityIDMap)
				if (e->HasComponent<T>())
					out.push_back(e);
			return out;
		}

		decltype(auto) GetAllEntitiesScene()
		{
			return m_EntityIDMap;
		}

		void AddEntity(Entity* entity);
		Entity* CreateEntity(const std::string& name = "");
		Entity* CreateEntityWithID(UUID uuid, const std::string& name = "");
		Entity* FindEntityByUUID(const UUID& uuid);
	private:
		REVector m_Registry;
		u32 m_EntitysCount = 0;
		struct SelectedContext
		{
			RockEngine::Entity* Entity;
			Submesh* Mesh = nullptr;
			float Distance = 0.0f;
		};
		std::vector<SelectedContext> m_SelectionContext;
		std::string m_DebugName;
		EntityMap m_EntityIDMap;
		Entity* m_SelectedEntity;

		float m_LightMultiplier = 0.3f;

		LightEnvironment m_LightEnvironment;

		Environment m_Environment;
		Ref<TextureCube> m_SkyboxTexture; 
		Ref<MaterialInstance> m_SkyboxMaterial;

		float m_SkyboxLod = 1.0f;
		float m_EnvironmentIntensity = 1.0f;
	private:
		friend class SceneRenderer;
		friend class SceneHierarchyPanel;
		friend class SceneRendererPanel;
		friend class EditorLayer;
		friend class Entity;
	};
}