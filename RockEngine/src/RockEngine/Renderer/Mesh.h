#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <RockEngine/Platform/OpenGL/OpenGLPipeline.h>

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
		const std::string& GetFilePath() { return m_FilePath; }
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		Ref<Pipeline> m_Pipeline;
		std::string m_FilePath;
	};
}