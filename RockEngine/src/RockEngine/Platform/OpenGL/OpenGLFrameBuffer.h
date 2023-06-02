#pragma once

#include "RockEngine/Renderer/FrameBuffer.h"

namespace RockEngine
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		~OpenGLFramebuffer() override;

		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;

		void Bind() const override;
		void Unbind() const override;

		void BindTexture(size_t attachmentIndex, size_t slot) const override;

		u32 GetWidth() const override { return m_Width; }
		u32 GetHeight() const override { return m_Height; }

		RendererID GetRendererID() const override { return m_RendererID; }
		RendererID GetColorAttachmentRendererID(size_t index) const override { return m_ColorAttachments.first[index]; }
		RendererID GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }

		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		RendererID m_RendererID;
		std::pair<std::vector<RendererID>, std::vector<FramebufferTextureFormat>>m_ColorAttachments;
		RendererID m_DepthAttachment;
		FramebufferTextureFormat m_DepthAttachmentFormat = FramebufferTextureFormat::None;
		FramebufferSpecification m_Specification;

		u32 m_Width, m_Height;
	};
}