#include "pch.h"
#include "IndexBuffer.h"

#include "RockEngine/Platform/OpenGL/OpenGLIndexBuffer.h"

namespace RockEngine
{
	IndexBuffer* IndexBuffer::Create(void* data, u32 size)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:
				return nullptr;
			
			case RendererAPIType::OpenGL:
				return new OpenGLIndexBuffer(data, size);
		}

		RE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	
	IndexBuffer* IndexBuffer::Create(u32 size)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:  return nullptr;
			case RendererAPIType::OpenGL: return new OpenGLIndexBuffer(size);
		}

		RE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
