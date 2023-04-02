#include "pch.h"
#include "OpenGLIndexBuffer.h"

#include "RockEngine/Renderer/Renderer.h"

#include <glad/glad.h>

namespace RockEngine
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, u32 size)
		: m_Size(size)
	{
		m_LocalData = Buffer::Copy(data, size);

		Ref<OpenGLIndexBuffer> instance = this;
		Renderer::Submit([instance]() mutable
			{
				glGenBuffers(1, &instance->m_RendererID);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RendererID);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, instance->m_LocalData.Size, instance->m_LocalData.Data, GL_STATIC_DRAW);
			}
		);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(u32 size)
		: m_Size(size)
	{

		Ref<OpenGLIndexBuffer> instance = this;
		Renderer::Submit([instance]() mutable
			{
				glGenBuffers(1, &instance->m_RendererID);
			}
		);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{

		Ref<OpenGLIndexBuffer> instance = this;
		Renderer::Submit([instance]() mutable
			{
				glDeleteBuffers(1, &instance->m_RendererID);
			}
		);
	}

	void OpenGLIndexBuffer::Bind() const {

		Ref<const OpenGLIndexBuffer> instance = this;
		Renderer::Submit([instance]() mutable
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RendererID);
			}
		);

	}

	void OpenGLIndexBuffer::SetData(void* data, u32 size, u32 offset)
	{
		m_LocalData = Buffer::Copy(data, size);
		m_Size = size;

		Ref<OpenGLIndexBuffer> instance = this;
		Renderer::Submit([instance]() mutable
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RendererID);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, instance->m_LocalData.Size, instance->m_LocalData.Data, GL_STATIC_DRAW);
			}
		);
	}
}