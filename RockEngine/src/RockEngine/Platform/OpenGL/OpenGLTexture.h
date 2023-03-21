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
		std::string m_FilePath;
	};
}