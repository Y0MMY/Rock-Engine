#pragma once

namespace RockEngine
{
	using RendererID = unsigned int;

	enum class RendererAPIType : uint32_t
	{
		None = 0,
		OpenGL = 1,
		Vulkan = 2
	};

	class RendererAPI
	{
	public:
		// Commands
		static void Clear(float r, float g, float b, float a);
		static void SetClearColor(float r, float g, float b, float a);
		static void SetViewport(u32 Width, u32 Height, u32 x, u32 y);

		static void DrawIndexed(u32 count);

		static void Init();
	public:
		static RendererAPIType Current() {
			return s_RendererAPI;
		}
	private:
		static RendererAPIType s_RendererAPI;
	};
}