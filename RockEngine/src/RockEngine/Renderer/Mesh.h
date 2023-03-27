#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "RockEngine/Renderer/RendererAPI.h"

#include <RockEngine/Renderer/Pipeline.h>
#include <RockEngine/Renderer/Texture.h>
#include <RockEngine/Renderer/Texture.h>
#include <RockEngine/Renderer/Material.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Assimp {
	class Importer;
}

namespace RockEngine
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 Texcoord;
	};
	static_assert(sizeof(Vertex) == 14 * sizeof(float));
	static const int NumAttributes = 5;

	struct Index
	{
		uint32_t V1, V2, V3;
	};
	static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

	class Mesh : public RefCounted
	{
	public:
		Mesh(const std::string& filepath);
		~Mesh() {}

		void Render();


		Ref<Shader> GetMeshShader() { return m_MeshShader; }
		Ref<Material> GetMaterial() { return m_BaseMaterial; }
		std::vector<Ref<MaterialInstance>> GetMaterials() { return m_Materials; }
		const std::vector<Ref<Texture2D>>& GetTextures() const { return m_Textures; }
		const std::string& GetFilePath() { return m_FilePath; }
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		Ref<Pipeline> m_Pipeline;
		std::string m_FilePath;

		std::vector<Ref<Texture2D>> m_Textures;
		std::vector<Ref<MaterialInstance>> m_Materials;

		Ref<Material> m_BaseMaterial;
		Ref<Shader> m_MeshShader;
	};
}