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
		for (auto entity : m_Entities)
			delete entity;
	}

	void Scene::Init()
	{
		auto skyboxShader = Shader::Create("assets/shaders/Skybox.glsl");
		m_SkyboxMaterial = MaterialInstance::Create(Material::Create(skyboxShader));
		m_SkyboxMaterial->SetFlag(MaterialFlag::DepthTest, false);
	}

	void Scene::OnUpdate(Timestep ts)
	{
		//m_Camera.OnUpdate(ts);
		//m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLod);

		//// Update all entities
		//for (auto entity : m_Entities)
		//{
		//	//auto mesh = entity->GetComponent<MeshComponent>().Mesh; // TODO: Add a vector what store a all meshes
		//	//if (mesh)
		//	//	mesh->OnUpdate(ts);
		//}

		//SceneRenderer::BeginScene(this);

		//for (auto entity : m_Entities)
		//{
		//	// TODO: Should we render (logically)
		//	SceneRenderer::SubmitEntity(entity);
		//}

		//SceneRenderer::EndScene();
	}

	void Scene::OnRenderRuntime(Timestep ts)
	{

	}

	void Scene::OnRenderEditor(Timestep ts, const EditorCamera& editorCamera)
	{
		{
			m_Environment = Environment();
			for (auto& entity : m_Entities)
				if (entity->HasComponent<SkyLightComponent>())
				{
					auto skyLightComponent = entity->GetComponent<SkyLightComponent>();
					m_Environment = skyLightComponent.SceneEnvironment;
					SetSkybox(m_Environment.RadianceMap);
					break;
				}
		}

		m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLod);

		SceneRenderer::BeginScene(this, { editorCamera, editorCamera.GetViewMatrix(), 0.1f, 1000.0f, 45.0f });
		for (auto& entity : m_Entities)
		{
			if (entity->HasComponent<MeshComponent>())
			{
				auto& meshComponent = entity->GetComponent<MeshComponent>();
				if (meshComponent.Mesh)
				{
					if (entity->HasComponent<TransformComponent>())
					{
						auto& transformComponent = entity->GetComponent<TransformComponent>();

						meshComponent.Mesh->OnUpdate(ts);
						SceneRenderer::SubmitMesh(meshComponent, transformComponent.GetTransform());
					}
				}
			}
		}
		SceneRenderer::EndScene();
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
		m_Entities.push_back(entity);
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
		entity->AddComponent<TransformComponent>();

		m_EntitysCount++;
		AddEntity(entity);

		//m_EntityIDMap[idComponent.ID] = entity;
		return entity;
	}

}