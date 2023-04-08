#include "pch.h"
#include "OpenGLPipeline.h"

#include "RockEngine/Renderer/Renderer.h"

#include <glad/glad.h>

namespace RockEngine
{
	static GLenum OpenGLShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:    return GL_FLOAT;
			case ShaderDataType::Float2:   return GL_FLOAT;
			case ShaderDataType::Float3:   return GL_FLOAT;
			case ShaderDataType::Float4:   return GL_FLOAT;
			case ShaderDataType::Mat3:     return GL_FLOAT;
			case ShaderDataType::Mat4:     return GL_FLOAT;
			case ShaderDataType::Int:      return GL_INT;
			case ShaderDataType::Int2:     return GL_INT;
			case ShaderDataType::Int3:     return GL_INT;
			case ShaderDataType::Int4:     return GL_INT;
			case ShaderDataType::Bool:     return GL_BOOL;
		}

		RE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
		: m_Specification(spec)
	{
		Invalidate();
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]()
			{
				glDeleteVertexArrays(1, &rendererID);
			});
	}

	void OpenGLPipeline::Invalidate()
	{
		RE_CORE_ASSERT(m_Specification.Layout.GetElements().size(), "Layout is empty!");
		Ref<OpenGLPipeline> instance = this;
		Renderer::Submit([instance]() mutable
		{
			if(instance->m_RendererID)
				glDeleteVertexArrays(1, &instance->m_RendererID);

			glGenVertexArrays(1, &instance->m_RendererID);
			//glBindVertexArray(instance->m_RendererID);

		});

	}

	void OpenGLPipeline::Bind() const
	{
		Ref<const OpenGLPipeline> instance = this;
		Renderer::Submit([instance]() mutable
		{
			if(instance->m_RendererID)
				glDeleteVertexArrays(1, &instance->m_RendererID);

			glBindVertexArray(instance->m_RendererID);
			const auto& layout = instance->m_Specification.Layout;
			uint32_t attribIndex = 0;

			for (const auto& element : layout)
			{
				auto base = OpenGLShaderDataType(element.Type);
				glEnableVertexAttribArray(attribIndex);

				if (base = GL_INT)
				{
					glVertexAttribIPointer(attribIndex, element.GetComponentCount(), GL_INT, layout.GetStride(), (const void*)(intptr_t)element.Offset);
				}
				else
					glVertexAttribPointer(attribIndex, element.GetComponentCount(), base, element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(), (const void*)(intptr_t)element.Offset);

				attribIndex++;
			}

		});

	}

}