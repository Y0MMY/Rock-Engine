#include "pch.h"
#include "Texture.h"

#include "RockEngine/Platform/OpenGL/OpenGLTexture.h"

namespace RockEngine
{
	uint32_t Texture::GetBPP(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGB:    return 3;
		case TextureFormat::RGBA:   return 4;
		}
		return 0;
	}

	uint32_t Texture::CalculateMipMapCount(uint32_t width, uint32_t height)
	{
		uint32_t levels = 1;
		while ((width | height) >> levels)
			levels++;

		return levels;
	}

	// ------------------------------------------


	Ref<Texture2D> Texture2D::Create(TextureFormat format, u32 width, u32 height, TextureWrap wrap)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::OpenGL: return Ref<OpenGLTexture2D>::Create(format, width, height, wrap);
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path, bool srgb)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::OpenGL: return Ref<OpenGLTexture2D>::Create(path, srgb);
		}
		return nullptr;
	}
}