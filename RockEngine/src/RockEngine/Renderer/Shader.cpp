#include "pch.h"
#include "Shader.h"

#include "RockEngine/Platform/OpenGL/OpenGLShader.h"

namespace RockEngine
{
	Shader* Shader::Create(const std::string& path)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::OpenGL: return new OpenGLShader(path);
		}
		RE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}