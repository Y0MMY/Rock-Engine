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

	//////////////////////////////////////////////////////////////////////////////////
	// Texture2D
	//////////////////////////////////////////////////////////////////////////////////

	OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, u32 width, u32 height, TextureWrap wrap)
		:m_Format(format), m_Width(width), m_Height(height),m_Wrap(wrap)
	{
		Ref<OpenGLTexture2D> instance = this;
		Renderer::Submit([instance]() mutable
			{
				glGenTextures(1, &instance->m_RendererID);
				glBindTexture(GL_TEXTURE_2D, instance->m_RendererID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				GLenum wrap = instance->m_Wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
				// Set our texture parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

				glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTextureFormat(instance->m_Format),
					instance->m_Width, instance->m_Height, 0, OpenGLTextureFormat(instance->m_Format), GL_UNSIGNED_BYTE, nullptr);

				glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
			});
		m_ImageData.Allocate(height * width * Texture::GetBPP(format));
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool srgb)
		: m_FilePath(path)
	{
		int width, height, channels;
		if (stbi_is_hdr(path.c_str()))
		{
			RE_CORE_INFO("Loading HDR texture {}, srgb = {}", path, srgb);
			m_ImageData.Data = (byte*)stbi_loadf(path.c_str(), &width, &height, &channels, srgb ? STBI_rgb : STBI_rgb_alpha);
			m_IsHDR = true;
			m_Format = TextureFormat::Float16;
		}
		else
		{
			RE_CORE_INFO("Loading texture {}, srgb = {}", path, srgb);
			m_ImageData.Data = (byte*)stbi_load(path.c_str(), &width, &height, &channels, srgb ? STBI_rgb : STBI_rgb_alpha);
			m_IsHDR = false;
			m_Format = TextureFormat::RGBA;
		}
		RE_CORE_ASSERT(m_ImageData.Data, "Could not read image!");
		m_Loaded = true;

		m_Width = width;
		m_Height = height;

		Ref<OpenGLTexture2D> instance = this;
		Renderer::Submit([instance, srgb]() mutable
			{
				if (!srgb)
				{
					glGenTextures(1, &instance->m_RendererID);
					glBindTexture(GL_TEXTURE_2D, instance->m_RendererID);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

					auto internalFormat = OpenGLTextureFormat(instance->m_Format);
					GLenum format = srgb ? GL_SRGB8 : (instance->m_IsHDR ? GL_RGB : internalFormat); // HDR = GL_RGB for now
					GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, instance->m_Width, instance->m_Height, 0, format, type, instance->m_ImageData.Data);
					glGenerateMipmap(GL_TEXTURE_2D);

					glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
				}
				else
				{
					glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_RendererID);
					int levels = Texture::CalculateMipMapCount(instance->m_Width, instance->m_Height);
					glTextureStorage2D(instance->m_RendererID, levels, GL_SRGB8, instance->m_Width, instance->m_Height);
					glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
					glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTextureSubImage2D(instance->m_RendererID, 0, 0, 0, instance->m_Width, instance->m_Height, GL_RGB, GL_UNSIGNED_BYTE, instance->m_ImageData.Data);
					glGenerateTextureMipmap(instance->m_RendererID);
				}
				stbi_image_free(instance->m_ImageData.Data);

			});
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {
			glDeleteTextures(1, &rendererID);
			});
	}

	void OpenGLTexture2D::Bind(u32 slot) const
	{
		Renderer::Submit([=]()
			{
				glBindTextureUnit(slot, m_RendererID);
			});
	}

	u32 OpenGLTexture2D::GetMipLevelCount() const
	{
		return Texture::CalculateMipMapCount(m_Width, m_Height);
	}

	//////////////////////////////////////////////////////////////////////////////////
	// TextureCube
	//////////////////////////////////////////////////////////////////////////////////

	OpenGLTextureCube::OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height)
		:m_Width(width),m_Height(height), m_Format(format)
	{
		u32 levels = Texture::CalculateMipMapCount(width, height);
		Ref<OpenGLTextureCube> instance = this;
		Renderer::Submit([instance, levels]() mutable
			{
				glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &instance->m_RendererID);
				glTextureStorage2D(instance->m_RendererID, levels, OpenGLTextureFormat(instance->m_Format), instance->m_Width, instance->m_Height);
				glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			});
	}

	OpenGLTextureCube::OpenGLTextureCube(const std::string& path)
		: m_FilePath(path)
	{
		
		int width, height, channels;
		stbi_set_flip_vertically_on_load(false);
		m_ImageData.Data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);

		m_Width = width;
		m_Height = height;
		m_Format = TextureFormat::RGB;

		u32 faceWidth = m_Width / 4;
		u32 faceHeight = m_Height / 3;

		RE_CORE_ASSERT(faceWidth == faceHeight, "Non-square faces!");

		std::array<byte*, 6> faces;
		for (size_t i = 0; i < faces.size(); i++)
			faces[i] = new byte[faceWidth * faceHeight * 3];
		int faceIndex = 0;

		for (size_t i = 0; i < 4; i++)
		{
			for (size_t y = 0; y < faceHeight; y++)
			{
				size_t yOffset = y + faceHeight;
				for (size_t x = 0; x < faceWidth; x++)
				{
					size_t xOffset = x + i * faceWidth;
					faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData.Data[(xOffset + yOffset * m_Width) * 3 + 0];
					faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData.Data[(xOffset + yOffset * m_Width) * 3 + 1];
					faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData.Data[(xOffset + yOffset * m_Width) * 3 + 2];
				}
			}
			faceIndex++;
		}

		for (size_t i = 0; i < 3; i++)
		{
			// Skip the middle one
			if (i == 1)
				continue;

			for (size_t y = 0; y < faceHeight; y++)
			{
				size_t yOffset = y + i * faceHeight;
				for (size_t x = 0; x < faceWidth; x++)
				{
					size_t xOffset = x + faceWidth;
					faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData.Data[(xOffset + yOffset * m_Width) * 3 + 0];
					faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData.Data[(xOffset + yOffset * m_Width) * 3 + 1];
					faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData.Data[(xOffset + yOffset * m_Width) * 3 + 2];
				}
			}
			faceIndex++;
		}
		Ref<OpenGLTextureCube> instance = this;
		Renderer::Submit([instance, faceWidth, faceHeight, faces]() mutable
		{
			glGenTextures(1, &instance->m_RendererID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, instance->m_RendererID);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameterf(instance->m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().MaxAnisotropy);

			auto format = OpenGLTextureFormat(instance->m_Format);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[2]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[0]);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[4]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[5]);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[1]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[3]);

			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

			glBindTexture(GL_TEXTURE_2D, 0);

			for (size_t i = 0; i < faces.size(); i++)
				delete[] faces[i];

			stbi_image_free(instance->m_ImageData.Data);
		});
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {
			glDeleteTextures(1, &rendererID);
			});
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		Renderer::Submit([=]() {
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
			});
	}

	uint32_t OpenGLTextureCube::GetMipLevelCount() const
	{
		return Texture::CalculateMipMapCount(m_Width, m_Height);
	}
}