#pragma once

#include "RockEngine/Core/Buffer.h"
#include "RockEngine/Renderer/Renderer.h"

namespace RockEngine
{
	enum class TextureFormat
	{
		None = 0,
		RGB = 1,
		RGBA = 2,
		Float16 = 3
	};

	enum class TextureWrap
	{
		None = 0,
		Clamp = 1,
		Repeat = 2
	};

	class Texture : public RefCounted
	{
	public:
		virtual ~Texture() { }

		virtual void Bind(u32 slot = 0) const = 0;

		virtual TextureFormat GetFormat() const = 0;
		virtual u32 GetWidth() const = 0;
		virtual u32 GetHeight() const = 0;
		virtual u32 GetMipLevelCount() const = 0;

		virtual RendererID GetRendererID() const = 0;

		virtual const Buffer& GetBuffer() const = 0;

		static u32 GetBPP(TextureFormat format);
		static u32 CalculateMipMapCount(u32 width, u32 height);

		virtual bool operator==(const Texture& other) const = 0;

		virtual const std::string& GetPath() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(TextureFormat format, u32 width, u32 height, TextureWrap = TextureWrap::Clamp);
		static Ref<Texture2D> Create(const std::string& path, bool srgb = false);
	};

	class TextureCube
	{

	};
}