#pragma once

#include <string>
#include <vector>

namespace RockEngine
{
	enum class ShaderDomain
	{
		None = 0, Vertex = 0, Pixel = 1
	};

	class ShaderUniformDecl
	{
	public:
		virtual uint32_t GetSize() const = 0;
		virtual uint32_t GetOffset() const = 0;
		virtual uint32_t GetCount() const = 0;
		virtual const std::string& GetName() const = 0;
		virtual ShaderDomain GetDomain() = 0;
	protected:
		virtual void SetOffset(uint32_t offset) = 0;
	private:
		friend class Shader;
		friend class OpenGLShader;
		friend class ShaderStruct;
	};

	typedef std::vector<ShaderUniformDecl*> ShaderUniformList;

	class ShaderUniformBufferDecl : public RefCounted
	{
	public:
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetRegister() const = 0;
		virtual uint32_t GetSize() const = 0;
		virtual const ShaderUniformList& GetUniformDeclarations() const = 0;

		virtual ShaderUniformDecl* FindUniform(const std::string& name) = 0;
	};

	typedef std::vector<ShaderUniformBufferDecl*> ShaderUniformBufferList;

	class ShaderStruct
	{
	public:
		ShaderStruct(const std::string& name)
			:m_Name(name),m_Size(0),m_Offset(0)
		{}

		inline void SetOffset(uint32_t offset) { m_Offset = offset; }

		inline const std::string& GetName() const { return m_Name; }
		inline uint32_t GetOffset() const { return m_Offset; }
		inline uint32_t GetSize() const { return m_Size; }
		inline const ShaderUniformList& GetFields() const { return m_Fields; }

		void AddField(ShaderUniformDecl* field)
		{
			m_Size += field->GetSize();
			uint32_t offset = 0;
			if (m_Fields.size())
			{
				auto prev = m_Fields.back();
				offset = prev->GetSize() + prev->GetOffset();
			}
			field->SetOffset(offset);
			m_Fields.push_back(field);
		}
	private:
		ShaderUniformList m_Fields;
		std::string m_Name;
		uint32_t m_Size;
		uint32_t m_Offset;
	};

	typedef std::vector<ShaderStruct*> ShaderStructList;

	class ShaderResourceDecl
	{
	public:
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetRegister() const = 0;
		virtual uint32_t GetCount() const = 0;
	};

	typedef std::vector<ShaderResourceDecl*> ShaderResourceList;
}