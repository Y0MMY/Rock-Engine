#pragma once

#include "RockEngine/Renderer/Mesh.h"

namespace RockEngine
{
	class Entity
	{
	public:
		Entity();
		~Entity();

		void SetMesh(const Ref<Mesh>& mesh);
		Ref<Mesh>& GetMesh() { return m_Mesh; }

		void SetMaterial(const Ref<MaterialInstance>& material) { m_Material = material; }
		Ref<MaterialInstance>& GetMaterial() { return m_Material; }

		const glm::mat4& GetTransform() const { return m_Transform; }
		glm::mat4& Transform() { return m_Transform; }
	private:
		glm::mat4 m_Transform;

		// TODO: Temp
		Ref<Mesh> m_Mesh;
		Ref<MaterialInstance> m_Material;
	};
}
