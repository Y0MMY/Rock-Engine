#pragma once

#include "RockEngine/Renderer/Texture.h"
#include "RockEngine/Core/Buffer.h"

namespace RockEngine
{
	class OpenGLTexture2D : public Texture2D {

	public:
		OpenGLTexture2D(TextureFormat format, u32 width, u32 height, TextureWrap wrap);
		OpenGLTexture2D(const std::string& path, bool srgb = false);

		virtual ~OpenGLTexture2D() override;

		virtual void Bind(u32 slot) const override;

		virtual TextureFormat GetFormat() const override { return m_Format; }
		virtual u32 GetWidth() const override { return m_Width; }
		virtual u32 GetHeight() const override { return m_Height; }
		virtual u32 GetMipLevelCount() const override;

		const Buffer& GetBuffer() const override { return m_ImageData; }

		virtual RendererID GetRendererID() const override { return m_RendererID;  }

		virtual bool Loaded() const override { return m_Loaded; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

		virtual const std::string& GetPath() const override { return m_FilePath; }
	private:
		Buffer m_ImageData;
		RendererID m_RendererID = 0;
		TextureFormat m_Format;
		TextureWrap m_Wrap = TextureWrap::Clamp;
		uint32_t m_Width, m_Height;

		bool m_IsHDR = false;
		bool m_Loaded = false;
		std::string m_FilePath;
	};

	class OpenGLTextureCube : public TextureCube
	{
	public:
		OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height);
		OpenGLTextureCube(const std::string& path);
		virtual ~OpenGLTextureCube() override;

		virtual void Bind(u32 slot) const override;

		virtual TextureFormat GetFormat() const override { return m_Format; }
		virtual u32 GetWidth() const override { return m_Width; }
		virtual u32 GetHeight() const override { return m_Height; }
		virtual u32 GetMipLevelCount() const override;

		const Buffer& GetBuffer() const override { return m_ImageData; }

		virtual RendererID GetRendererID() const override { return m_RendererID; }

		virtual bool Loaded() const override { return false; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
		}

		virtual const std::string& GetPath() const override { return m_FilePath; }
	private:
		RendererID m_RendererID;
		TextureFormat m_Format;
		uint32_t m_Width, m_Height;

		Buffer m_ImageData;

		std::string m_FilePath;
	};
}