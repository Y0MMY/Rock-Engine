#include "pch.h"
#include "RendererConext.h"
#include "RendererAPI.h"

struct GLFWwindow;

namespace RockEngine
{
	Ref<RendererContext> RendererContext::Create(GLFWwindow* windowHandle)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:    return nullptr;
			//case RendererAPIType::OpenGL:  return Ref<OpenGLContext>::Create(windowHandle);
		}
		RE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
