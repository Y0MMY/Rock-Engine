#pragma once

#include "RockEngine/Renderer/ShaderUniform.h"

namespace RockEngine
{
	class OpenGLShaderUniformDecl : public ShaderUniformDecl
	{
	public:
		enum class Type
		{
			NONE, FLOAT32, VEC2, VEC3, VEC4, MAT3, MAT4, INT32, BOOL, STRUCT
		};
	public:
		OpenGLShaderUniformDecl(ShaderDomain domain, Type type, const std::string& name, uint32_t count = 1);
		OpenGLShaderUniformDecl(ShaderDomain domain, ShaderStruct* uniformStruct, const std::string& name, uint32_t count = 1);

		virtual uint32_t GetSize() const { return m_Size; }
		virtual uint32_t GetOffset() const { return m_Offset; }
		virtual uint32_t GetCount() const { return m_Count; }
		virtual const std::string& GetName() const { return m_Name; }
		virtual ShaderDomain GetDomain() { return m_Domain; }

		inline const ShaderStruct& GetShaderUniformStruct() const { RE_CORE_ASSERT(m_Struct, ""); return *m_Struct; }

		inline int32_t GetLocation() const { return m_Location; }
		inline Type GetType() const { return m_Type; }
		inline bool IsArray() const { return m_Count > 1; }
	protected:
		virtual void SetOffset(uint32_t offset) override;
	public:
		static uint32_t SizeOfUniformType(Type type);
		static Type StringToType(const std::string& type);
		static std::string TypeToString(Type type);
	private:
		friend class Shader;
		friend class OpenGLShader;
		friend class ShaderStruct;
		friend class OpenGLShaderUniformBufferDecl;
	private:
		std::string m_Name;
		uint32_t m_Size;
		uint32_t m_Count;
		uint32_t m_Offset;
		ShaderDomain m_Domain;

		Type m_Type;
		ShaderStruct* m_Struct;

		mutable int32_t m_Location;
	};

	struct GLShaderUniformField
	{
		OpenGLShaderUniformDecl::Type type;
		std::string name;
		uint32_t count;
		mutable uint32_t size;
		mutable int32_t location;
	};

	class OpenGLShaderUniformBufferDecl : public ShaderUniformBufferDecl
	{
	private:
		friend class Shader;
	private:
		std::string m_Name;
		ShaderUniformList m_Uniforms;
		uint32_t m_Register;
		uint32_t m_Size;
		ShaderDomain m_Domain;
	public:
		OpenGLShaderUniformBufferDecl(const std::string& name, ShaderDomain domain);

		void PushUniform(OpenGLShaderUniformDecl* uniform);

		inline const std::string& GetName() const override { return m_Name; }
		inline uint32_t GetRegister() const override { return m_Register; }
		inline uint32_t GetSize() const override { return m_Size; }
		virtual ShaderDomain GetDomain() const { return m_Domain; }
		inline const ShaderUniformList& GetUniformDeclarations() const override { return m_Uniforms; }

		ShaderUniformDecl* FindUniform(const std::string& name);
	};

	class OpenGLShaderResourceDecl : public ShaderResourceDecl
	{
	public:
		enum class Type
		{
			NONE, TEXTURE2D, TEXTURECUBE
		};
	public:
		OpenGLShaderResourceDecl(Type type, const std::string& name, uint32_t count);

		inline virtual const std::string& GetName() const override  { return m_Name; }
		inline virtual uint32_t GetRegister() const override { return m_Register; }
		inline virtual uint32_t GetCount() const override { return m_Count; }
		
		inline Type GetType() const { return m_Type; }
	public:
		static Type StringToType(const std::string& type);
		static std::string TypeToString(Type type);
	private:
		std::string m_Name;
		uint32_t m_Register;
		uint32_t m_Count;
		Type m_Type;
	private:
		friend class OpenGLShader;
	};
}