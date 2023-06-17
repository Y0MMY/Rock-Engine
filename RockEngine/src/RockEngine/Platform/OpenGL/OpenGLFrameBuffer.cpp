#include "pch.h"
#include "OpenGLFrameBuffer.h"

#include "RockEngine/Renderer/Renderer.h"

#include <glad/glad.h>

namespace RockEngine
{
	namespace Utils
	{
		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, RendererID* outID, u32 count)
		{
			glCreateTextures(TextureTarget(multisampled), 1, outID);
		}

		static void BindTexture(bool multisampled, RendererID id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static GLenum DataType(GLenum format)
		{
			switch (format)
			{
				case GL_RGBA:
				case GL_RGBA8: return GL_UNSIGNED_BYTE;
				case GL_RG16F:
				case GL_RG32F:
				case GL_RGBA16F:
				case GL_RGBA32F: return GL_FLOAT;
				case GL_DEPTH24_STENCIL8: return GL_UNSIGNED_INT_24_8;
			}

			RE_CORE_ASSERT(false, "Unknown format!");
			return 0;
		}

		static void AttachColorTexture(RendererID id, int samples, GLenum format, u32 width, u32 height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				// Only RGBA access for now
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, DataType(format), nullptr);

				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(RendererID id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{

			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
				case FramebufferTextureFormat::DEPTH32F:
					return true;
			}
			return false;
		}

	}
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec), m_Width(spec.Width), m_Height(spec.Height)

	{
		for (auto format : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(format.TextureFormat))
				m_ColorAttachments.second.emplace_back(format.TextureFormat);
			else
				m_DepthAttachmentFormat = format.TextureFormat;
		}


		RE_CORE_INFO_TAG("Renderer", "Creating FrameBuffer {} : {}", spec.Width, spec.Height);
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
		if (!forceRecreate && (m_Width == width && m_Height == height))
			return;

		m_Width = width;
		m_Height = height;

		Ref<OpenGLFramebuffer> instance = this;
		Renderer::Submit([instance]() mutable
			{
				auto& [colorAttachment, colorAttachmentsFormat] = instance->m_ColorAttachments;
				if (instance->m_RendererID)
				{
					glDeleteFramebuffers(1, &instance->m_RendererID);
					glDeleteTextures(1, &instance->m_DepthAttachment);
					glDeleteTextures(colorAttachment.size(), colorAttachment.data());

					colorAttachment.clear();
					instance->m_DepthAttachment = 0;
				}

				glGenFramebuffers(1, &instance->m_RendererID);
				glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);

				bool multisample = instance->m_Specification.Samples > 1;

				if (colorAttachmentsFormat.size())
				{
					colorAttachment.resize(colorAttachmentsFormat.size());
					Utils::CreateTextures(multisample, colorAttachment.data(), colorAttachment.size());

					for (int i = 0; i < colorAttachment.size(); i++)
					{
						Utils::BindTexture(multisample, colorAttachment[i]);
						switch (colorAttachmentsFormat[i])
						{
							case FramebufferTextureFormat::RGBA8:
								Utils::AttachColorTexture(colorAttachment[i], instance->m_Specification.Samples,
									GL_RGBA8, instance->m_Width, instance->m_Height, i);
								break;
							case FramebufferTextureFormat::RGBA16F:
								Utils::AttachColorTexture(colorAttachment[i], instance->m_Specification.Samples,
									GL_RGBA16F, instance->m_Width, instance->m_Height, i);
								break;
							case FramebufferTextureFormat::RGBA32F:
								Utils::AttachColorTexture(colorAttachment[i], instance->m_Specification.Samples,
									GL_RGBA32F, instance->m_Width, instance->m_Height, i);
								break;
							case FramebufferTextureFormat::RG32F:
								Utils::AttachColorTexture(colorAttachment[i], instance->m_Specification.Samples,
									GL_RG32F, instance->m_Width, instance->m_Height, i);
								break;
						}

					}
				}

				if (instance->m_DepthAttachmentFormat != FramebufferTextureFormat::None)
				{
					Utils::CreateTextures(multisample, &instance->m_DepthAttachment, 1);
					Utils::BindTexture(instance->m_Specification.Samples > 1, instance->m_DepthAttachment);

					switch (instance->m_DepthAttachmentFormat)
					{
					case FramebufferTextureFormat::DEPTH24STENCIL8:
						Utils::AttachDepthTexture(instance->m_DepthAttachment, instance->m_Specification.Samples,
							GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, instance->m_Width, instance->m_Height);
						break;
					case FramebufferTextureFormat::DEPTH32F:
						Utils::AttachDepthTexture(instance->m_DepthAttachment, instance->m_Specification.Samples,
							GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, instance->m_Width, instance->m_Height);
						break;
					}
				}

				if (colorAttachment.size() > 1)
				{
					RE_CORE_ASSERT(colorAttachment.size() <= 4);
					GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
					glDrawBuffers(colorAttachment.size(), buffers);
				}
				else if (colorAttachment.size() == 0)
				{
					glDrawBuffer(GL_NONE);
				}
#if 0
				RGBA8 = 1,
					RGBA16F = 2,
					RGBA32F = 3,
					RG32F = 4,
				Utils::CreateTexures(instance->m_Specification.Samples > 1, &instance->m_ColorAttachment, 1);
				Utils::BindTexture(instance->m_Specification.Samples > 1, instance->m_ColorAttachment);

				if (instance->m_Specification.Format == FramebufferTextureFormat::RGBA16F)
				{
					Utils::AttachColorTexture(instance->m_ColorAttachment, instance->m_Specification.Samples, GL_RGBA16F, instance->m_Width, instance->m_Height, 0);

				}
				else if (instance->m_Specification.Format == FramebufferTextureFormat::RGBA8)
				{
				
					Utils::AttachColorTexture(instance->m_ColorAttachment, instance->m_Specification.Samples, GL_RGBA, instance->m_Width, instance->m_Height, 0);
				}
				

				Utils::CreateTexures(instance->m_Specification.Samples > 1, &instance->m_DepthAttachment, 1);
				Utils::BindTexture(instance->m_Specification.Samples > 1, instance->m_DepthAttachment);

				Utils::AttachDepthTexture(instance->m_DepthAttachment, instance->m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, instance->m_Width, instance->m_Height);
#endif
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
		});
	}
	
	void OpenGLFramebuffer::Unbind() const
	{
		Renderer::Submit([]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		});
	}

	void OpenGLFramebuffer::BindTexture(size_t attachmentIndex, size_t slot) const
	{
		Ref<const OpenGLFramebuffer> instance = this;
		Renderer::Submit([instance, attachmentIndex, slot] ()
		{
			glBindTextureUnit(slot, instance->m_ColorAttachments.first[attachmentIndex]);
		});
	}
}