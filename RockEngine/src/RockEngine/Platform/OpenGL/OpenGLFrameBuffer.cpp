#include "pch.h"
#include "OpenGLFrameBuffer.h"

#include "RockEngine/Renderer/Renderer.h"

#include <glad/glad.h>

namespace RockEngine
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpec& spec)
		: m_Spec(spec), m_Width(spec.Width), m_Height(spec.Height)

	{
		Resize(m_Width, m_Height, true);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		Ref<OpenGLFramebuffer> instance = this;
		Renderer::Submit([instance]()
			{
				glDeleteFramebuffers(1, &instance->m_RendererID);
			});
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
	{
		if (!forceRecreate && (m_Spec.Width == width && m_Spec.Height == height))
			return;
		m_Width = width;
		m_Height = height;

		Ref<OpenGLFramebuffer> instance = this;
		Renderer::Submit([instance]() mutable
			{
				if (instance->m_RendererID)
				{
					glDeleteFramebuffers(1, &instance->m_RendererID);
					glDeleteTextures(1, &instance->m_ColorAttachment);
					glDeleteTextures(1, &instance->m_DepthAttachment);
				}

				glGenFramebuffers(1, &instance->m_RendererID);
				glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);

				glGenTextures(1, &instance->m_ColorAttachment);
				glBindTexture(GL_TEXTURE_2D, instance->m_ColorAttachment);

				if (instance->m_Spec.Format == FramebufferTextureFormat::RGBA16F)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, instance->m_Width, instance->m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
				}
				else if (instance->m_Spec.Format == FramebufferTextureFormat::RGBA8)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, instance->m_Width, instance->m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				}
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, instance->m_ColorAttachment, 0);

				glGenTextures(1, &instance->m_DepthAttachment);
				glBindTexture(GL_TEXTURE_2D, instance->m_DepthAttachment);
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, instance->m_Width, instance->m_Height, 0,
					GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
				);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, instance->m_DepthAttachment, 0);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					RE_CORE_ERROR("Framebuffer is incomplete!");

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			});
	}

	void OpenGLFramebuffer::Bind() const
	{
		Ref<const OpenGLFramebuffer> instance = this;
		Renderer::Submit([instance]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);
			glViewport(0, 0, instance->m_Width, instance->m_Height);

			glBindTexture(0, instance->m_RendererID);
		});
	}
	
	void OpenGLFramebuffer::Unbind() const
	{
		Renderer::Submit([]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		});
	}

	void OpenGLFramebuffer::BindTexture(u32 slot) const
	{
		Ref<const OpenGLFramebuffer> instance = this;
		Renderer::Submit([instance, slot] ()
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, instance->m_ColorAttachment);
		});
	}
}