#pragma once 

#include <glm/glm.hpp>

namespace RockEngine
{
	struct AABB
	{
		glm::vec3 Min, Max;

		AABB()
			: Min(0.f), Max(0.f)
		{}

		AABB(const glm::vec3& min, const glm::vec3& max)
			: Min(min), Max(max)
		{
		}

	};
}