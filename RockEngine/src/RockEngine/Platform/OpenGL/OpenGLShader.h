#pragma once

#include "RockEngine/Renderer/Shader.h"

namespace RockEngine
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);

		virtual void Bind() const override;
	private:
		void ReadShaderFromFile(const std::string& filepath);
	private:
		RendererID m_RendererID = 0;

		std::string m_ShaderSource;
	};
}