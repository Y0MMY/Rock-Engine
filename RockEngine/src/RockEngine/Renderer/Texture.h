#pragma once

#include "RockEngine/Core/Buffer.h"
#include "RockEngine/Renderer/RendererAPI.h"

#include <glm/glm.hpp>

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

	namespace Utils
	{
		inline uint32_t CalculateMipCount(uint32_t width, uint32_t height)
		{
			return std::floor(std::log2(glm::min(width, height))) + 1;
		}

		inline uint32_t GetImageFormatBPP(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::RGB:
			case TextureFormat::RGBA:    return 4;
			case TextureFormat::Float16: return 2 * 4;
			}
			RE_CORE_ASSERT(false,"");
			return 0;
		}
	}

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

		virtual bool Loaded() const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		virtual const std::string& GetPath() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(TextureFormat format, u32 width, u32 height, TextureWrap = TextureWrap::Clamp);
		static Ref<Texture2D> Create(const std::string& path, bool srgb = false);
	};

	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create(TextureFormat format, u32 width, u32 height);
		static Ref<TextureCube> Create(const std::string& path);
	};
}