#pragma once

#include "Entity.h"

#include "yaml-cpp/yaml.h"

namespace RockEngine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::string& filepath);

		void Deserialize();
	private:
		void SerializeEntity(YAML::Emitter& out, Entity* entity);
		void SerializeEnvironment(YAML::Emitter& out, const Ref<Scene>& scene);
		Ref<Scene> m_Scene;
	};
}
