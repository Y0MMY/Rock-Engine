#pragma once

namespace RockEngine
{
	using RendererID = unsigned int;

	// TODO: move into separate header
	enum class PrimitiveType
	{
		None = 0, Triangles, Lines
	};

	enum class RendererAPIType : uint32_t
	{
		None = 0,
		OpenGL = 1,
		Vulkan = 2
	};

	struct RenderAPICapabilities
	{
		std::string Vendor;
		std::string Renderer;
		std::string Version;

		int MaxSamples = 0;
		float MaxAnisotropy = 0.0f;
		int MaxTextureUnits = 0;
	};

	class RendererAPI
	{
	public:
		// Commands
		static void Clear(float r, float g, float b, float a);
		static void SetClearColor(float r, float g, float b, float a);
		static void SetViewport(u32 Width, u32 Height, u32 x, u32 y);

		static void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest = true);
		static void SetLineThickness(float thickness);

		static void Init();

		static RenderAPICapabilities& GetCapabilities()
		{
			static RenderAPICapabilities capabilities;
			return capabilities;
		}
	public:
		static RendererAPIType Current() {
			return s_RendererAPI;
		}
	private:
		static RendererAPIType s_RendererAPI;
	};
}