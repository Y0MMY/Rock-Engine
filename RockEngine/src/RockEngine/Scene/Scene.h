#pragma once

#include "RockEngine/Renderer/Camera.h"

#include "RockEngine/Renderer/Mesh.h"

#include <RockEngine/Core/REVector.h>

namespace RockEngine
{
	struct Environment
	{
		Ref<TextureCube> RadianceMap;
		Ref<TextureCube> IrradianceMap;

		static Environment Load(const std::string& filepath);
	};

	struct Light
	{
		glm::vec3 Direction;
		glm::vec3 Radiance;

		float Multiplier = 1.0f;
	};
	class Entity;
	using EntityMap = std::unordered_map<UUID, Entity>;
	class Scene : public RefCounted
	{
	public:
		Scene(const std::string& debugName = "Scene");
		~Scene();

		void Init();

		void OnUpdate(Timestep ts);

		void SetCamera(const Camera& camera);
		Camera& GetCamera() { return m_Camera; }

		void SetEnvironment(const Environment& environment);
		void SetSkybox(const Ref<TextureCube> skybox);
		void SetSelected(Entity* entity);

		Light& GetLight() { return m_Light; }

		float& GetSkyboxLod() { return m_SkyboxLod; }

		void AddEntity(Entity* entity);
		Entity* CreateEntity(const std::string& name = "");
	private:
		REVector m_Registry;
		u32 m_EntitysCount = 0;
		struct SelectedSubmesh
		{
			RockEngine::Entity* Entity;
			Submesh* Mesh = nullptr;
			float Distance = 0.0f;
		};
		std::vector<SelectedSubmesh> m_SelectedSubmeshes;
		std::string m_DebugName;
		std::vector<Entity*> m_Entities;
		Entity* m_SelectionContext;

		Camera m_Camera;
		Light m_Light;
		float m_LightMultiplier = 0.3f;

		Environment m_Environment;
		Ref<TextureCube> m_SkyboxTexture;
		Ref<MaterialInstance> m_SkyboxMaterial;

		float m_SkyboxLod = 1.0f;

		friend class SceneRenderer;
		friend class SceneHierarchyPanel;
		friend class EditorLayer;
		friend class Entity;
	};
}