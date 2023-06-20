#pragma once

#include <cmath>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Binormal;
	glm::vec2 Texcoord;
};

struct Index
{
	uint32_t V1, V2, V3;
};

namespace RockEngine::Math
{
	RockEngine::Ref<RockEngine::Mesh> CreateSphere(float radius)
	{
		std::vector<Vertex> vertices;
		std::vector<Index> indices;

		constexpr float latitudeBands = 30;
		constexpr float longitudeBands = 30;

		for (float latitude = 0.0F; latitude <= latitudeBands; latitude++)
		{
			float theta = latitude * M_PI / latitudeBands;
			float sinTheta = glm::sin(theta);
			float cosTheta = glm::cos(theta);

			for (float longitude = 0.0F; longitude <= longitudeBands; longitude++)
			{
				float phi = longitude * 2 * M_PI / longitudeBands;
				float sinPhi = glm::sin(phi);
				float cosPhi = glm::cos(phi);

				Vertex vertex;
				vertex.Normal = { cosPhi * sinTheta, cosTheta, sinPhi * sinTheta };
				vertex.Position = { radius * vertex.Normal.x, radius * vertex.Normal.y, radius * vertex.Normal.z };
				vertices.push_back(vertex);
			}
		}

		for (uint32_t latitude = 0; latitude < latitudeBands; latitude++)
		{
			for (uint32_t longitude = 0; longitude < longitudeBands; longitude++)
			{
				uint32_t first = (latitude * (longitudeBands + 1)) + longitude;
				uint32_t second = first + longitudeBands + 1;

				indices.push_back({ first, second, first + 1 });
				indices.push_back({ second, second + 1, first + 1 });
			}
		}

		return Ref<Mesh>::Create(vertices, indices, glm::mat4(1.0F));
	}
}