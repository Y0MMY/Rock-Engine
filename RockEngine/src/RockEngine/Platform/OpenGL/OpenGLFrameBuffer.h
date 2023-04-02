#pragma once

#include "RockEngine/Renderer/FrameBuffer.h"

namespace RockEngine
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpec& spec);
		~OpenGLFramebuffer() override;

		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;

		void Bind() const override;
		void Unbind() const override;

		void BindTexture(u32 slot) const override;

		u32 GetWidth() const override { return m_Width; }
		u32 GetHeight() const override { return m_Height; }

		RendererID GetRendererID() const override { return m_RendererID; }
		RendererID GetColorAttachmentRendererID() const override { return m_ColorAttachment; }
		RendererID GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }

		const FramebufferSpec& GetSpecification() const override { return m_Spec; }
	private:
		RendererID m_RendererID;
		RendererID m_ColorAttachment = 0, m_DepthAttachment = 0;
		FramebufferSpec m_Spec;

		u32 m_Width, m_Height;
	};
}