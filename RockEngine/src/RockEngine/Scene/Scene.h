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

	struct LightEnvironment
	{
		DirectionalLight DirectionalLights[4];
	};

	class Entity;
	using EntityMap = std::unordered_map<UUID, Entity*>;

	class Scene : public RefCounted
	{
	public:
		Scene(const std::string& debugName = "Scene");
		~Scene();

		void Init();

		void OnUpdate(Timestep ts);
		void OnRenderRuntime(Timestep ts);
		void OnRenderEditor(Timestep ts, const EditorCamera& editorCamera);

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
		decltype(auto) GetAllEntitiesWith()
		{
			std::vector<Entity*> out;
			for (const auto& [key, e] : m_EntityIDMap)
				if (e->HasComponent<T>())
					out.push_back(e);
			return out;
		}

		decltype(auto) GetAllEntities()
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
		//std::vector<Entity*> m_Entities;
		EntityMap m_EntityIDMap;
		Entity* m_SelectedEntity;

		float m_LightMultiplier = 0.3f;

		LightEnvironment m_LightEnvironment;

		Environment m_Environment;
		Ref<TextureCube> m_SkyboxTexture; 
		Ref<MaterialInstance> m_SkyboxMaterial;

		float m_SkyboxLod = 1.0f;
		float m_EnvironmentIntensity = 1.0f;

		friend class SceneRenderer;
		friend class SceneHierarchyPanel;
		friend class EditorLayer;
		friend class Entity;
	};
}