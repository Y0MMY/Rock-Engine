#include "pch.h"
#include "VertexBuffer.h"

#include "RockEngine/Platform/OpenGL/OpenGLVertexBuffer.h"

namespace RockEngine
{
	VertexBuffer* VertexBuffer::Create(void* data, u32 size)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:
			return nullptr;

		case RendererAPIType::OpenGL:
			return new OpenGLVertexBuffer(data, size);
		}

		RE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	VertexBuffer* VertexBuffer::Create(u32 size)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:
			return nullptr;

		case RendererAPIType::OpenGL:
			return new OpenGLVertexBuffer(size);
		}

		RE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}