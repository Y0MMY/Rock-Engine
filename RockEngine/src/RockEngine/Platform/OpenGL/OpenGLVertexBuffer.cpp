#include "pch.h"
#include "OpenGLVertexBuffer.h"

#include <glad/glad.h>

#include "RockEngine/Renderer/Renderer.h"

namespace RockEngine
{
	static GLenum OpenGLUsage(VertexBufferUsage usage)
	{
		switch (usage)
		{
		case VertexBufferUsage::Static:    return GL_STATIC_DRAW;
		case VertexBufferUsage::Dynamic:   return GL_DYNAMIC_DRAW;
		}
		RE_CORE_ASSERT(false, "Unknown vertex buffer usage");
		return 0;
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, u32 size, VertexBufferUsage usage)
		: m_Size(size)
	{
		m_LocalData = Buffer::Copy(data, size);
		Renderer::Submit([this]() mutable
			{
				glGenBuffers(1, &m_RendererID);

				glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
				glBufferData(GL_ARRAY_BUFFER, m_LocalData.Size, m_LocalData.Data, GL_STATIC_DRAW);
			}
		);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(u32 size, VertexBufferUsage usage)
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
			}
		);
	}
}