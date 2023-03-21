#pragma once

#include "RockEngine/Renderer/Renderer.h"

namespace RockEngine
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8 = 1,
		RGBA16F = 2,
		RGBA32F = 3,
		RG32F = 4,

		// Depth/stencil
		DEPTH32F = 5,
		DEPTH24STENCIL8 = 6,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferSpec
	{
		u32 Width;
		u32 Height;
		u32 Samples = 1;

		FramebufferTextureFormat format;

		bool SpawChainTarget = false;
	};

	class Framebuffer : public RefCounted
	{
	public:
		virtual ~Framebuffer() {}

		virtual const FramebufferSpec& GetSpecification() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void BindTexture(u32 slot = 0) const = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual RendererID GetRendererID() const = 0;
		virtual RendererID GetColorAttachmentRendererID() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpec& spec);
	};
}