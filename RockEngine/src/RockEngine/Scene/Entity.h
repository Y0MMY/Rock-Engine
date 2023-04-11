#pragma once

#include "RockEngine/Renderer/Mesh.h"
#include "RockEngine/Core/UUID.h"

#include "RockEngine/Scene/Components.h"
#include "RockEngine/Scene/Scene.h"

namespace RockEngine
{
	class Scene;
	using EntityID = uint32_t;
	class Entity
	{
	public:
		~Entity() {}

		template<typename T>
		bool HasComponent()
		{
			RE_CORE_ASSERT(m_Scene);
			return m_Scene->m_Registry.has<T>(m_Handle);
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&...args)
		{
			RE_CORE_ASSERT(m_Scene);
			RE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<T>(m_Handle,std::forward<Args>(args)...);
		}

		template<typename T>
		decltype(auto) GetComponent()
		{
			RE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
			return m_Scene->m_Registry.get<T>(m_Handle);
		}

		template<typename T>
		void RemoveComponent()
		{
			RE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
			m_Scene->m_Registry.remove<T>(m_Handle);
		}

		//TODO: Move to Component
		/*void SetMesh(const Ref<Mesh>& mesh) { m_Mesh = mesh; }
		Ref<Mesh>& GetMesh() { return m_Mesh; }

		void SetMaterial(const Ref<MaterialInstance>& material) { m_Material = material; }
		Ref<MaterialInstance>& GetMaterial() { return m_Material; }*/

		auto Transform() { return m_Scene->m_Registry.get<TransformComponent>(m_Handle); }
		const glm::mat4& Transform() const { return m_Scene->m_Registry.get<TransformComponent>(m_Handle).GetTransform(); }
		const glm::mat4& GetTransform() const { return m_Scene->m_Registry.get<TransformComponent>(m_Handle).GetTransform(); }

		UUID GetUUID() { return m_Handle; }
		const std::string& GetName() const { return m_Name; }
	private:
		Entity(const std::string& name);
	private:
		std::string m_Name;
		glm::mat4 m_Transform;
		EntityID m_Handle;
		Scene* m_Scene = nullptr;

		// TODO: Temp
		Ref<Mesh> m_Mesh;
		Ref<MaterialInstance> m_Material;
		friend class Scene;
		friend class SceneHierarchyPanel;
	};
}
