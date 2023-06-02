#include "pch.h"
#include "Texture.h"

#include "RockEngine/Platform/OpenGL/OpenGLTexture.h"

namespace RockEngine
{
	Ref<Texture2D> Texture2D::Create(TextureFormat format, u32 width, u32 height, TextureWrap wrap)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::OpenGL: return Ref<OpenGLTexture2D>::Create(format, width, height, wrap);
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path, bool srgb)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::OpenGL: return Ref<OpenGLTexture2D>::Create(path, srgb);
		}
		return nullptr;
	}

	// ---------------------------------------------------

	Ref<TextureCube> TextureCube::Create(TextureFormat format, u32 width, u32 height)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::OpenGL: return Ref<OpenGLTextureCube>::Create(format, width, height);
		}
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(const std::filesystem::path& path)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::OpenGL: return Ref<OpenGLTextureCube>::Create(path);
		}
		return nullptr;
	}
}