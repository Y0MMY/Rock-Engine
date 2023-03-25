#include "pch.h"
#include "Shader.h"

#include "RockEngine/Platform/OpenGL/OpenGLShader.h"

namespace RockEngine
{
	std::vector<Ref<Shader>> Shader::s_AllShaders;

	Ref<Shader> Shader::Create(const std::string& path)
	{
		Ref<Shader> shader = nullptr;
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::OpenGL: shader = Ref<OpenGLShader>::Create(path);
		}
		s_AllShaders.push_back(shader);
		return shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		RE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(),"");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Ref<Shader>(Shader::Create(filepath));
		auto& name = shader->GetName();
		RE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(),"");
		m_Shaders[name] = shader;
	}
	void ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		RE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(),"");
		m_Shaders[name] = Ref<Shader>(Shader::Create(filepath));
	}

	const Ref<Shader>& ShaderLibrary::Get(const std::string& name) const
	{
		RE_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(),"");
		return m_Shaders.at(name);
	}
}