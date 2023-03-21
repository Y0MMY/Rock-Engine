#include "pch.h"
#include "FrameBuffer.h"

#include "RockEngine/Platform/OpenGL/OpenGLFramebuffer.h"

namespace RockEngine
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpec& spec)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::OpenGL: return Ref<OpenGLFramebuffer>::Create(spec);
		}
		RE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}