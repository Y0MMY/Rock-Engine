#include "pch.h"
#include "OpenGLShaderUniform.h"

namespace RockEngine
{
	uint32_t OpenGLShaderUniformDecl::SizeOfUniformType(Type type)
	{
		switch (type)
		{
		case OpenGLShaderUniformDecl::Type::BOOL:       return 1;
		case OpenGLShaderUniformDecl::Type::INT32:      return 4;
		case OpenGLShaderUniformDecl::Type::FLOAT32:    return 4;
		case OpenGLShaderUniformDecl::Type::VEC2:       return 4 * 2;
		case OpenGLShaderUniformDecl::Type::VEC3:       return 4 * 3;
		case OpenGLShaderUniformDecl::Type::VEC4:       return 4 * 4;
		case OpenGLShaderUniformDecl::Type::MAT3:       return 4 * 3 * 3;
		case OpenGLShaderUniformDecl::Type::MAT4:       return 4 * 4 * 4;
		}
		return 0;
	}

	OpenGLShaderUniformDecl::Type OpenGLShaderUniformDecl::StringToType(const std::string& type)
	{
		if (type == "int")      return Type::INT32;
		if (type == "bool")     return Type::BOOL;
		if (type == "float")    return Type::FLOAT32;
		if (type == "vec2")     return Type::VEC2;
		if (type == "vec3")     return Type::VEC3;
		if (type == "vec4")     return Type::VEC4;
		if (type == "mat3")     return Type::MAT3;
		if (type == "mat4")     return Type::MAT4;

		return Type::NONE;
	}

	std::string OpenGLShaderUniformDecl::TypeToString(Type type)
	{
		switch (type)
		{
		case OpenGLShaderUniformDecl::Type::INT32:      return "int32";
		case OpenGLShaderUniformDecl::Type::BOOL:       return "bool";
		case OpenGLShaderUniformDecl::Type::FLOAT32:    return "float";
		case OpenGLShaderUniformDecl::Type::VEC2:       return "vec2";
		case OpenGLShaderUniformDecl::Type::VEC3:       return "vec3";
		case OpenGLShaderUniformDecl::Type::VEC4:       return "vec4";
		case OpenGLShaderUniformDecl::Type::MAT3:       return "mat3";
		case OpenGLShaderUniformDecl::Type::MAT4:       return "mat4";
		}
		return "Invalid Type";
	}

	OpenGLShaderUniformDecl::OpenGLShaderUniformDecl(ShaderDomain domain, Type type, const std::string& name, uint32_t count)
		: m_Name(name), m_Size(SizeOfUniformType(type) * count),m_Count(count),m_Offset(0),m_Domain(domain), m_Type(type), m_Struct(nullptr)
	{
	}

	OpenGLShaderUniformDecl::OpenGLShaderUniformDecl(ShaderDomain domain, ShaderStruct* uniformStruct, const std::string& name, uint32_t count)
		: m_Name(name), m_Size(uniformStruct->GetSize() * count), m_Count(count), m_Offset(0), m_Domain(domain), m_Type(OpenGLShaderUniformDecl::Type::STRUCT), m_Struct(uniformStruct)
	{
	}

	void OpenGLShaderUniformDecl::SetOffset(uint32_t offset)
	{
		if (m_Type == Type::STRUCT)
			m_Struct->SetOffset(offset);

		m_Offset = offset;
	}
	// -------------------------------

	OpenGLShaderUniformBufferDecl::OpenGLShaderUniformBufferDecl(const std::string& name, ShaderDomain domain)
		: m_Name(name), m_Domain(domain), m_Size(0), m_Register(0)
	{
	}

	void OpenGLShaderUniformBufferDecl::PushUniform(OpenGLShaderUniformDecl* uniform)
	{
		uint32_t offset = 0;
		if (m_Uniforms.size())
		{
			auto prev = m_Uniforms.back();
			offset = prev->GetOffset() + prev->GetSize();
		}
		uniform->SetOffset(offset);
		m_Size += uniform->GetSize();
		m_Uniforms.push_back(uniform);
	}

	ShaderUniformDecl* OpenGLShaderUniformBufferDecl::FindUniform(const std::string& name)
	{
		for (auto& it : m_Uniforms)
		{
			if (it->GetName() == name)
				return it;
		}
		return nullptr;
	}
	// -------------------------------

	OpenGLShaderResourceDecl::OpenGLShaderResourceDecl(Type type, const std::string& name, uint32_t count)
		: m_Type(type), m_Name(name), m_Count(count)
	{
		m_Name = name;
		m_Count = count;
	}

	OpenGLShaderResourceDecl::Type OpenGLShaderResourceDecl::StringToType(const std::string& type)
	{
		if (type == "sampler2D")    return Type::TEXTURE2D;
		if (type == "sampler2DMS")  return Type::TEXTURE2D;
		if (type == "samplerCube")  return Type::TEXTURECUBE;

		return Type::NONE;
	}

	std::string OpenGLShaderResourceDecl::TypeToString(Type type)
	{
		switch (type)
		{
		case Type::TEXTURE2D:	return "sampler2D";
		case Type::TEXTURECUBE:	return "samplerCube";
		}
		return "Invalid Type";
	}
}