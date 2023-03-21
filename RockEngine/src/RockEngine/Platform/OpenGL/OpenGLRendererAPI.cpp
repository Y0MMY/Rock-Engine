#include "pch.h"
#include "RockEngine/Renderer/RendererAPI.h"

#include "Glad/glad.h"

namespace RockEngine
{
	void RendererAPI::Clear(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RendererAPI::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}
	
	void RendererAPI::DrawIndexed(u32 count)
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void RendererAPI::SetViewport(u32 Width, u32 Height, u32 x, u32 y)
	{
		glViewport(x, y, Width, Height);
	}

	void RendererAPI::Init()
	{
		/*unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);*/
	}
}