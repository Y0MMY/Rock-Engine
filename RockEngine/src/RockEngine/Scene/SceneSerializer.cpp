#include "pch.h"
#include "SceneSerializer.h"

#include "RockEngine/Core/Math/Math.h"

#include "Components.h"

namespace YAML
{
	Emitter& operator<<(Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
}

namespace RockEngine
{
	void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity* entity)
	{
		auto uuid = entity->GetUUID();
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity";
		out << YAML::Value << uuid;

		if (entity->HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; //TagComponent;

			auto tag = entity->GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity->HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; //TransformComponent;

			auto& transform = entity->GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << transform.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity->HasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap; // MeshComponent

			auto mesh = entity->GetComponent<MeshComponent>().Mesh;
			out << YAML::Key << "AssetPath" << YAML::Value << mesh->GetFilePath();

			out << YAML::EndMap; // MeshComponent
		}
		out << YAML::EndMap; 
	}

	void SceneSerializer::SerializeEnvironment(YAML::Emitter& out, const Ref<Scene>& scene)
	{
		out << YAML::Key << "Environment"; 
		out << YAML::Value;
		out << YAML::BeginMap; // Environment
		out << YAML::Key << "AssetPath" << YAML::Value << scene->GetEnvironment().FilePath;
		out << YAML::EndMap; // Environment
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{

	}
		
	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene";
		out << YAML::Value << "Scene Name";

		SerializeEnvironment(out, m_Scene);

		out << YAML::Key << "Entities";
		out << YAML::Value << YAML::BeginSeq;

		auto entitys = m_Scene->GetAllEntitiesWith<IDComponent>();
		for (auto entity : entitys)
		{
			SerializeEntity(out, entity);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

}