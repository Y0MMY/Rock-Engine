#include "pch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

namespace RockEngine
{
	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		ReadShaderFromFile(filepath);
	}

	void OpenGLShader::ReadShaderFromFile(const std::string& filepath)
	{
		std::ifstream in(filepath, std::ios::out);

		if (in)
		{
			in.seekg(0, std::ios::end);
			m_ShaderSource.resize(in.tellg());

			in.seekg(std::ios::beg);
			in.read((char*)m_ShaderSource.c_str(), m_ShaderSource.size());
		}
		else
		{
			RE_CORE_ASSERT(false, "Could not open file!");
		}
		in.close();

	}
	void OpenGLShader::Bind() const {
		Renderer::Submit([=]()
			{
				glUseProgram(m_RendererID);
			});
	}
}