#include "pch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>

#include "stb_image.h"

namespace RockEngine
{
	static GLenum OpenGLTextureFormat(TextureFormat format)
	{
		switch (format)
		{
			case RockEngine::TextureFormat::RGB:     return GL_RGB;
			case RockEngine::TextureFormat::RGBA:    return GL_RGBA;
			case RockEngine::TextureFormat::Float16: return GL_RGBA16F;
		}
		RE_CORE_ASSERT(false, "Unknown texture format!");
		return 0;
	}

	OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, u32 width, u32 height, TextureWrap wrap)
		: m_Format(format), m_Width(width), m_Height(height), m_Wrap(wrap)
	{
		Renderer::Submit([this]()
			{
				glGenTextures(1, &m_RendererID);
				glBindTexture(GL_TEXTURE_2D, m_RendererID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				GLenum wrap = m_Wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

				glTextureParameterf(m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().MaxAnisotropy);

				glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTextureFormat(m_Format), m_Width, m_Height, 0, OpenGLTextureFormat(m_Format),
					GL_UNSIGNED_BYTE, nullptr);

				glBindTexture(GL_TEXTURE_2D, 0);
			});
		m_ImageData.Allocate(width * height * Texture::GetBPP(format));
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool srgb)
		: m_FilePath(path)
	{
		int width, height, channels;
		if (stbi_is_hdr(path.c_str()))
		{
			RE_CORE_INFO("Loading HDR texture {0}, srgb={1}", path, srgb);
			m_ImageData.Data = (byte*)stbi_loadf(path.c_str(), &width, &height, &channels, 0);
			m_IsHDR = true;
			m_Format = TextureFormat::Float16;
		}
		else
		{
			RE_CORE_INFO("Loading texture {0}, srgb={1}", path, srgb);
			m_ImageData.Data = stbi_load(path.c_str(), &width, &height, &channels, srgb ? STBI_rgb : STBI_rgb_alpha);
			RE_CORE_ASSERT(m_ImageData.Data, "Could not read image!");
			m_Format = TextureFormat::RGBA;
		}

		if (!m_ImageData.Data)
			return;

		m_Width = width;
		m_Height = height;

		Ref<OpenGLTexture2D> instance = this;
		Renderer::Submit([instance, srgb]() mutable
			{
				// TODO: Consolidate properly
				if (srgb)
				{
					glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_RendererID);
					int levels = Texture::CalculateMipMapCount(instance->m_Width, instance->m_Height);
					glTextureStorage2D(instance->m_RendererID, levels, GL_SRGB8, instance->m_Width, instance->m_Height);
					glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
					glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTextureSubImage2D(instance->m_RendererID, 0, 0, 0, instance->m_Width, instance->m_Height, GL_RGB, GL_UNSIGNED_BYTE, instance->m_ImageData.Data);
					glGenerateTextureMipmap(instance->m_RendererID);
				}
				else
				{
					glGenTextures(1, &instance->m_RendererID);
					glBindTexture(GL_TEXTURE_2D, instance->m_RendererID);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

					GLenum internalFormat = OpenGLTextureFormat(instance->m_Format);
					GLenum format = srgb ? GL_SRGB8 : (instance->m_IsHDR ? GL_RGB : OpenGLTextureFormat(instance->m_Format)); // HDR = GL_RGB for now
					GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, instance->m_Width, instance->m_Height, 0, format, type, instance->m_ImageData.Data);
					glGenerateMipmap(GL_TEXTURE_2D);

					glBindTexture(GL_TEXTURE_2D, 0);
				}
		stbi_image_free(instance->m_ImageData.Data);



			});
	}

	void OpenGLTexture2D::Bind(u32 slot) const
	{
		Renderer::Submit([this, slot]() {
			glBindTextureUnit(slot, m_RendererID);
			});
	}

	u32 OpenGLTexture2D::GetMipLevelCount() const
	{
		return Texture::CalculateMipMapCount(m_Width, m_Height);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {
			glDeleteTextures(1, &rendererID);
			});
	}
}