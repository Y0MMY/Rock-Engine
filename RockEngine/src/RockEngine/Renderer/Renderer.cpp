#include "pch.h"
#include "Renderer.h"

#include "RendererAPI.h"

namespace RockEngine
{
	RendererAPIType RendererAPI::s_RendererAPI = RendererAPIType::OpenGL;

	struct RendererData
	{
		RenderCommandQueue m_CommandQueue;
	};

	RendererData s_Data;

	void Renderer::Init()
	{

	}

	void Renderer::Clear(float r, float g, float b, float a /* = 1.0f */)
	{
		Renderer::Submit([=]() {
			RendererAPI::Clear(r,g,b,a);
			});
	}

	void Renderer::WaitAndRender()
	{
		s_Data.m_CommandQueue.Execute();
	}

	RenderCommandQueue& Renderer::GetRenderCommandQueue()
	{
		return s_Data.m_CommandQueue;
	}
}