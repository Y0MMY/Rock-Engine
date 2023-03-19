#include "pch.h"
#include "OpenGLVertexBuffer.h"

#include <glad/glad.h>

namespace RockEngine
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, u32 size)
		: m_Size(size)
	{
		m_LocalData = Buffer::Copy(data, size);
		Renderer::Submit([this]() mutable
			{
				glGenBuffers(1, &m_RendererID);

				glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
				glBufferData(GL_ARRAY_BUFFER, m_LocalData.Size, m_LocalData.Data, GL_STATIC_DRAW);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			}
		);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(u32 size)
		: m_Size(size)
	{
		Renderer::Submit([this]() mutable
			{
				glGenBuffers(1, &m_RendererID);
			}
		);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		Renderer::Submit([this]() mutable
			{
				glDeleteBuffers(1, &m_RendererID);
			}
		);
	}

	void OpenGLVertexBuffer::Bind() const {

		Renderer::Submit([this]() mutable
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
			}
		);

	}

	void OpenGLVertexBuffer::SetData(void* data, u32 size, u32 offset)
	{
		m_LocalData = Buffer::Copy(data, size);
		m_Size = size;

		Renderer::Submit([this]() mutable
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
				glBufferData(GL_ARRAY_BUFFER, m_LocalData.Size, m_LocalData.Data, GL_STATIC_DRAW);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			}
		);
	}
}