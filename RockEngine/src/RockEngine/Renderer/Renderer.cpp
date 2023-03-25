#include "pch.h"
#include "Renderer.h"

namespace RockEngine
{
	RendererAPIType RendererAPI::s_RendererAPI = RendererAPIType::OpenGL;

	struct RendererData
	{
		RenderCommandQueue m_CommandQueue;
		Ref<ShaderLibrary> m_ShaderLibrary;
	};

	RendererData s_Data;

	void Renderer::Init()
	{
		s_Data.m_ShaderLibrary = Ref<ShaderLibrary>::Create();
		Renderer::Submit([=]() {
			RendererAPI::Init();
			});

		s_Data.m_ShaderLibrary->Load("assets/shaders/shader.glsl");
	}


	void Renderer::Clear(float r, float g, float b, float a /* = 1.0f */)
	{
		Renderer::Submit([=]() {
			RendererAPI::Clear(r,g,b,a);
			});
	}

	void Renderer::SetViewport(u32 Width, u32 Height, u32 x, u32 y)
	{
		Renderer::Submit([=]() {
			RendererAPI::SetViewport(Width,Height,x,y);
			});
	}

	void Renderer::DrawIndexed(u32 count, bool depthTest)
	{
		Renderer::Submit([=]() {
			RendererAPI::DrawIndexed(count, depthTest);
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