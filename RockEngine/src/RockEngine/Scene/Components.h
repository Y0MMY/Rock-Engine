#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "RockEngine/Core/UUID.h"
#include "RockEngine/Renderer/SceneEnvironment.h"

namespace RockEngine
{
	enum class DrawTarget // TODO: Move to Assets
	{
		None = 0, 
		Draw,
		DrawWithShader,
		DrawSphere
	};

	struct IDComponent
	{
		UUID ID = 0;
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		glm::vec3 WorldTranslation = { 0.0f, 0.0f, 0.0f };

		glm::vec3 Up = { 0.0F, 1.0F, 0.0F };
		glm::vec3 Right = { 1.0F, 0.0F, 0.0F };
		glm::vec3 Forward = { 0.0F, 0.0F, -1.0F };

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(Rotation))
				* glm::scale(glm::mat4(1.0f), Scale);
		}

	};


	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent& other) = default;
		~TagComponent() = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}

		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};

	struct RendererComponent
	{
		bool Visible = true;
		bool DepthTest = false;
	};

	struct MeshComponent
	{
		Ref<Mesh> Mesh;
		MeshComponent() = default;
		MeshComponent(const MeshComponent& other) = default;
		MeshComponent(const Ref<RockEngine::Mesh>& mesh)
			: Mesh(mesh) {}

		operator Ref<RockEngine::Mesh>() { return Mesh; }

		DrawTarget Target = DrawTarget::None;
	};

	struct SkyLightComponent
	{
		std::string Name;
		std::string FilePath;

		Environment SceneEnvironment;
		float Intensity = 1.0f;
		float Angle = 0.0f;
	};

	struct DirectionalLightComponent
	{
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float LightSize = 0.5f; // For PCSS

		bool CastShadows = true;
		bool SoftShadows = true;

	};

	struct PointLightComponent
	{
		glm::vec3 Radiance = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float LightSize = 0.5f; // For PCSS
		float MinRadius = 1.f;
		float Radius = 10.0f;
		float Falloff = 1.0f;
	};

	struct SphereColliderComponent // TODO: Set a virtual class, in Scnene Hierarchy we can change Colider
	{
		float Radius = 0.5F;
		bool IsTrigger = false;

		// The mesh that will be drawn in the editor to show the collision bounds
		Ref<Mesh> DebugMesh;

		SphereColliderComponent() = default;
		SphereColliderComponent(const SphereColliderComponent& other) = default;
	};
}