#pragma once

#include <glad/glad.h>

#include "OpenGLShaderUniform.h"
#include "RockEngine/Renderer/Shader.h"

namespace RockEngine
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);

		virtual void Bind() const override;

		virtual RendererID GetRendererID() const override { return m_RendererID; }

		virtual const std::string& GetName() const override { return m_Name; }
		virtual const std::string& GetPath() const override { return m_AssetPath; }

		virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) override {}

		void UploadUniformInt(uint32_t location, int32_t value);
		void UploadUniformIntArray(uint32_t location, int32_t* values, int32_t count);
		void UploadUniformFloat(uint32_t location, float value);
		void UploadUniformFloat2(uint32_t location, const glm::vec2& value);
		void UploadUniformFloat3(uint32_t location, const glm::vec3& value);
		void UploadUniformFloat4(uint32_t location, const glm::vec4& value);
		void UploadUniformMat3(uint32_t location, const glm::mat3& values);
		void UploadUniformMat4(uint32_t location, const glm::mat4& values);
		void UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetBool(const std::string& name, bool value) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
		virtual void SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind = true) override;

		void UploadUniformStruct(OpenGLShaderUniformDecl* uniform, byte* buffer, uint32_t offset);

		void ResolveAndSetUniforms(const Ref<OpenGLShaderUniformBufferDecl>& decl, Buffer buffer);
		void ResolveAndSetUniform(OpenGLShaderUniformDecl* uniform, Buffer buffer);
		void ResolveAndSetUniformArray(OpenGLShaderUniformDecl* uniform, Buffer buffer);
		void ResolveAndSetUniformField(const OpenGLShaderUniformDecl& field, byte* data, int32_t offset);

		virtual const ShaderUniformBufferList& GetVSRendererUniforms() const override { return m_VSRendererUniformBuffers; }
		virtual const ShaderUniformBufferList& GetPSRendererUniforms() const override { return m_PSRendererUniformBuffers; }
		virtual const ShaderUniformBufferDecl& GetVSMaterialUniformBuffer() const override { return *m_VSMaterialUniformBuffer; }
		virtual const ShaderUniformBufferDecl& GetPSMaterialUniformBuffer() const override { return *m_PSMaterialUniformBuffer; }
		virtual bool HasVSMaterialUniformBuffer() const override { return (bool)m_VSMaterialUniformBuffer; }
		virtual bool HasPSMaterialUniformBuffer() const override { return (bool)m_PSMaterialUniformBuffer; }

		virtual void SetVSMaterialUniformBuffer(Buffer buffer) override;
		virtual void SetPSMaterialUniformBuffer(Buffer buffer) override;

		void UploadUniformMat4(const std::string& name, const glm::mat4& value);

		void UploadUniformInt(const std::string& name, int32_t value);
		void UploadUniformIntArray(const std::string& name, int32_t* values, uint32_t count);

		virtual const ShaderResourceList& GetResources() const override { return m_Resources; }

		virtual void SetIntArray(const std::string& name, int* values, uint32_t size) override;

		void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override
		{
			m_ShaderReloadedCallbacks.push_back(callback);
		}

	private:
		const std::string ReadShaderFromFile(const std::string& filepath);
		void Reload();
		void Load(const std::string& source);
		void Parse();
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void ParseUniformStruct(const std::string& block, ShaderDomain domain);
		void ParseUniform(const std::string& statement, ShaderDomain domain);
		ShaderStruct* FindStruct(const std::string& name);

		void ResolveUniforms();
		void ValidateUniforms();
		void CompileAndUploadShader();

		static GLenum ShaderTypeFromString(const std::string& type);

		int32_t GetUniformLocation(const std::string& name) const;
		//void UploadUniformFloat4(const std::string& name, const glm::vec4& values);
	private:
		RendererID m_RendererID = 0;
		std::string m_AssetPath;
		std::string m_Name;

		bool m_IsCompute = false;
		bool m_Loaded = false;

		ShaderResourceList m_Resources;
		ShaderStructList m_Structs;
		ShaderUniformBufferList m_VSRendererUniformBuffers;
		ShaderUniformBufferList m_PSRendererUniformBuffers;
		Ref<OpenGLShaderUniformBufferDecl> m_VSMaterialUniformBuffer;
		Ref<OpenGLShaderUniformBufferDecl> m_PSMaterialUniformBuffer;
		std::unordered_map<GLenum ,std::string> m_ShaderSource;

		std::vector<ShaderReloadedCallback> m_ShaderReloadedCallbacks;
	};
}