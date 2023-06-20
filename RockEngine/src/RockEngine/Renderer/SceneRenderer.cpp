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

	SceneRenderer::SceneRenderer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
		Init();
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::Init()
	{
		FramebufferSpecification geoFramebufferSpec;
		geoFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::Depth };
		geoFramebufferSpec.Samples = 8;
		geoFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		RenderPassSpecification geoRenderPassSpec;
		geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
		m_GeoPass = RenderPass::Create(geoRenderPassSpec);

		FramebufferSpecification compFramebufferSpec;
		compFramebufferSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
		compFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		RenderPassSpecification compRenderPassSpec;
		compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(compFramebufferSpec);
		m_CompositePass = RenderPass::Create(compRenderPassSpec);

		m_CompositeShader = Shader::Create("assets/shaders/SceneComposite.glsl");

		m_BRDFLUT = Texture2D::Create("assets/textures/BRDF_LUT.tga");

		// Grid
		auto gridShader = Shader::Create("assets/shaders/Grid.glsl");
		m_GridMaterial = MaterialInstance::Create(Material::Create(gridShader));
		m_GridMaterial->SetFlag(MaterialFlag::TwoSided, true);
		float gridScale = 16.025f, gridSize = 0.025f;
		m_GridMaterial->Set("u_Scale", gridScale);
		m_GridMaterial->Set("u_Res", gridSize);

		// Outline
		auto outlineShader = Shader::Create("assets/shaders/Outline.glsl");
		m_OutlineMaterial = MaterialInstance::Create(Material::Create(outlineShader));
		m_OutlineMaterial->SetFlag(MaterialFlag::DepthTest, false);

		// Shere Sun
		auto sphereShader = Shader::Create("assets/shaders/Outline.glsl");
		m_ColiderSphereMaterial = MaterialInstance::Create(Material::Create(sphereShader));
		m_ColiderSphereMaterial->SetFlag(MaterialFlag::DepthTest, false);

		// Shadow Map
		m_ShadowMapShader = Shader::Create("assets/shaders/ShadowMap.glsl");

		FramebufferSpecification shadowMapFramebufferSpec;
		shadowMapFramebufferSpec.Width = 1024 * 4;
		shadowMapFramebufferSpec.Height = 1024 * 4;
		shadowMapFramebufferSpec.Attachments = { FramebufferTextureFormat::DEPTH32F };
		shadowMapFramebufferSpec.ClearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
		shadowMapFramebufferSpec.NoResize = true;

		// 4 cascades
		for (int i = 0; i < 4; i++)
		{
			RenderPassSpecification shadowMapRenderPassSpec;
			shadowMapRenderPassSpec.TargetFramebuffer = Framebuffer::Create(shadowMapFramebufferSpec);
			m_ShadowMapRenderPass[i] = RenderPass::Create(shadowMapRenderPassSpec);
		}

		Renderer::Submit([this]()
			{
				glGenSamplers(1, &m_ShadowMapSampler);

				// Setup the shadowmap depth sampler
				glSamplerParameteri(m_ShadowMapSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glSamplerParameteri(m_ShadowMapSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glSamplerParameteri(m_ShadowMapSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glSamplerParameteri(m_ShadowMapSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			});
	}

	void SceneRenderer::BeginScene(const SceneRendererCamera& camera)
	{
		RE_CORE_ASSERT(m_Scene);
		RE_CORE_ASSERT(!m_Active);

		m_Active = true;

		m_SceneData.SceneCamera = camera;
		m_SceneData.SkyboxMaterial = m_Scene->m_SkyboxMaterial;
		m_SceneData.SceneEnvironment = m_Scene->m_Environment;
		m_SceneData.SceneEnvironmentIntensity = m_Scene->m_EnvironmentIntensity;
		m_SceneData.SceneLightEnvironment = m_Scene->m_LightEnvironment;
	}

	void SceneRenderer::EndScene()
	{
		RE_CORE_ASSERT(m_Active);
		FlushDrawList();
		m_Active = false;
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth != width || m_ViewportHeight != height)
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;

			m_GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
			m_CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);
			//m_NeedsResize = true;
		}
	}

	void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> overrideMaterial)
	{
		// TODO: Culling, sorting, etc.
		m_DrawList.push_back({ mesh, overrideMaterial, transform });
		m_ShadowPassDrawList.push_back({ mesh, overrideMaterial, transform });
	}

	void SceneRenderer::SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform)
	{
		m_SelectedMeshDrawList.push_back({ mesh, nullptr, transform });
		m_ShadowPassDrawList.push_back({ mesh, nullptr, transform });
	}

	void SceneRenderer::SubmitMeshWithShader(Ref<Mesh> mesh, const glm::mat4& transform, Ref<Shader> shader)
	{
		m_DrawListWithShader.push_back({ mesh, shader, transform});
	}

	void SceneRenderer::SubmitSoliderSphere(Ref<Mesh> mesh, const glm::mat4& transform)
	{
		m_DrawColiderSphere.push_back({ mesh, m_ColiderSphereMaterial, transform });
	}

	static Ref<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

	Environment SceneRenderer::CreateEnvironmentMap(const std::filesystem::path& filepath)
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
			for (int level = 1, size = cubemapSize / 2; level < envFiltered->GetMipLevelCount(); level++, size /= 2) // <= ?
			{
				const GLuint numGroups = glm::max(1, size / 32);
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

	void SceneRenderer::GeometryPass()
	{
		bool outline = m_SelectedMeshDrawList.size() > 0;
		bool collider = m_DrawColiderSphere.size() > 0;

		if (outline)
		{
			Renderer::Submit([]()
				{
					glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				});
		}

		Renderer::BeginRenderPass(m_GeoPass);

		if (outline)
		{
			Renderer::Submit([]()
				{
					glStencilMask(0);
				});
		}

		auto& sceneCamera = m_SceneData.SceneCamera;

		auto viewProjection = sceneCamera.Camera.GetProjectionMatrix() * sceneCamera.ViewMatrix;
		glm::vec3 cameraPosition = glm::inverse(m_SceneData.SceneCamera.ViewMatrix)[3]; // TODO: Negate instead

		// Skybox
		auto skyboxShader = m_SceneData.SkyboxMaterial->GetShader();
		m_SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
		m_SceneData.SkyboxMaterial->Set("u_SkyIntensity", m_SceneData.SceneEnvironmentIntensity);
		Renderer::SubmitFullscreenQuad(m_SceneData.SkyboxMaterial);

		float aspectRatio = (float)m_GeoPass->GetSpecification().TargetFramebuffer->GetWidth() / (float)m_GeoPass->GetSpecification().TargetFramebuffer->GetHeight();
		float frustumSize = 2.0f * sceneCamera.Near * glm::tan(sceneCamera.FOV * 0.5f) * aspectRatio;

		for (auto& dc : m_DrawList)
		{
			auto baseMaterial = dc.Mesh->GetMaterial();
			baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
			baseMaterial->Set("u_ViewMatrix", sceneCamera.ViewMatrix);
			baseMaterial->Set("u_CameraPosition", cameraPosition);
			baseMaterial->Set("u_LightMatrixCascade0", ShadowSettings.LightMatrices[0]);
			baseMaterial->Set("u_LightMatrixCascade1", ShadowSettings.LightMatrices[1]);
			baseMaterial->Set("u_LightMatrixCascade2", ShadowSettings.LightMatrices[2]);
			baseMaterial->Set("u_LightMatrixCascade3", ShadowSettings.LightMatrices[3]);
			baseMaterial->Set("u_ShowCascades", ShadowSettings.ShowCascades);
			baseMaterial->Set("u_LightView", ShadowSettings.LightViewMatrix);
			baseMaterial->Set("u_CascadeSplits", ShadowSettings.CascadeSplits);
			baseMaterial->Set("u_SoftShadows", ShadowSettings.SoftShadows);
			baseMaterial->Set("u_LightSize", ShadowSettings.LightSize);
			baseMaterial->Set("u_MaxShadowDistance", ShadowSettings.MaxShadowDistance);
			baseMaterial->Set("u_ShadowFade", ShadowSettings.ShadowFade);
			baseMaterial->Set("u_CascadeFading", ShadowSettings.CascadeFading);
			baseMaterial->Set("u_CascadeTransitionFade", ShadowSettings.CascadeTransitionFade);
			baseMaterial->Set("u_IBLContribution", m_SceneData.SceneEnvironmentIntensity);

			// Environment (TODO: don't do this per mesh)
			baseMaterial->Set("u_EnvRadianceTex", m_SceneData.SceneEnvironment.RadianceMap);
			baseMaterial->Set("u_EnvIrradianceTex", m_SceneData.SceneEnvironment.IrradianceMap);
			baseMaterial->Set("u_BRDFLUTTexture", m_BRDFLUT);

			// Set lights (TODO: move to light environment and don't do per mesh)
			auto directionalLight = m_SceneData.SceneLightEnvironment.DirectionalLights[0];
			baseMaterial->Set("u_DirectionalLights", directionalLight);

			auto rd = baseMaterial->FindResourceDeclaration("u_ShadowMapTexture");
			if (rd)
			{
				auto reg = rd->GetRegister();

				auto tex = m_ShadowMapRenderPass[0]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
				auto tex1 = m_ShadowMapRenderPass[1]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
				auto tex2 = m_ShadowMapRenderPass[2]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
				auto tex3 = m_ShadowMapRenderPass[3]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();

				RendererID Sampler = m_ShadowMapSampler;
				Renderer::Submit([reg, tex, tex1, tex2, tex3, Sampler]() mutable
					{
						// 4 cascades
						glBindTextureUnit(reg, tex);
						glBindSampler(reg++, Sampler);

						glBindTextureUnit(reg, tex1);
						glBindSampler(reg++, Sampler);

						glBindTextureUnit(reg, tex2);
						glBindSampler(reg++, Sampler);

						glBindTextureUnit(reg, tex3);
						glBindSampler(reg++, Sampler);
					});
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
		for (auto& dc : m_SelectedMeshDrawList)
		{
			auto baseMaterial = dc.Mesh->GetMaterial();
			baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
			baseMaterial->Set("u_ViewMatrix", sceneCamera.ViewMatrix);
			baseMaterial->Set("u_CameraPosition", cameraPosition);
			baseMaterial->Set("u_CascadeSplits", ShadowSettings.CascadeSplits);
			baseMaterial->Set("u_ShowCascades", ShadowSettings.ShowCascades);
			baseMaterial->Set("u_SoftShadows", ShadowSettings.SoftShadows);
			baseMaterial->Set("u_LightSize", ShadowSettings.LightSize);
			baseMaterial->Set("u_MaxShadowDistance", ShadowSettings.MaxShadowDistance);
			baseMaterial->Set("u_ShadowFade", ShadowSettings.ShadowFade);
			baseMaterial->Set("u_CascadeFading", ShadowSettings.CascadeFading);
			baseMaterial->Set("u_CascadeTransitionFade", ShadowSettings.CascadeTransitionFade);
			baseMaterial->Set("u_IBLContribution", m_SceneData.SceneEnvironmentIntensity);

			// Environment (TODO: don't do this per mesh)
			baseMaterial->Set("u_EnvRadianceTex", m_SceneData.SceneEnvironment.RadianceMap);
			baseMaterial->Set("u_EnvIrradianceTex", m_SceneData.SceneEnvironment.IrradianceMap);
			baseMaterial->Set("u_BRDFLUTTexture", m_BRDFLUT);

			baseMaterial->Set("u_LightMatrixCascade0", ShadowSettings.LightMatrices[0]);
			baseMaterial->Set("u_LightMatrixCascade1", ShadowSettings.LightMatrices[1]);
			baseMaterial->Set("u_LightMatrixCascade2", ShadowSettings.LightMatrices[2]);
			baseMaterial->Set("u_LightMatrixCascade3", ShadowSettings.LightMatrices[3]);

			// Set lights (TODO: move to light environment and don't do per mesh)
			baseMaterial->Set("u_DirectionalLights", m_SceneData.SceneLightEnvironment.DirectionalLights[0]);

			auto rd = baseMaterial->FindResourceDeclaration("u_ShadowMapTexture");
			if (rd)
			{
				auto reg = rd->GetRegister();

				auto tex = m_ShadowMapRenderPass[0]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
				auto tex1 = m_ShadowMapRenderPass[1]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
				auto tex2 = m_ShadowMapRenderPass[2]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();
				auto tex3 = m_ShadowMapRenderPass[3]->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID();

				RendererID Sampler = m_ShadowMapSampler;
				Renderer::Submit([reg, tex, tex1, tex2, tex3, Sampler]() mutable
					{
						// 4 cascades
						glBindTextureUnit(reg, tex);
						glBindSampler(reg++, Sampler);

						glBindTextureUnit(reg, tex1);
						glBindSampler(reg++, Sampler);

						glBindTextureUnit(reg, tex2);
						glBindSampler(reg++, Sampler);

						glBindTextureUnit(reg, tex3);
						glBindSampler(reg++, Sampler);
					});
			}

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
			m_OutlineMaterial->Set("u_ViewProjection", viewProjection);
			for (auto& dc : m_SelectedMeshDrawList)
			{
				Renderer::SubmitMesh(dc.Mesh, dc.Transform, m_OutlineMaterial);
			}

			Renderer::Submit([]()
				{
					glPointSize(10);
					glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				});
			for (auto& dc : m_SelectedMeshDrawList)
			{
				Renderer::SubmitMesh(dc.Mesh, dc.Transform, m_OutlineMaterial);
			}

			Renderer::Submit([]()
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glStencilMask(0xff);
					glStencilFunc(GL_ALWAYS, 1, 0xff);
					glEnable(GL_DEPTH_TEST);
				});
		}

		m_ColiderSphereMaterial->Set("u_ViewProjection", viewProjection);
		for (auto& dc : m_DrawColiderSphere)
		{
			if (dc.Mesh)
				Renderer::SubmitMesh(dc.Mesh, dc.Transform, m_ColiderSphereMaterial);
		}

		// Grid
		if (GetOptions().ShowGrid)
		{
			m_GridMaterial->Set("u_ViewProjection", viewProjection);
			Renderer::SubmitQuad(m_GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
		}

		if (GetOptions().ShowBoundingBoxes)
		{
			Renderer2D::BeginScene(viewProjection);
			for (auto& dc : m_DrawList)
				Renderer::DrawAABB(dc.Mesh, dc.Transform);
			Renderer2D::EndScene();
		}

		Renderer::EndRenderPass();
	}

	void SceneRenderer::CompositePass()
	{
		auto& compositeBuffer = m_CompositePass->GetSpecification().TargetFramebuffer;

		Renderer::BeginRenderPass(m_CompositePass);
		m_CompositeShader->Bind();
		m_CompositeShader->SetFloat("u_Exposure", m_SceneData.SceneCamera.Camera.GetExposure());
		m_CompositeShader->SetInt("u_TextureSamples", m_GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);
		m_CompositeShader->SetFloat2("u_ViewportSize", glm::vec2(compositeBuffer->GetWidth(), compositeBuffer->GetHeight()));
		m_GeoPass->GetSpecification().TargetFramebuffer->BindTexture();
		Renderer::Submit([this]()
			{
				glBindTextureUnit(1, m_GeoPass->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID());
			});
		Renderer::SubmitFullscreenQuad(nullptr);
		Renderer::EndRenderPass();
	}

	void SceneRenderer::CalculateCascades(CascadeData* cascades, const glm::vec3& lightDirection)
	{
		auto& sceneCamera = m_SceneData.SceneCamera;
		auto viewProjection = sceneCamera.Camera.GetProjectionMatrix() * sceneCamera.ViewMatrix;

		const int SHADOW_MAP_CASCADE_COUNT = 4;
		float cascadeSplits[SHADOW_MAP_CASCADE_COUNT];

		// TODO: less hard-coding!
		float nearClip = 0.1f;
		float farClip = 1000.0f;
		float clipRange = farClip - nearClip;

		float minZ = nearClip;
		float maxZ = nearClip + clipRange;

		float range = maxZ - minZ;
		float ratio = maxZ / minZ;

		// Calculate split depths based on view camera frustum
		// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
			float log = minZ * std::pow(ratio, p);
			float uniform = minZ + range * p;
			float d = ShadowSettings.CascadeSplitLambda * (log - uniform) + uniform;
			cascadeSplits[i] = (d - nearClip) / clipRange;
		}

		cascadeSplits[3] = 0.3f;

		// Manually set cascades here
		// cascadeSplits[0] = 0.05f;
		// cascadeSplits[1] = 0.15f;
		// cascadeSplits[2] = 0.3f;
		// cascadeSplits[3] = 1.0f;

		// Calculate orthographic projection matrix for each cascade
		float lastSplitDist = 0.0;
		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			float splitDist = cascadeSplits[i];

			glm::vec3 frustumCorners[8] =
			{
				glm::vec3(-1.0f,  1.0f, -1.0f),
				glm::vec3(1.0f,  1.0f, -1.0f),
				glm::vec3(1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f, -1.0f,  1.0f),
				glm::vec3(-1.0f, -1.0f,  1.0f),
			};

			// Project frustum corners into world space
			glm::mat4 invCam = glm::inverse(viewProjection);
			for (uint32_t i = 0; i < 8; i++)
			{
				glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
				frustumCorners[i] = invCorner / invCorner.w;
			}

			for (uint32_t i = 0; i < 4; i++)
			{
				glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
				frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
				frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
			}

			// Get frustum center
			glm::vec3 frustumCenter = glm::vec3(0.0f);
			for (uint32_t i = 0; i < 8; i++)
				frustumCenter += frustumCorners[i];

			frustumCenter /= 8.0f;

			//frustumCenter *= 0.01f;

			float radius = 0.0f;
			for (uint32_t i = 0; i < 8; i++)
			{
				float distance = glm::length(frustumCorners[i] - frustumCenter);
				radius = glm::max(radius, distance);
			}
			radius = std::ceil(radius * 16.0f) / 16.0f;

			glm::vec3 maxExtents = glm::vec3(radius);
			glm::vec3 minExtents = -maxExtents;

			glm::vec3 lightDir = -lightDirection;
			glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f + ShadowSettings.CascadeNearPlaneOffset, 
				maxExtents.z - minExtents.z + ShadowSettings.CascadeFarPlaneOffset);

			// Store split distance and matrix in cascade
			cascades[i].SplitDepth = (nearClip + splitDist * clipRange) * -1.0f;
			cascades[i].ViewProj = lightOrthoMatrix * lightViewMatrix;
			cascades[i].View = lightViewMatrix;

			lastSplitDist = cascadeSplits[i];
		}
	}

	void SceneRenderer::ShadowMapPass()
	{
		auto& directionalLights = m_SceneData.SceneLightEnvironment.DirectionalLights;
		if (directionalLights[0].Multiplier == 0.0f || !directionalLights[0].CastShadows)
		{
			for (int i = 0; i < 4; i++)
			{
				// Clear shadow maps
				Renderer::BeginRenderPass(m_ShadowMapRenderPass[i]);
				Renderer::EndRenderPass();
			}
			return;
		}

		CascadeData cascades[4] = {};
		CalculateCascades(cascades, directionalLights[0].Direction);
		ShadowSettings.LightViewMatrix = cascades[0].View;

		Renderer::Submit([]()
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			});

		for (int i = 0; i < 4; i++)
		{
			ShadowSettings.CascadeSplits[i] = cascades[i].SplitDepth;

			Renderer::BeginRenderPass(m_ShadowMapRenderPass[i]);

			glm::mat4 shadowMapVP = cascades[i].ViewProj;
			m_ShadowMapShader->SetMat4("u_ViewProjection", shadowMapVP);

			static glm::mat4 scaleBiasMatrix = glm::scale(glm::mat4(1.0f), { 0.5f, 0.5f, 0.5f }) * glm::translate(glm::mat4(1.0f), { 1, 1, 1 });
			ShadowSettings.LightMatrices[i] = scaleBiasMatrix * cascades[i].ViewProj;


			// Render entities
			for (auto& dc : m_ShadowPassDrawList)
			{
				Renderer::SubmitMeshWithShader(dc.Mesh, dc.Transform, m_ShadowMapShader);
			}

			Renderer::EndRenderPass();
		}
	}

	void SceneRenderer::FlushDrawList()
	{
		RE_CORE_ASSERT(m_Scene);

		memset(&m_Stats, 0, sizeof(SceneRendererStats));

		{
			Renderer::Submit([this]()
				{
					m_Stats.ShadowPassTimer.Reset();
				});
			ShadowMapPass();
			Renderer::Submit([this]
				{
					m_Stats.ShadowPass = m_Stats.ShadowPassTimer.ElapsedMillis();
				});
		}
		{
			Renderer::Submit([this]()
				{
					m_Stats.GeometryPassTimer.Reset();
				});
			GeometryPass();
			Renderer::Submit([this]
				{
					m_Stats.GeometryPass = m_Stats.GeometryPassTimer.ElapsedMillis();
				});
		}
		{
			Renderer::Submit([this]()
				{
					m_Stats.CompositePassTimer.Reset();
				});

			CompositePass();
			Renderer::Submit([this]
				{
					m_Stats.CompositePass = m_Stats.CompositePassTimer.ElapsedMillis();
				});

			//	BloomBlurPass();
		}

		m_DrawList.clear();
		m_SelectedMeshDrawList.clear();
		m_ShadowPassDrawList.clear();
		m_DrawColiderSphere.clear();
		m_SceneData = {};
	}

	Ref<Texture2D> SceneRenderer::GetFinalColorBuffer()
	{
		// return m_CompositePass->GetSpecification().TargetFramebuffer;
		RE_CORE_ASSERT(false, "Not implemented");
		return nullptr;
	}

	Ref<RenderPass> SceneRenderer::GetFinalRenderPass()
	{
		return m_CompositePass;
	}

	uint32_t SceneRenderer::GetFinalColorBufferRendererID()
	{
		return m_CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID();
	}

	void SceneRenderer::SetFocusPoint(const glm::vec2& point)
	{
		m_FocusPoint = point;
	}

	SceneRendererOptions& SceneRenderer::GetOptions()
	{
		return m_Options;
	}
}