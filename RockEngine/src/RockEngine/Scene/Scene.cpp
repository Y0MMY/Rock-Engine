#include "pch.h"
#include "Scene.h"

#include "RockEngine/Renderer/SceneRenderer.h"
#include "RockEngine/Scene/Entity.h"

namespace RockEngine
{
	static const std::string DefaultEntityName = "Entity";

	Environment Environment::Load(const std::string& filepath)
	{
		auto [rad, irrad] = SceneRenderer::CreateEnvironmentMap(filepath);
		return { rad, irrad };
	}

	Scene::Scene(const std::string& debugName)
		: m_DebugName(debugName), m_SelectionContext(nullptr)
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
		m_Camera.OnUpdate(ts);
		m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLod);

		// Update all entities
		for (auto entity : m_Entities)
		{
			auto mesh = entity->GetMesh();
			if (mesh)
				mesh->OnUpdate(ts);
		}

		SceneRenderer::BeginScene(this);

		for (auto entity : m_Entities)
		{
			// TODO: Should we render (logically)
			SceneRenderer::SubmitEntity(entity);
		}

		SceneRenderer::EndScene();
	}

	void Scene::SetCamera(const Camera& camera)
	{
		m_Camera = camera;
	}

	void Scene::SetSelected(Entity* entity)
	{
		m_SelectionContext = entity;
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

		auto& idComponent = entity->AddComponent<IDComponent>();
		idComponent.ID = entity->m_Handle;

		if (!name.empty())
			entity->AddComponent<TagComponent>(name);
		entity->AddComponent<TransformComponent>();

		m_EntitysCount++;
		AddEntity(entity);

		//m_EntityIDMap[idComponent.ID] = entity;
		return entity;
	}

}