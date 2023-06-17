#include "pch.h"
#include "SceneSerializer.h"

#include "RockEngine/Core/Math/Math.h"
#include "RockEngine/Utilities/StringUtils.h"

#include "RockEngine/Renderer/SceneRenderer.h"

#include "Components.h"

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}

	};

	template<>
	struct convert<glm::quat>
	{
		static Node encode(const glm::quat& rhs)
		{
			Node node;
			node.push_back(rhs.w);
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::quat& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.w = node[0].as<float>();
			rhs.x = node[1].as<float>();
			rhs.y = node[2].as<float>();
			rhs.z = node[3].as<float>();
			return true;
		}
	};

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
			auto mesh = entity->GetComponent<MeshComponent>().Mesh;
			if (mesh)
			{
				out << YAML::Key << "MeshComponent";
				out << YAML::BeginMap; // MeshComponent

				out << YAML::Key << "AssetPath" << YAML::Value << mesh->GetFilePath().string();

				out << YAML::EndMap; // MeshComponent
			}
		}

		if (entity->HasComponent<DirectionalLightComponent>())
		{
			out << YAML::Key << "DirectionalLightComponent";
			out << YAML::BeginMap; // DirectionalLightComponent

			auto& directionalLightComponent = entity->GetComponent<DirectionalLightComponent>();
			out << YAML::Key << "Radiance" << YAML::Value << directionalLightComponent.Radiance;
			out << YAML::Key << "LightSize" << YAML::Value << directionalLightComponent.LightSize;

			out << YAML::EndMap; // DirectionalLightComponent
		}

		if (entity->HasComponent<SkyLightComponent>())
		{
			out << YAML::Key << "SkyLightComponent";
			out << YAML::BeginMap; // SkyLightComponent

			auto& skyLightComponent = entity->GetComponent<SkyLightComponent>();
			out << YAML::Key << "EnvironmentAssetPath" << YAML::Value << skyLightComponent.FilePath;
			out << YAML::Key << "Intensity" << YAML::Value << skyLightComponent.Intensity;
			out << YAML::Key << "Angle" << YAML::Value << skyLightComponent.Angle;

			out << YAML::EndMap; // SkyLightComponent
		}
		out << YAML::EndMap; // Entity
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{

	}
		
	void SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene";
		out << YAML::Value << "Scene Name";

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

	void SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return;

		std::string sceneName = data["Scene"].as<std::string>();
		RE_CORE_INFO("Deserializing scene '{0}'", sceneName);
		m_Scene->SetName(sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				RE_CORE_INFO("Deserialized entity with ID = {0}, name = {1}", uuid, name);
				Entity* deserializedEntity = m_Scene->CreateEntityWithID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& transform = deserializedEntity->GetComponent<TransformComponent>();
					transform.Translation = transformComponent["Position"].as<glm::vec3>();
					auto& rotationNode = transformComponent["Rotation"];
					// Rotations used to be stored as quaternions
					if (rotationNode.size() == 4)
					{
						glm::quat rotation = transformComponent["Rotation"].as<glm::quat>();
						transform.Rotation = glm::eulerAngles(rotation);
					}
					else
					{
						RE_CORE_ASSERT(rotationNode.size() == 3);
						transform.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					}
					transform.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					std::string meshPath = meshComponent["AssetPath"].as<std::string>();
					
					if (!deserializedEntity->HasComponent<MeshComponent>())
						deserializedEntity->AddComponent<MeshComponent>(Ref<Mesh>::Create(meshPath));

					RE_CORE_INFO("  Mesh Asset Path: {0}", meshPath);
				}

				auto skyLightComponent = entity["SkyLightComponent"];
				if (skyLightComponent)
				{
					auto& component = deserializedEntity->AddComponent<SkyLightComponent>();
					std::string env = skyLightComponent["EnvironmentAssetPath"].as<std::string>();
					if (!env.empty())
					{
						component.SceneEnvironment = SceneRenderer::CreateEnvironmentMap(env);
					}
					component.Intensity = skyLightComponent["Intensity"].as<float>();
					component.Angle = skyLightComponent["Angle"].as<float>();
				}

				auto directionalLightComponent = entity["DirectionalLightComponent"];
				if (directionalLightComponent)
				{
					auto& component = deserializedEntity->AddComponent<DirectionalLightComponent>();
					component.Radiance = directionalLightComponent["Radiance"].as<glm::vec3>();
					component.LightSize = directionalLightComponent["LightSize"].as<float>();
				}
			}
		}
	}

}