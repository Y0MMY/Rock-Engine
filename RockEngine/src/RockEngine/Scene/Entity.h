#pragma once

#include "RockEngine/Renderer/Mesh.h"

namespace RockEngine
{
	class Scene;
	class Entity
	{
	public:
		~Entity() {}

		template<typename T>
		bool HasComponent()
		{
			RE_CORE_ASSERT(m_Scene);
			return m_Scene->m_Registry.has<T>();
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&...args)
		{
			RE_CORE_ASSERT(m_Scene);
			RE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<T>(std::forward<Args>(args)...);
		}

		template<typename T>
		decltype(auto) GetComponent()
		{
			RE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
			auto& component = m_Scene->m_Registry.get<T>(0);
			return component;
		}

		template<typename T>
		void RemoveComponent()
		{
			RE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
			m_Scene->m_Registry.remove<T>();
		}

		//TODO: Move to Component
		void SetMesh(const Ref<Mesh>& mesh) { m_Mesh = mesh; }
		Ref<Mesh>& GetMesh() { return m_Mesh; }

		void SetMaterial(const Ref<MaterialInstance>& material) { m_Material = material; }
		Ref<MaterialInstance>& GetMaterial() { return m_Material; }

		const glm::mat4& GetTransform() const { return m_Transform; }
		glm::mat4& Transform() { return m_Transform; }
		const std::string& GetName() const { return m_Name; }
	private:
		Entity(const std::string& name);
	private:
		std::string m_Name;
		glm::mat4 m_Transform;

		Scene* m_Scene = nullptr;

		// TODO: Temp
		Ref<Mesh> m_Mesh;
		Ref<MaterialInstance> m_Material;
		friend class Scene;
	};
}
