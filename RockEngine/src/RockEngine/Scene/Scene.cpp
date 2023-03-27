#include "pch.h"
#include "Scene.h"

#include "RockEngine/Renderer/SceneRenderer.h"

namespace RockEngine
{

	Environment Environment::Load(const std::string& filepath)
	{
		auto [rad, irrad] = SceneRenderer::CreateEnvironmentMap(filepath);
		return { rad, irrad };
	}

	Scene::Scene(const std::string& debugName)
		: m_DebugName(debugName)
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
		m_Camera.Update(ts);
		m_SkyboxMaterial->Set("u_TextureLod", 0.0);

		// Update all entities
		for (auto entity : m_Entities)
		{
			auto mesh = entity->GetMesh();
			/*if (mesh)
				mesh->OnUpdate(ts);*/
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

	Entity* Scene::CreateEntity()
	{
		auto entity = new Entity();
		AddEntity(entity);
		return entity;
	}

}