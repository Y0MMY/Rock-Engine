#include "pch.h"
#include "VertexBuffer.h"

#include "RockEngine/Platform/OpenGL/OpenGLVertexBuffer.h"

namespace RockEngine
{
	Ref<VertexBuffer> VertexBuffer::Create(void* data, u32 size)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:
			return nullptr;

		case RendererAPIType::OpenGL:
			return Ref<OpenGLVertexBuffer>::Create(data, size);
		}

		RE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(u32 size)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:
			return nullptr;

		case RendererAPIType::OpenGL:
			return  Ref<OpenGLVertexBuffer>::Create(size);
		}

		RE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}