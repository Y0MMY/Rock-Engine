#include "pch.h"
#include "OpenGLIndexBuffer.h"

#include <glad/glad.h>

namespace RockEngine
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, u32 size)
		: m_Size(size)
	{
		m_LocalData = Buffer::Copy(data, size);
		Renderer::Submit([this]() mutable
			{
				glGenBuffers(1, &m_RendererID);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_LocalData.Size, m_LocalData.Data, GL_STATIC_DRAW);
			}
		);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(u32 size)
		: m_Size(size)
	{
		Renderer::Submit([this]() mutable
			{
				glGenBuffers(1, &m_RendererID);
			}
		);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		Renderer::Submit([this]() mutable
			{
				glDeleteBuffers(1, &m_RendererID);
			}
		);
	}

	void OpenGLIndexBuffer::Bind() const {

		Renderer::Submit([this]() mutable
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
			}
		);

	}

	void OpenGLIndexBuffer::SetData(void* data, u32 size, u32 offset)
	{
		m_LocalData = Buffer::Copy(data, size);
		m_Size = size;

		Renderer::Submit([this]() mutable
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_LocalData.Size, m_LocalData.Data, GL_STATIC_DRAW);
			}
		);
	}
}