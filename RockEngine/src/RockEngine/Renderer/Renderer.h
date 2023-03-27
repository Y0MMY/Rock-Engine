#pragma once

#include "RenderCommandQueue.h"

#include "Mesh.h"
#include "RenderPass.h"

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

		static Ref<ShaderLibrary> GetShaderLibrary();

		static void SubmitQuad(Ref<MaterialInstance> material, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitFullscreenQuad(Ref<MaterialInstance> material);


		static void BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear = true);
		static void EndRenderPass();

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