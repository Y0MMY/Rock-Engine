#pragma once

#include "RendererAPI.h"

#include "RenderCommandQueue.h"
#include "RockEngine/Core/Core.h"

#include "Shader.h"

namespace RockEngine
{

	class Renderer
	{
	public:
		// Commands
		static void Clear();
		static void Clear(float r, float g, float b, float a = 1.0f);
		static void SetClearColor(float r, float g, float b, float a);
		static void SetViewport(u32 Width, u32 Height, u32 x = 0, u32 y = 0);

		static void DrawIndexed(u32 count, bool depthTest = true);

		static void Init();

		//static Ref<ShaderLibrary> GetShaderLibrary();

		static void WaitAndRender();

		template <typename FuncT>
		static void Submit(FuncT&& func)
		{
			auto renderCMD = [](void* ptr)
			{
				auto pFunc = (FuncT*)ptr;

				(*pFunc)();

				pFunc->~FuncT();
			};

			auto storageBuffer = GetRenderCommandQueue().Allocate(renderCMD, sizeof(func));
			new (storageBuffer) FuncT(std::forward<FuncT>(func));
		}
	private:
		static RenderCommandQueue& GetRenderCommandQueue();
	};
}