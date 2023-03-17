#pragma once

#include "RenderCommandQueue.h"

namespace RockEngine
{

	class Renderer
	{
	public:
		// Commands
		static void Clear();
		static void Clear(float r, float g, float b, float a = 1.0f);
		static void SetClearColor(float r, float g, float b, float a);

		static void Init();

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