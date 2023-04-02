#include "pch.h"
#include "Renderer.h"

#include "Shader.h"

#include <glad/glad.h>

#include "RendererAPI.h"
#include "SceneRenderer.h"

namespace RockEngine
{
	RendererAPIType RendererAPI::s_RendererAPI = RendererAPIType::OpenGL;

	struct RendererData
	{
		RenderCommandQueue m_CommandQueue;

		Ref<ShaderLibrary> m_ShaderLibrary;

		Ref<RenderPass> m_ActiveRenderPass;

		Ref<VertexBuffer> m_FullscreenQuadVertexBuffer;
		Ref<IndexBuffer> m_FullscreenQuadIndexBuffer;
		Ref<Pipeline> m_FullscreenQuadPipeline;
	};

	static RendererData s_Data;

	void Renderer::BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear)
	{
		RE_CORE_ASSERT(renderPass, "Render pass cannot be null!");

		// TODO: Convert all of this into a render command buffer
		s_Data.m_ActiveRenderPass = renderPass;

		renderPass->GetSpecification().TargetFramebuffer->Bind();
		if (clear)
		{
			const glm::vec4& clearColor = renderPass->GetSpecification().TargetFramebuffer->GetSpecification().ClearColor;
			Renderer::Submit([=]() {
				RendererAPI::Clear(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
				});
		}

	}

	void Renderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> overrideMaterial)
	{
	// auto material = overrideMaterial ? overrideMaterial : mesh->GetMaterialInstance();
		// auto shader = material->GetShader();
		// TODO: Sort this out
		mesh->m_VertexBuffer->Bind();
		mesh->m_Pipeline->Bind();
		mesh->m_IndexBuffer->Bind();

		auto& materials = mesh->GetMaterials();
		for (Submesh& submesh : mesh->m_Submeshes)
		{
			// Material
			auto material = overrideMaterial ? overrideMaterial : materials[submesh.MaterialIndex];
			auto shader = material->GetShader();
			material->Bind();

			if (mesh->m_IsAnimated)
			{
				for (size_t i = 0; i < mesh->m_BoneTransforms.size(); i++)
				{
					std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
					shader->SetMat4(uniformName, mesh->m_BoneTransforms[i]);
				}
			}
			shader->SetMat4("u_Transform", transform * submesh.Transform);

			Renderer::Submit([submesh, material]() {
				if (material->GetFlag(MaterialFlag::DepthTest))	
					glEnable(GL_DEPTH_TEST);
				else
					glDisable(GL_DEPTH_TEST);

				if (!material->GetFlag(MaterialFlag::TwoSided))
					Renderer::Submit([]() { glEnable(GL_CULL_FACE); });
				else
					Renderer::Submit([]() { glDisable(GL_CULL_FACE); });

				glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
			});
		}
	}

	void Renderer::EndRenderPass()
	{
		RE_CORE_ASSERT(s_Data.m_ActiveRenderPass, "No active render pass! Have you called Renderer::EndRenderPass twice?");
		s_Data.m_ActiveRenderPass->GetSpecification().TargetFramebuffer->Unbind();
		s_Data.m_ActiveRenderPass = nullptr;
	}

	void Renderer::Init()
	{
		s_Data.m_ShaderLibrary = Ref<ShaderLibrary>::Create();
		Renderer::Submit([=]() {
			RendererAPI::Init();
			});

		s_Data.m_ShaderLibrary->Load("assets/shaders/ShaderPBR.glsl");
		SceneRenderer::Init();

		// Create fullscreen quad
		float x = -1;
		float y = -1;
		float width = 2, height = 2;
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		QuadVertex* data = new QuadVertex[4];

		data[0].Position = glm::vec3(x, y, 0.1f);
		data[0].TexCoord = glm::vec2(0, 0);

		data[1].Position = glm::vec3(x + width, y, 0.1f);
		data[1].TexCoord = glm::vec2(1, 0);

		data[2].Position = glm::vec3(x + width, y + height, 0.1f);
		data[2].TexCoord = glm::vec2(1, 1);

		data[3].Position = glm::vec3(x, y + height, 0.1f);
		data[3].TexCoord = glm::vec2(0, 1);

		PipelineSpecification pipelineSpecification;
		pipelineSpecification.Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};
		s_Data.m_FullscreenQuadPipeline = Pipeline::Create(pipelineSpecification);

		s_Data.m_FullscreenQuadVertexBuffer = VertexBuffer::Create(data, 4 * sizeof(QuadVertex));
		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
		s_Data.m_FullscreenQuadIndexBuffer = IndexBuffer::Create(indices, 6 * sizeof(uint32_t));

	}

	Ref<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		return s_Data.m_ShaderLibrary;
	}

	void Renderer::SubmitQuad(Ref<MaterialInstance> material, const glm::mat4& transform)
	{
		bool depthTest = true;
		if (material)
		{
			material->Bind();
			depthTest = material->GetFlag(MaterialFlag::DepthTest);

			auto shader = material->GetShader();
			shader->SetMat4("u_Transform", transform);
		}

		s_Data.m_FullscreenQuadVertexBuffer->Bind();
		s_Data.m_FullscreenQuadPipeline->Bind();
		s_Data.m_FullscreenQuadIndexBuffer->Bind();
		Renderer::DrawIndexed(6, depthTest);
	}

	void Renderer::SubmitFullscreenQuad(Ref<MaterialInstance> material)
	{
		bool depthTest = true;
		if (material)
		{
			material->Bind();
			depthTest = material->GetFlag(MaterialFlag::DepthTest);
		}

		s_Data.m_FullscreenQuadVertexBuffer->Bind();
		s_Data.m_FullscreenQuadPipeline->Bind();
		s_Data.m_FullscreenQuadIndexBuffer->Bind();
		Renderer::DrawIndexed(6, depthTest);

	}

	void Renderer::Clear(float r, float g, float b, float a /* = 1.0f */)
	{
		Renderer::Submit([=]() {
			RendererAPI::Clear(r,g,b,a);
			});
	}

	void Renderer::SetViewport(u32 Width, u32 Height, u32 x, u32 y)
	{
		Renderer::Submit([=]() {
			RendererAPI::SetViewport(Width,Height,x,y);
			});
	}

	void Renderer::DrawIndexed(u32 count, bool depthTest)
	{
		Renderer::Submit([=]() {
			RendererAPI::DrawIndexed(count, depthTest);
		});
	}

	void Renderer::WaitAndRender()
	{
		s_Data.m_CommandQueue.Execute();
	}

	RenderCommandQueue& Renderer::GetRenderCommandQueue()
	{
		return s_Data.m_CommandQueue;
	}
}