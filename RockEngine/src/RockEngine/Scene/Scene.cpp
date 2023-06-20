#include "pch.h"
#include "Scene.h"

#include "RockEngine/Renderer/SceneRenderer.h"
#include "RockEngine/Scene/Entity.h"

namespace RockEngine
{
	static const std::string DefaultEntityName = "Entity";

	Scene::Scene(const std::string& debugName)
		: m_DebugName(debugName), m_SelectedEntity(nullptr)
	{
		Init();

	}

	Scene::~Scene()
	{
		
	}

	void Scene::Init()
	{
		auto skyboxShader = Shader::Create("assets/shaders/Skybox.glsl");
		m_SkyboxMaterial = MaterialInstance::Create(Material::Create(skyboxShader));
		m_SkyboxMaterial->SetFlag(MaterialFlag::DepthTest, false);
	}

	void Scene::OnUpdate(Timestep ts)
	{
		
	}

	void Scene::OnRenderRuntime(Ref<SceneRenderer> renderer,Timestep ts)
	{

	}

	void Scene::OnRenderEditor(Ref<SceneRenderer> renderer, Timestep ts, const EditorCamera& editorCamera)
	{

		// Directional Lights	
		{
			m_LightEnvironment = LightEnvironment();
			auto lights = GetAllEntitiesWith<DirectionalLightComponent>();
			uint32_t directionalLightIndex = 0;

			for (const auto e : lights)
			{
				if (!e->GetComponent<RendererComponent>().Visible)
					continue;
				auto transform = e->GetComponent<TransformComponent>().GetTransform();
				auto lightComponent = e->GetComponent<DirectionalLightComponent>();

				glm::vec3 direction = -glm::normalize(glm::mat3(transform) * glm::vec3(1.0f));
				m_LightEnvironment.DirectionalLights[directionalLightIndex++] =
				{
					direction,
					lightComponent.Radiance,
					lightComponent.Intensity,
					lightComponent.CastShadows
				};
			}
		}

		// Point Lights
		{
			auto pointLights = m_Registry.group<PointLightComponent>();
			auto pointlightEntity = GetAllEntitiesWith<PointLightComponent>();
			if (pointLights.size() != m_LightEnvironment.PointLights.size())
				m_LightEnvironment.PointLights.resize(pointLights.size());

			uint32_t pointLightIndex = 0;
			for (auto e : pointLights)
			{
				if (!pointlightEntity[pointLightIndex]->GetComponent<RendererComponent>().Visible)
					continue;
				auto transform = pointlightEntity[pointLightIndex]->GetComponent<TransformComponent>();
				m_LightEnvironment.PointLights[pointLightIndex++] = {
							transform.Translation,
							e->Intensity,
							e->Radiance,
							e->MinRadius,
							e->Radius,
							e->Falloff,
							e->LightSize,
				};

			}
		}

		{
			m_Environment = Environment();
			for (const auto& [key, entity] : m_EntityIDMap)
				if (entity->HasComponent<SkyLightComponent>())
				{
					if (!entity->GetComponent<RendererComponent>().Visible)
						continue;
					auto skyLightComponent = entity->GetComponent<SkyLightComponent>();
					m_Environment = skyLightComponent.SceneEnvironment;
					m_EnvironmentIntensity = skyLightComponent.Intensity;
					SetSkybox(m_Environment.RadianceMap);
					break;
				}

				m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLod);
		}


		renderer->SetScene(this);
		renderer->BeginScene({ editorCamera, editorCamera.GetViewMatrix(), 0.1f, 1000.0f, 45.0f });
		for (const auto& [key, entity] : m_EntityIDMap)
		{
			if(!entity->GetComponent<RendererComponent>().Visible) 
				continue;
			if (entity->HasComponent<MeshComponent>())
			{
				auto& meshComponent = entity->GetComponent<MeshComponent>();
				if (meshComponent.Mesh)
				{
					auto& transformComponent = entity->GetComponent<TransformComponent>();

					meshComponent.Mesh->OnUpdate(ts);
					if (m_SelectedEntity == entity && renderer->GetOptions().DrawOutline)
						renderer->SubmitSelectedMesh(meshComponent, transformComponent.GetTransform());
					else
						renderer->SubmitMesh(meshComponent, transformComponent.GetTransform());
				}
			}
		}
		renderer->EndScene();
	}

	void Scene::SetSelected(Entity* entity)
	{
		m_SelectedEntity = entity;
	}

	void Scene::SetEnvironment(const Environment& environment)
	{
		m_Environment = environment;
		SetSkybox(environment.RadianceMap);
	}

	void Scene::SetSkybox(const Ref<TextureCube> skybox)
	{
		m_SkyboxTexture = skybox;
		m_SkyboxMaterial->Set("u_Texture", skybox);
	}

	void Scene::AddEntity(Entity* entity)
	{
		//m_Entities.push_back(entity);
		m_EntityIDMap[entity->GetComponent<IDComponent>().ID] = entity;
	}

	Entity* Scene::CreateEntity(const std::string& name /* = "" */)
	{
		Entity* entity = new Entity(name);
		entity->m_Scene = this;
		entity->m_Handle = m_EntitysCount;

		auto& idComponent = entity->AddComponent<IDComponent>();
		idComponent.ID = {};

		if (!name.empty())
			entity->AddComponent<TagComponent>(name);
		entity->AddComponent<TransformComponent>(); // NOTE: Entity always has TransformComponent
		entity->AddComponent<RendererComponent>();  // NOTE: Entity always has RendererComponent

		m_EntitysCount++;
		AddEntity(entity);

		//m_EntityIDMap[idComponent.ID] = entity;
		return entity;
	}

	Entity* Scene::FindEntityByUUID(const UUID& uuid)
	{
		RE_CORE_ASSERT(m_EntityIDMap.find(uuid) == m_EntityIDMap.end());
		return m_EntityIDMap[uuid];
	}

	Entity* Scene::CreateEntityWithID(UUID uuid, const std::string& name)
	{
		RE_CORE_ASSERT(m_EntityIDMap.find(uuid) == m_EntityIDMap.end());

		Entity* entity = new Entity(name);
		entity->m_Scene = this;
		entity->m_Handle = m_EntitysCount;

		auto& idComponent = entity->AddComponent<IDComponent>();
		idComponent.ID = uuid;

		if (!name.empty())
			entity->AddComponent<TagComponent>(name);
		entity->AddComponent<TransformComponent>(); // NOTE: Entity always has TransformComponent
		entity->AddComponent<RendererComponent>();  // NOTE: Entity always has RendererComponent

		m_EntitysCount++;
		AddEntity(entity);
		return entity;
	}

}