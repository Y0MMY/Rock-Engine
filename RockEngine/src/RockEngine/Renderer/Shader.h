#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RockEngine/Renderer/RendererAPI.h"
#include "ShaderUniform.h"

#include <RockEngine/Core/Buffer.h>

namespace RockEngine
{
	enum class UniformType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Matrix3x3, Matrix4x4,
		Int32, Uint32
	};

	struct UniformDecl
	{
		UniformType type;
		std::string Name;
		u32 Offset;
	};

	struct UniformBuffer
	{
		std::byte* Buffer;
		std::vector< UniformDecl >Uniforms;
	};

	struct UniformBufferBase
	{
		virtual const byte* GetBuffer() const = 0;
		virtual UniformDecl* GetUniforms() const = 0;
		virtual unsigned int GetUniformCount() const = 0;
	};

	class Shader : public RefCounted
	{
	public:
		virtual ~Shader() {}

	public:
		using ShaderReloadedCallback = std::function<void()>;

		virtual void Reload() = 0;

		virtual void Bind() const = 0;
		virtual RendererID GetRendererID() const = 0;
		virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) = 0;

		virtual const std::string& GetName() const = 0;
		virtual const std::string& GetPath() const = 0;

		// Temporary while we don't have materials
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetBool(const std::string& name, bool value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind = true) = 0;

		virtual const ShaderUniformBufferList& GetVSRendererUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetPSRendererUniforms() const = 0;
		virtual bool HasVSMaterialUniformBuffer() const = 0;
		virtual bool HasPSMaterialUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDecl& GetVSMaterialUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDecl& GetPSMaterialUniformBuffer() const = 0;

		virtual void SetVSMaterialUniformBuffer(Buffer buffer) = 0;
		virtual void SetPSMaterialUniformBuffer(Buffer buffer) = 0;

		virtual const ShaderResourceList& GetResources() const = 0;

		virtual void SetIntArray(const std::string& name, int* values, uint32_t size) = 0;

		static Ref<Shader> Create(const std::string& path);

		virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) = 0;

		// Temporary, before we have an asset manager
		static std::vector<Ref<Shader>> s_AllShaders;
	};

	class ShaderLibrary : public RefCounted
	{
	public:
		ShaderLibrary()
		{

		}

		~ShaderLibrary()
		{

		}

		void Add(const Ref<Shader>& shader);
		void Load(const std::string& name, const std::string& filepath);
		void Load(const std::string& filepath);

		const Ref<Shader>& Get(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}