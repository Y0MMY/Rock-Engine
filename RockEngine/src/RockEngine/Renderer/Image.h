#pragma once

#include <glm/glm.hpp>

namespace RockEngine
{
	//enum class ImageFormat
	//{
	//	None = 0,
	//	RGB,
	//	RGBA,
	//	RGBA16F,
	//	RGBA32F,
	//	RG32F,

	//	SRGB,

	//	DEPTH32F,
	//	DEPTH24STENCIL8,

	//	// Defaults
	//	Depth = DEPTH24STENCIL8
	//};

	//class Image : public RefCounted
	//{
	//public:
	//	virtual ~Image() {}

	//	virtual void Invalidate() = 0;
	//	virtual void Release() = 0;

	//	virtual uint32_t GetWidth() const = 0;
	//	virtual uint32_t GetHeight() const = 0;

	//	virtual ImageFormat GetFormat() const = 0;

	//	virtual Buffer GetBuffer() const = 0;
	//	virtual Buffer& GetBuffer() = 0;

	//	virtual uint64_t GetHash() const = 0;

	//	// TODO: usage (eg. shader read)
	//};

	//class Image2D : public Image
	//{
	//public:
	//	static Ref<Image2D> Create(ImageFormat format, uint32_t width, uint32_t height, Buffer buffer);
	//	static Ref<Image2D> Create(ImageFormat format, uint32_t width, uint32_t height, const void* data = nullptr);
	//};

	//namespace Utils {

	//	inline uint32_t GetImageFormatBPP(ImageFormat format)
	//	{
	//		switch (format)
	//		{
	//		case ImageFormat::RGB:
	//		case ImageFormat::SRGB:    return 3;
	//		case ImageFormat::RGBA:    return 4;
	//		case ImageFormat::RGBA16F: return 2 * 4;
	//		case ImageFormat::RGBA32F: return 4 * 4;
	//		}
	//		RE_CORE_ASSERT(false);
	//		return 0;
	//	}

	//	inline uint32_t CalculateMipCount(uint32_t width, uint32_t height)
	//	{
	//		return std::floor(std::log2(glm::min(width, height))) + 1;
	//	}

	//	inline uint32_t GetImageMemorySize(ImageFormat format, uint32_t width, uint32_t height)
	//	{
	//		return width * height * GetImageFormatBPP(format);
	//	}

	//}


}