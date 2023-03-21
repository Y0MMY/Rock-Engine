#include "pch.h"
#include "IndexBuffer.h"

#include "RockEngine/Platform/OpenGL/OpenGLIndexBuffer.h"

namespace RockEngine
{
	Ref<IndexBuffer> IndexBuffer::Create(void* data, u32 size)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:
				return nullptr;
			
			case RendererAPIType::OpenGL:
				return Ref<OpenGLIndexBuffer>::Create(data, size);
		}

		RE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	
	Ref<IndexBuffer> IndexBuffer::Create(u32 size)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:  return nullptr;
			case RendererAPIType::OpenGL: return Ref<OpenGLIndexBuffer>::Create(size);
		}

		RE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
