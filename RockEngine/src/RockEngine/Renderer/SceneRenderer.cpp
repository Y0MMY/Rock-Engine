#include "pch.h"
#include "SceneRenderer.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "RockEngine/Scene/Entity.h"

#include "SceneEnvironment.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

namespace RockEngine
{
	struct SceneRendererData
	{
		const Scene* ActiveScene = nullptr;
		struct SceneInfo
		{
			SceneRendererCamera SceneCamera;

			// Resources
			Ref<MaterialInstance> SkyboxMaterial;
			Environment SceneEnvironment;
			float SceneEnvironmentIntensity;
			LightEnvironment SceneLightEnvironment;
		} SceneData;

		Ref<Texture2D> BRDFLUT;
		Ref<Shader> CompositeShader;
		Ref<Shader> ShadowMapShader;

		Ref<RenderPass> ShadowMapRenderPass[4];
		Ref<RenderPass> GeoPass;
		Ref<RenderPass> CompositePass;

		// Shadows Map
		struct ShadowMapSettings
		{
			RendererID ShadowMapSampler;
		};
		ShadowMapSettings s_ShadowMap;

		struct DrawCommand
		{
			Ref<Mesh> Mesh;
			Ref<MaterialInstance> Material;
			glm::mat4 Transform;
		};
		std::vector<DrawCommand> DrawList;
		std::vector<DrawCommand> SelectedMeshDrawList;
		std::vector<DrawCommand> ShadowPassDrawList;
		SceneRendererOptions Options;

		// Grid
		Ref<MaterialInstance> GridMaterial;

		Ref<MaterialInstance> OutlineMaterial, OutlineAnimMaterial;
	};

	SceneRendererData* s_Data = nullptr;

	void SceneRenderer::Init()
	{
		s_Data = new SceneRendererData();

		////////////////////////////////////////////////////////////////////////////////////

		FramebufferSpecification geoFramebufferSpec;
		geoFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::Depth };
		geoFramebufferSpec.Samples = 8;
		geoFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		RenderPassSpecification geoRenderPassSpec;
		geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
		s_Data->GeoPass = RenderPass::Create(geoRenderPassSpec);

		////////////////////////////////////////////////////////////////////////////////////

		FramebufferSpecification compFramebufferSpec;
		compFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
		compFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		RenderPassSpecification compRenderPassSpec;
		compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(compFramebufferSpec);
		s_Data->CompositePass = RenderPass::Create(compRenderPassSpec);

		////////////////////////////////////////////////////////////////////////////////////

		//Shadows
		FramebufferSpecification shadowFrameBufferSpec;
		shadowFrameBufferSpec.Attachments = { FramebufferTextureFormat::DEPTH32F };
		shadowFrameBufferSpec.Width = 2000;
		shadowFrameBufferSpec.Width = 2000;
		shadowFrameBufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };


		RenderPassSpecification shadowRenderPassSpec;

		for (int i = 0; i < 4; i++)
		{
			shadowRenderPassSpec.TargetFramebuffer = Framebuffer::Create(shadowFrameBufferSpec);
			s_Data->ShadowMapRenderPass[i] = RenderPass::Create(shadowRenderPassSpec);
		}

		Renderer::Submit([]()
			{
				glGenSamplers(1, &s_Data->s_ShadowMap.ShadowMapSampler);

				// Setup the shadowmap depth sampler
				glSamplerParameteri(s_Data->s_ShadowMap.ShadowMapSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glSamplerParameteri(s_Data->s_ShadowMap.ShadowMapSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glSamplerParameteri(s_Data->s_ShadowMap.ShadowMapSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glSamplerParameteri(s_Data->s_ShadowMap.ShadowMapSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			});
		////////////////////////////////////////////////////////////////////////////////////

		s_Data->BRDFLUT = Texture2D::Create("assets/textures/BRDF_LUT.tga");
		s_Data->CompositeShader = Shader::Create("assets/shaders/SceneComposite.glsl");

		// Shadows 
		s_Data->ShadowMapShader = Shader::Create("assets/shaders/ShadowMap.glsl");

		// Outline
		auto outlineShader = Shader::Create("assets/shaders/Outline.glsl");
		s_Data->OutlineMaterial = MaterialInstance::Create(Material::Create(outlineShader));
		s_Data->OutlineMaterial->SetFlag(MaterialFlag::DepthTest, false);

		auto outlineAnimShader = Shader::Create("assets/shaders/Outline_Anim.glsl");
		s_Data->OutlineAnimMaterial = MaterialInstance::Create(Material::Create(outlineAnimShader));
		s_Data->OutlineAnimMaterial->SetFlag(MaterialFlag::DepthTest, false);

		// Grid
		auto gridShader = Shader::Create("assets/shaders/Grid.glsl");
		s_Data->GridMaterial = MaterialInstance::Create(Material::Create(gridShader));
		float gridScale = 16.025f, gridSize = 0.025f;
		s_Data->GridMaterial->Set("u_Scale", gridScale);
		s_Data->GridMaterial->Set("u_Res", gridSize);
	}

	void SceneRenderer::SetViewportSize(u32 width, u32 height)
	{
		s_Data->GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
		s_Data->CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);

		//m_ViewportWidth = width;
		//m_ViewportHeight = height;
	}

	void SceneRenderer::Shutdown()
	{
		delete s_Data;
	}

	void SceneRenderer::BeginScene(const Scene* scene, const SceneRendererCamera& camera)
	{
		RE_CORE_ASSERT(!s_Data->ActiveScene, "");
		s_Data->ActiveScene = scene;

		s_Data->SceneData.SceneCamera = camera;
		s_Data->SceneData.SkyboxMaterial = scene->m_SkyboxMaterial;
		s_Data->SceneData.SceneEnvironment = scene->m_Environment;
		s_Data->SceneData.SceneEnvironmentIntensity = scene->m_EnvironmentIntensity;
		s_Data->SceneData.SceneLightEnvironment = scene->m_LightEnvironment;
	}

	void SceneRenderer::EndScene()
	{
		RE_CORE_ASSERT(s_Data->ActiveScene, "");
		s_Data->ActiveScene = nullptr;

		FlushDrawList();
	}

	SceneRendererOptions& SceneRenderer::GetOptions()
	{
		return s_Data->Options;
	}

	void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> overrideMaterial)
	{
		// TODO: Culling, sorting, etc.
		s_Data->DrawList.push_back({ mesh, overrideMaterial, transform });
		s_Data->ShadowPassDrawList.push_back({mesh, overrideMaterial, transform });
	}

	void SceneRenderer::SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform)
	{
		// TODO: Culling, sorting, etc.
		s_Data->SelectedMeshDrawList.push_back({ mesh, nullptr, transform });
		s_Data->ShadowPassDrawList.push_back({ mesh, nullptr, transform });
	}

	static Ref<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

	std::pair<Ref<TextureCube>, Ref<TextureCube>> SceneRenderer::CreateEnvironmentMap(const std::filesystem::path& filepath)
	{
		const uint32_t cubemapSize = 2048;
		const uint32_t irradianceMapSize = 32;

		Ref<TextureCube> envUnfiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);
		if (!equirectangularConversionShader)
			equirectangularConversionShader = Shader::Create("assets/shaders/EquirectangularToCubeMap.glsl");
		Ref<Texture2D> envEquirect = Texture2D::Create(filepath);
		RE_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::Float16, "Texture is not HDR!");

		equirectangularConversionShader->Bind();
		envEquirect->Bind();
		Renderer::Submit([envUnfiltered, cubemapSize, envEquirect]()
			{
				glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
				glGenerateTextureMipmap(envUnfiltered->GetRendererID());
			});


		if (!envFilteringShader)
			envFilteringShader = Shader::Create("assets/shaders/EnvironmentMipFilter.glsl");

		Ref<TextureCube> envFiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);

		Renderer::Submit([envUnfiltered, envFiltered]()
			{
				glCopyImageSubData(envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
					envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
					envFiltered->GetWidth(), envFiltered->GetHeight(), 6);
			});

		envFilteringShader->Bind();
		envUnfiltered->Bind();

		Renderer::Submit([envUnfiltered, envFiltered, cubemapSize]() {
			const float deltaRoughness = 1.0f / glm::max((float)(envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
			for (u32 level = 1, size = cubemapSize / 2; level < envFiltered->GetMipLevelCount(); level++, size /= 2) // <= ?
			{
				const GLuint numGroups = glm::max(1, (int)size / 32);
				glBindImageTexture(0, envFiltered->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glProgramUniform1f(envFilteringShader->GetRendererID(), 0, level * deltaRoughness);
				glDispatchCompute(numGroups, numGroups, 6);
			}
			});

		if (!envIrradianceShader)
			envIrradianceShader = Shader::Create("assets/shaders/EnvironmentIrradiance.glsl");

		Ref<TextureCube> irradianceMap = TextureCube::Create(TextureFormat::Float16, irradianceMapSize, irradianceMapSize);
		envIrradianceShader->Bind();
		envFiltered->Bind();
		Renderer::Submit([irradianceMap]()
			{
				glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
				glGenerateTextureMipmap(irradianceMap->GetRendererID());
			});

		return { envFiltered, irradianceMap };

	}

	Ref<Texture2D> SceneRenderer::GetFinalColorBuffer()
	{
		RE_CORE_ASSERT(false, "Not implemented");
		return nullptr;

	}

	uint32_t SceneRenderer::GetFinalColorBufferRendererID()
	{
		return s_Data->CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID();
		return 0;
	}

	Ref<RenderPass> SceneRenderer::GetFinalRenderPass()
	{
		return s_Data->CompositePass;
	}

	void SceneRenderer::FlushDrawList()
	{
		RE_CORE_ASSERT(!s_Data->ActiveScene, "");
		GeometryPass();
		CompositePass();

		s_Data->DrawList.clear();
		s_Data->SelectedMeshDrawList.clear();
		s_Data->SceneData = { };
	}

	void SceneRenderer::GeometryPass()
	{
		bool outline = s_Data->SelectedMeshDrawList.size() > 0;

		if (outline)
		{
			Renderer::Submit([]()
				{
					glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				});
		}

		Renderer::BeginRenderPass(s_Data->GeoPass);

		if (outline)
		{
			Renderer::Submit([]()
				{
					glStencilMask(0);
				});
		}

		auto& sceneCamera = s_Data->SceneData.SceneCamera;

		auto viewProjection = sceneCamera.Camera.GetProjectionMatrix() * sceneCamera.ViewMatrix;
		glm::vec3 cameraPosition = glm::inverse(viewProjection)[3]; // TODO: Negate instead

		// Skybox
		auto skyboxShader = s_Data->SceneData.SkyboxMaterial->GetShader();
		s_Data->SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
		s_Data->SceneData.SkyboxMaterial->Set("u_SkyIntensity", s_Data->SceneData.SceneEnvironmentIntensity);
		Renderer::SubmitFullscreenQuad(s_Data->SceneData.SkyboxMaterial);

		// Point Lights
		size_t pointLightsCount = s_Data->SceneData.SceneLightEnvironment.PointLights.size();

		// Render entities
		for (auto& dc : s_Data->DrawList)
		{
			auto baseMaterial = dc.Mesh->GetMaterial();
			baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
			baseMaterial->Set("u_CameraPosition", cameraPosition);

			// Environment (TODO: don't do this per mesh)
			baseMaterial->Set("u_EnvRadianceTex", s_Data->SceneData.SceneEnvironment.RadianceMap);
			baseMaterial->Set("u_EnvIrradianceTex", s_Data->SceneData.SceneEnvironment.IrradianceMap);
			baseMaterial->Set("u_BRDFLUTTexture", s_Data->BRDFLUT);

			// Set lights (TODO: move to light environment and don't do per mesh)
			auto directionalLight = s_Data->SceneData.SceneLightEnvironment.DirectionalLights[0];
			baseMaterial->Set("u_DirectionalLights", directionalLight);

			if (pointLightsCount)
			{
				baseMaterial->Set("u_PointLightsCount", pointLightsCount);
				size_t pointLightIndex = 0;
				for (const auto l : s_Data->SceneData.SceneLightEnvironment.PointLights)
				{
					std::string unifromName = "u_PointLights[" + std::to_string(pointLightIndex) + "]";
					std::string Position = unifromName + ".Position";
					std::string Intensity = unifromName + ".Intensity";
					std::string Radiance = unifromName + ".Radiance";
					std::string MinRadius = unifromName + ".MinRadius";
					std::string Radius = unifromName + ".Radius";
					std::string Falloff = unifromName + ".Falloff";
					std::string LightSize = unifromName + ".LightSize";

					baseMaterial->GetShader()->SetFloat3(Position, l.Position);
					baseMaterial->GetShader()->SetFloat(Intensity, l.Intensity);
					baseMaterial->GetShader()->SetFloat3(Radiance, l.Radiance);
					baseMaterial->GetShader()->SetFloat(MinRadius, l.MinRadius);
					baseMaterial->GetShader()->SetFloat(Radius, l.Radius);
					baseMaterial->GetShader()->SetFloat(Falloff, l.Falloff);
					baseMaterial->GetShader()->SetFloat(LightSize, l.SourceSize);
					pointLightIndex++;
				}
			}

			auto overrideMaterial = nullptr; // dc.Material;
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
		}

		if (outline)
		{
			Renderer::Submit([]()
				{
					glStencilFunc(GL_ALWAYS, 1, 0xff);
					glStencilMask(0xff);
				});
		}

		// Render selected entities

		for (auto& dc : s_Data->SelectedMeshDrawList)
		{
			auto baseMaterial = dc.Mesh->GetMaterial();
			baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
			baseMaterial->Set("u_CameraPosition", cameraPosition);

			// Environment (TODO: don't do this per mesh)
			baseMaterial->Set("u_EnvRadianceTex", s_Data->SceneData.SceneEnvironment.RadianceMap);
			baseMaterial->Set("u_EnvIrradianceTex", s_Data->SceneData.SceneEnvironment.IrradianceMap);
			baseMaterial->Set("u_BRDFLUTTexture", s_Data->BRDFLUT);

			// Set lights (TODO: move to light environment and don't do per mesh)
			auto directionalLight = s_Data->SceneData.SceneLightEnvironment.DirectionalLights[0];
			baseMaterial->Set("u_DirectionalLights", directionalLight);

			auto overrideMaterial = nullptr; // dc.Material;
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
		}

		if (outline)
		{
			Renderer::Submit([]()
				{
					glStencilFunc(GL_NOTEQUAL, 1, 0xff);
					glStencilMask(0);

					glLineWidth(10);
					glEnable(GL_LINE_SMOOTH);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDisable(GL_DEPTH_TEST);
				});

			// Draw outline here
			s_Data->OutlineMaterial->Set("u_ViewProjection", viewProjection);
			s_Data->OutlineAnimMaterial->Set("u_ViewProjection", viewProjection);
			for (auto& dc : s_Data->SelectedMeshDrawList)
			{
				Renderer::SubmitMesh(dc.Mesh, dc.Transform, dc.Mesh->IsAnimated() ? s_Data->OutlineAnimMaterial : s_Data->OutlineMaterial);
			}

			Renderer::Submit([]()
				{
					glPointSize(10);
					glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				});
			for (auto& dc : s_Data->SelectedMeshDrawList)
			{
				Renderer::SubmitMesh(dc.Mesh, dc.Transform, dc.Mesh->IsAnimated() ? s_Data->OutlineAnimMaterial : s_Data->OutlineMaterial);
			}

			Renderer::Submit([]()
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glStencilMask(0xff);
					glStencilFunc(GL_ALWAYS, 1, 0xff);
					glEnable(GL_DEPTH_TEST);
				});
		}

		// Grid
		if (GetOptions().ShowGrid)
		{
			s_Data->GridMaterial->Set("u_ViewProjection", viewProjection);
			Renderer::SubmitQuad(s_Data->GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
		}

		// Show Bounding Boxes
		if (GetOptions().ShowBoundingBoxes)
		{
			Renderer2D::BeginScene(viewProjection);
			for (auto& dc : s_Data->DrawList)
				Renderer::DrawAABB(dc.Mesh, dc.Transform);
			Renderer2D::EndScene();
		}

		Renderer::EndRenderPass();
	}

	void SceneRenderer::CompositePass()
	{
		Renderer::BeginRenderPass(s_Data->CompositePass);
		s_Data->CompositeShader->Bind();
		s_Data->CompositeShader->SetFloat("u_Exposure", s_Data->SceneData.SceneCamera.Camera.GetExposure());
		s_Data->CompositeShader->SetInt("u_TextureSamples", s_Data->GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);

		s_Data->GeoPass->GetSpecification().TargetFramebuffer->BindTexture();
		Renderer::SubmitFullscreenQuad(nullptr);
		Renderer::EndRenderPass();
	}

	void SceneRenderer::ShadowMapPass()
	{
		
	}
}