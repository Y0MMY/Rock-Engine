#pragma once

#include "RockEngine/Renderer/FrameBuffer.h"

namespace RockEngine
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpec& spec);
		~OpenGLFramebuffer() override;

		void Bind() const override;
		void Unbind() const override;

		void BindTexture(u32 slot) const override;
		virtual void Resize(uint32_t width, uint32_t height) override;

		const FramebufferSpec& GetSpecification() const override { return m_Spec; }
		RendererID GetRendererID() const override { return m_RendererID; }
		RendererID GetColorAttachmentRendererID() const override { return m_ColorAttachment; }
	private:
		RendererID m_RendererID;
		RendererID m_ColorAttachment, m_DepthAttachment;
		FramebufferSpec m_Spec;

		u32 m_Width, m_Height;
	};
}