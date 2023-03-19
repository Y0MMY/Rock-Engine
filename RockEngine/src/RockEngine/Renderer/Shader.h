#pragma once

#include "RockEngine/Renderer/Renderer.h"

namespace RockEngine
{
	class Shader
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
	
		static Shader* Create(const std::string& path);
	};
}