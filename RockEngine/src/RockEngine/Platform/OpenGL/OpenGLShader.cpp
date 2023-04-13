#include "pch.h"
#include "OpenGLShader.h"

#include "RockEngine/Renderer/Renderer.h"

#include "RockEngine/Utilities/StringUtils.h"

namespace RockEngine
{
#define UNIFORM_LOGGING 1
#if UNIFORM_LOGGING
#define RE_LOG_UNIFORM(...) RE_CORE_WARN(__VA_ARGS__)
#else
#define RE_LOG_UNIFORM
#endif

	const char* FindToken(const char* str, const std::string& token);
	std::vector<std::string> SplitString(const std::string& string, const char delimiter);
	std::vector<std::string> Tokenize(const std::string& string);
	std::vector<std::string> GetLines(const std::string& string);
	std::string GetBlock(const char* str, const char** outPosition);
	bool StartsWith(const std::string& string, const std::string& start);
	std::string GetStatement(const char* str, const char** outPosition);

	GLenum OpenGLShader::ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;
		if (type == "compute")
			return GL_COMPUTE_SHADER;

		return GL_NONE;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_AssetPath(filepath)
	{
		ReadShaderFromFile(filepath);
		m_Name = Utils::GetFilename(filepath);
		Reload();
	}

	void OpenGLShader::Reload() 
	{
		std::string source = ReadShaderFromFile(m_AssetPath);
		Load(source);
	}

	void OpenGLShader::Load(const std::string& source)
	{
		m_ShaderSource = PreProcess(source); 
		if (!m_IsCompute)
			Parse();

		Renderer::Submit([=]()
			{
				if (m_RendererID)
					glDeleteProgram(m_RendererID);

				CompileAndUploadShader();
				if (!m_IsCompute)
				{
					ResolveUniforms();
					ValidateUniforms();
				}
			});

		if (m_Loaded)
		{
			for (auto& e : m_ShaderReloadedCallbacks)
				e();
		}

		m_Loaded = true;
	}

	const std::string OpenGLShader::ReadShaderFromFile(const std::string& filepath)
	{
	
		return Utils::ReadFromFile(filepath.c_str());
	}
	void OpenGLShader::Bind() const {
		Renderer::Submit([=]()
			{
				glUseProgram(m_RendererID);
			});
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* TypeToken = "#type";
		auto typeTokenLength = strlen(TypeToken);

		size_t pos = source.find(TypeToken);

		while (pos != std::string::npos)
		{
			auto eol = source.find_first_of("\r\n", pos);
			RE_CORE_ASSERT(eol != std::string::npos, "Syntax error");

			auto begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			RE_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel" || type == "compute", "Invalid shader type specified");
			
			auto nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(TypeToken, nextLinePos);
			auto shaderType = ShaderTypeFromString(type);
			shaderSources[shaderType] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

			// Compute shaders cannot contain other types
			if (shaderType == GL_COMPUTE_SHADER)
			{
				m_IsCompute = true;
				break;
			}
		}

		return shaderSources;
	}

	void OpenGLShader::Parse()
	{
		const char* token;
		const char* vstr;
		const char* fstr;

		m_Resources.clear();
		m_Structs.clear();
		m_VSMaterialUniformBuffer.Reset();
		m_PSMaterialUniformBuffer.Reset();

		auto& vertexSource = m_ShaderSource[GL_VERTEX_SHADER];
		auto& fragmentSource = m_ShaderSource[GL_FRAGMENT_SHADER];

		// Vertex Shader
		vstr = vertexSource.c_str();
		while (token = FindToken(vstr, "struct"))
			ParseUniformStruct(GetBlock(token, &vstr), ShaderDomain::Vertex);

		vstr = vertexSource.c_str();
		while (token = FindToken(vstr, "uniform"))
			ParseUniform(GetStatement(token, &vstr), ShaderDomain::Vertex);

		// Fragment Shader
		fstr = fragmentSource.c_str();
		while (token = FindToken(fstr, "struct"))
			ParseUniformStruct(GetBlock(token, &fstr), ShaderDomain::Pixel);

		fstr = fragmentSource.c_str();
		while (token = FindToken(fstr, "uniform"))
			ParseUniform(GetStatement(token, &fstr), ShaderDomain::Pixel);
	}

	static bool IsTypeStringResource(const std::string& type)
	{
		if (type == "sampler1D")		return true;
		if (type == "sampler2D")		return true;
		if (type == "sampler2DMS")		return true;
		if (type == "samplerCube")		return true;
		if (type == "sampler2DShadow")	return true;
		return false;
	}

	ShaderStruct* OpenGLShader::FindStruct(const std::string& name)
	{
		for (ShaderStruct* s : m_Structs)
		{
			if (s->GetName() == name)
				return s;
		}
		return nullptr;
	}

	void OpenGLShader::ParseUniform(const std::string& statement, ShaderDomain domain)
	{
		std::vector<std::string> tokens = Tokenize(statement);
		uint32_t index = 0;

		index++; // "uniform"
		std::string typeString = tokens[index++];
		std::string name = tokens[index++];
		// Strip ; from name if present
		if (const char* s = strstr(name.c_str(), ";"))
			name = std::string(name.c_str(), s - name.c_str());

		std::string n(name);
		int32_t count = 1;
		const char* namestr = n.c_str();
		if (const char* s = strstr(namestr, "["))
		{
			name = std::string(namestr, s - namestr);

			const char* end = strstr(namestr, "]");
			std::string c(s + 1, end - s);
			count = atoi(c.c_str());
		}

		if (IsTypeStringResource(typeString))
		{
			ShaderResourceDecl* Decl = new OpenGLShaderResourceDecl(OpenGLShaderResourceDecl::StringToType(typeString), name, count);
			m_Resources.push_back(Decl);
		}
		else
		{
			OpenGLShaderUniformDecl::Type t = OpenGLShaderUniformDecl::StringToType(typeString);
			OpenGLShaderUniformDecl* Decl = nullptr;

			if (t == OpenGLShaderUniformDecl::Type::NONE)
			{
				// Find struct
				ShaderStruct* s = FindStruct(typeString);
				RE_CORE_ASSERT(s, "");
				Decl = new OpenGLShaderUniformDecl(domain, s, name, count);
			}
			else
			{
				Decl = new OpenGLShaderUniformDecl(domain, t, name, count);
			}

			if (StartsWith(name, "r_"))
			{
				if (domain == ShaderDomain::Vertex)
					((OpenGLShaderUniformBufferDecl*)m_VSRendererUniformBuffers.front())->PushUniform(Decl);
				else if (domain == ShaderDomain::Pixel)
					((OpenGLShaderUniformBufferDecl*)m_PSRendererUniformBuffers.front())->PushUniform(Decl);
			}
			else
			{
				if (domain == ShaderDomain::Vertex)
				{
					if (!m_VSMaterialUniformBuffer)
						m_VSMaterialUniformBuffer.Reset(new OpenGLShaderUniformBufferDecl("", domain));

					m_VSMaterialUniformBuffer->PushUniform(Decl);
				}
				else if (domain == ShaderDomain::Pixel)
				{
					if (!m_PSMaterialUniformBuffer)
						m_PSMaterialUniformBuffer.Reset(new OpenGLShaderUniformBufferDecl("", domain));

					m_PSMaterialUniformBuffer->PushUniform(Decl);
				}
			}
		}
	}

	void OpenGLShader::ParseUniformStruct(const std::string& block, ShaderDomain domain)
	{
		std::vector<std::string> tokens = Tokenize(block);

		uint32_t index = 0;
		index++; // struct
		std::string name = tokens[index++];
		ShaderStruct* uniformStruct = new ShaderStruct(name);
		index++; // {
		while (index < tokens.size())
		{
			if (tokens[index] == "}")
				break;

			std::string type = tokens[index++];
			std::string name = tokens[index++];

			// Strip ; from name if present
			if (const char* s = strstr(name.c_str(), ";"))
				name = std::string(name.c_str(), s - name.c_str());

			uint32_t count = 1;
			const char* namestr = name.c_str();
			if (const char* s = strstr(namestr, "["))
			{
				name = std::string(namestr, s - namestr);

				const char* end = strstr(namestr, "]");
				std::string c(s + 1, end - s);
				count = atoi(c.c_str());
			}
			ShaderUniformDecl* field = new OpenGLShaderUniformDecl(domain, OpenGLShaderUniformDecl::StringToType(type), name, count);

			uniformStruct->AddField(field);
		}
		m_Structs.push_back(uniformStruct);
	}

	void OpenGLShader::CompileAndUploadShader()
	{
		std::vector<GLuint> shaderRendererIDs;

		GLuint program = glCreateProgram();
		for (auto& kv : m_ShaderSource)
		{
			GLenum type = kv.first;
			std::string& source = kv.second;

			GLuint shaderRendererID = glCreateShader(type);
			const GLchar* sourceCstr = (const GLchar*)source.c_str();
			glShaderSource(shaderRendererID, 1, &sourceCstr, 0);

			glCompileShader(shaderRendererID);

			GLint isCompiled = 0;
			glGetShaderiv(shaderRendererID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shaderRendererID, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shaderRendererID, maxLength, &maxLength, &infoLog[0]);

				RE_CORE_ERROR("Shader compilation failed ({0}):\n{1}", m_AssetPath, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shaderRendererID);

				RE_CORE_ASSERT(false, "Failed");
			}

			shaderRendererIDs.push_back(shaderRendererID);
			glAttachShader(program, shaderRendererID);
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			RE_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_AssetPath, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : shaderRendererIDs)
				glDeleteShader(id);
		}

		// Always detach shaders after a successful link.
		for (auto id : shaderRendererIDs)
			glDetachShader(program, id);

		m_RendererID = program;
	}

	void OpenGLShader::ResolveUniforms()
	{
		glUseProgram(m_RendererID);

		for (size_t i = 0; i < m_VSRendererUniformBuffers.size(); i++)
		{
			OpenGLShaderUniformBufferDecl* decl = (OpenGLShaderUniformBufferDecl*)m_VSRendererUniformBuffers[i];
			const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
			for (size_t j = 0; j < uniforms.size(); j++)
			{
				OpenGLShaderUniformDecl* uniform = (OpenGLShaderUniformDecl*)uniforms[j];
				if (uniform->GetType() == OpenGLShaderUniformDecl::Type::STRUCT)
				{
					const ShaderStruct& s = uniform->GetShaderUniformStruct();
					const auto& fields = s.GetFields();
					for (size_t k = 0; k < fields.size(); k++)
					{
						OpenGLShaderUniformDecl* field = (OpenGLShaderUniformDecl*)fields[k];
						field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
					}
				}
				else
				{
					uniform->m_Location = GetUniformLocation(uniform->m_Name);
				}
			}
		}

		for (size_t i = 0; i < m_PSRendererUniformBuffers.size(); i++)
		{
			OpenGLShaderUniformBufferDecl* decl = (OpenGLShaderUniformBufferDecl*)m_PSRendererUniformBuffers[i];
			const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
			for (size_t j = 0; j < uniforms.size(); j++)
			{
				OpenGLShaderUniformDecl* uniform = (OpenGLShaderUniformDecl*)uniforms[j];
				if (uniform->GetType() == OpenGLShaderUniformDecl::Type::STRUCT)
				{
					const ShaderStruct& s = uniform->GetShaderUniformStruct();
					const auto& fields = s.GetFields();
					for (size_t k = 0; k < fields.size(); k++)
					{
						OpenGLShaderUniformDecl* field = (OpenGLShaderUniformDecl*)fields[k];
						field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
					}
				}
				else
				{
					uniform->m_Location = GetUniformLocation(uniform->m_Name);
				}
			}
		}

		{
			const auto& decl = m_VSMaterialUniformBuffer;
			if (decl)
			{
				const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
				for (size_t j = 0; j < uniforms.size(); j++)
				{
					OpenGLShaderUniformDecl* uniform = (OpenGLShaderUniformDecl*)uniforms[j];
					if (uniform->GetType() == OpenGLShaderUniformDecl::Type::STRUCT)
					{
						const ShaderStruct& s = uniform->GetShaderUniformStruct();
						const auto& fields = s.GetFields();
						for (size_t k = 0; k < fields.size(); k++)
						{
							OpenGLShaderUniformDecl* field = (OpenGLShaderUniformDecl*)fields[k];
							field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
						}
					}
					else
					{
						uniform->m_Location = GetUniformLocation(uniform->m_Name);
					}
				}
			}
		}

		{
			const auto& decl = m_PSMaterialUniformBuffer;
			if (decl)
			{
				const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
				for (size_t j = 0; j < uniforms.size(); j++)
				{
					OpenGLShaderUniformDecl* uniform = (OpenGLShaderUniformDecl*)uniforms[j];
					if (uniform->GetType() == OpenGLShaderUniformDecl::Type::STRUCT)
					{
						const ShaderStruct& s = uniform->GetShaderUniformStruct();
						const auto& fields = s.GetFields();
						for (size_t k = 0; k < fields.size(); k++)
						{
							OpenGLShaderUniformDecl* field = (OpenGLShaderUniformDecl*)fields[k];
							field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
						}
					}
					else
					{
						uniform->m_Location = GetUniformLocation(uniform->m_Name);
					}
				}
			}
		}

		uint32_t sampler = 0;
		for (size_t i = 0; i < m_Resources.size(); i++)
		{
			OpenGLShaderResourceDecl* resource = (OpenGLShaderResourceDecl*)m_Resources[i];
			int32_t location = GetUniformLocation(resource->m_Name);

			if (resource->GetCount() == 1)
			{
				resource->m_Register = sampler;
				if (location != -1)
					UploadUniformInt(location, sampler);

				sampler++;
			}
			else if (resource->GetCount() > 1)
			{
				resource->m_Register = sampler;
				uint32_t count = resource->GetCount();
				int* samplers = new int[count];
				for (uint32_t s = 0; s < count; s++)
					samplers[s] = sampler++;
				UploadUniformIntArray(resource->GetName(), samplers, count);
				delete[] samplers;
			}
		}
	}

	void OpenGLShader::ValidateUniforms()
	{

	}

	// Parsing helper functions
	const char* FindToken(const char* str, const std::string& token)
	{
		const char* t = str;
		while (t = strstr(t, token.c_str()))
		{
			bool left = str == t || isspace(t[-1]);
			bool right = !t[token.size()] || isspace(t[token.size()]);
			if (left && right)
				return t;

			t += token.size();
		}
		return nullptr;
	}

	const char* FindToken(const std::string& string, const std::string& token)
	{
		return FindToken(string.c_str(), token);
	}

	std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
	{
		size_t start = 0;
		size_t end = string.find_first_of(delimiters);

		std::vector<std::string> result;

		while (end <= std::string::npos)
		{
			std::string token = string.substr(start, end - start);
			if (!token.empty())
				result.push_back(token);

			if (end == std::string::npos)
				break;

			start = end + 1;
			end = string.find_first_of(delimiters, start);
		}

		return result;
	}

	std::vector<std::string> SplitString(const std::string& string, const char delimiter)
	{
		return SplitString(string, std::string(1, delimiter));
	}

	std::vector<std::string> Tokenize(const std::string& string)
	{
		return SplitString(string, " \t\n\r");
	}

	std::vector<std::string> GetLines(const std::string& string)
	{
		return SplitString(string, "\n");
	}

	std::string GetBlock(const char* str, const char** outPosition)
	{
		const char* end = strstr(str, "}");
		if (!end)
			return str;

		if (outPosition)
			*outPosition = end;
		auto length = end - str + 1;
		return std::string(str, length);
	}

	bool StartsWith(const std::string& string, const std::string& start)
	{
		return string.find(start) == 0;
	}

	std::string GetStatement(const char* str, const char** outPosition)
	{
		const char* end = strstr(str, ";");
		if (!end)
			return str;

		if (outPosition)
			*outPosition = end;
		auto length = end - str + 1;
		return std::string(str, length);
	}

	void OpenGLShader::SetPSMaterialUniformBuffer(Buffer buffer)
	{
		Renderer::Submit([=]()
		{
			glUseProgram(m_RendererID);
			ResolveAndSetUniforms(m_PSMaterialUniformBuffer, buffer);
		});
	}
	
	void OpenGLShader::SetVSMaterialUniformBuffer(Buffer buffer)
	{
		Renderer::Submit([=]()
		{
			glUseProgram(m_RendererID);
			ResolveAndSetUniforms(m_VSMaterialUniformBuffer, buffer);
		});
	}

	void OpenGLShader::ResolveAndSetUniforms(const Ref<OpenGLShaderUniformBufferDecl>& decl, Buffer buffer)
	{
		const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
		for (int i = 0; i < uniforms.size(); i++)
		{
			OpenGLShaderUniformDecl* uniform = (OpenGLShaderUniformDecl*)uniforms[i];

			if (uniform->IsArray())
			{
				ResolveAndSetUniformArray(uniform, buffer);
			}

			else
				ResolveAndSetUniform(uniform, buffer);
		}
	}

	void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDecl* uniform, Buffer buffer)
	{
		//HZ_CORE_ASSERT(uniform->GetLocation() != -1, "Uniform has invalid location!");

		uint32_t offset = uniform->GetOffset();
		switch (uniform->GetType())
		{
		case OpenGLShaderUniformDecl::Type::BOOL:
			UploadUniformFloat(uniform->GetLocation(), *(bool*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::FLOAT32:
			UploadUniformFloat(uniform->GetLocation(), *(float*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::INT32:
			UploadUniformInt(uniform->GetLocation(), *(int32_t*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::VEC2:
			UploadUniformFloat2(uniform->GetLocation(), *(glm::vec2*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::VEC3:
			UploadUniformFloat3(uniform->GetLocation(), *(glm::vec3*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::VEC4:
			UploadUniformFloat4(uniform->GetLocation(), *(glm::vec4*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::MAT3:
			UploadUniformMat3(uniform->GetLocation(), *(glm::mat3*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::MAT4:
			UploadUniformMat4Array(uniform->GetLocation(), *(glm::mat4*)&buffer.Data[offset], uniform->GetCount());
			break;
		case OpenGLShaderUniformDecl::Type::STRUCT:
			UploadUniformStruct(uniform, buffer.Data, offset);
			break;
		default:
			RE_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}

	void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDecl* uniform, Buffer buffer)
	{
		if (uniform->GetLocation() == -1)
			return;

		RE_CORE_ASSERT(uniform->GetLocation() != -1, "Uniform has invalid location!");

		u32 offset = uniform->GetOffset();

		switch (uniform->GetType())
		{
		case OpenGLShaderUniformDecl::Type::BOOL:
			UploadUniformFloat(uniform->GetLocation(), *(bool*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::FLOAT32:
			UploadUniformFloat(uniform->GetLocation(), *(float*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::INT32:
			UploadUniformInt(uniform->GetLocation(), *(int32_t*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::VEC2:
			UploadUniformFloat2(uniform->GetLocation(), *(glm::vec2*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::VEC3:
			UploadUniformFloat3(uniform->GetLocation(), *(glm::vec3*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::VEC4:
			UploadUniformFloat4(uniform->GetLocation(), *(glm::vec4*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::MAT3:
			UploadUniformMat3(uniform->GetLocation(), *(glm::mat3*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::MAT4:
			UploadUniformMat4(uniform->GetLocation(), *(glm::mat4*)&buffer.Data[offset]);
			break;
			case OpenGLShaderUniformDecl::Type::STRUCT:
				UploadUniformStruct(uniform, buffer.Data, offset);
			break;
		default:
			RE_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}

	// Shaders functions
	int32_t OpenGLShader::GetUniformLocation(const std::string& name) const
	{
		int32_t result = glGetUniformLocation(m_RendererID, name.c_str());
		if (result == -1)
			RE_CORE_WARN("Could not find uniform '{0}' in shader", name);

		return result;
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		Renderer::Submit([=]() {
			UploadUniformFloat(name, value);
			});
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		Renderer::Submit([=]() {
			UploadUniformInt(name, value);
			});
	}

	void OpenGLShader::SetBool(const std::string& name, bool value)
	{
		Renderer::Submit([=]() {
			UploadUniformInt(name, value);
			});
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		Renderer::Submit([=]() {
			UploadUniformFloat2(name, value);
			});
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		Renderer::Submit([=]() {
			UploadUniformFloat3(name, value);
			});
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		Renderer::Submit([=]() {
			UploadUniformMat4(name, value);
			});
	}

	void OpenGLShader::SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind)
	{
		if (bind)
		{
			UploadUniformMat4(name, value);
		}
		else
		{
			int location = glGetUniformLocation(m_RendererID, name.c_str());
			if (location != -1)
				UploadUniformMat4(location, value);
		}
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t size)
	{
		Renderer::Submit([=]() {
			UploadUniformIntArray(name, values, size);
			});
	}

	void OpenGLShader::UploadUniformInt(uint32_t location, int32_t value)
	{
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(uint32_t location, int32_t* values, int32_t count)
	{
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(uint32_t location, float value)
	{
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(uint32_t location, const glm::vec2& value)
	{
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(uint32_t location, const glm::vec3& value)
	{
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(uint32_t location, const glm::vec4& value)
	{
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(uint32_t location, const glm::mat3& value)
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, &(value[0].x));
	}

	void OpenGLShader::UploadUniformMat4(uint32_t location, const glm::mat4& value)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, &(value[0].x));
	}

	void OpenGLShader::UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count)
	{
		glUniformMatrix4fv(location, count, GL_FALSE, &(values[0].x));
	}

	void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDecl& field, byte* data, int32_t offset)
	{
		switch (field.GetType())
		{
		case OpenGLShaderUniformDecl::Type::FLOAT32:
			UploadUniformFloat(field.GetLocation(), *(float*)&data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::INT32:
			UploadUniformInt(field.GetLocation(), *(int32_t*)&data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::VEC2:
			UploadUniformFloat2(field.GetLocation(), *(glm::vec2*)&data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::VEC3:
			UploadUniformFloat3(field.GetLocation(), *(glm::vec3*)&data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::VEC4:
			UploadUniformFloat4(field.GetLocation(), *(glm::vec4*)&data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::MAT3:
			UploadUniformMat3(field.GetLocation(), *(glm::mat3*)&data[offset]);
			break;
		case OpenGLShaderUniformDecl::Type::MAT4:
			UploadUniformMat4(field.GetLocation(), *(glm::mat4*)&data[offset]);
			break;
		default:
			RE_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}

	void OpenGLShader::UploadUniformStruct(OpenGLShaderUniformDecl* uniform, byte* buffer, uint32_t offset)
	{
		const ShaderStruct& s = uniform->GetShaderUniformStruct();
		const auto& fields = s.GetFields();
		for (size_t k = 0; k < fields.size(); k++)
		{
			OpenGLShaderUniformDecl* field = (OpenGLShaderUniformDecl*)fields[k];
			ResolveAndSetUniformField(*field, buffer, offset);
			offset += field->m_Size;
		}
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int32_t value)
	{
		int32_t location = GetUniformLocation(name);
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int32_t* values, uint32_t count)
	{
		int32_t location = GetUniformLocation(name);
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		glUseProgram(m_RendererID);
		auto location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1)
			glUniform1f(location, value);
		else
			RE_LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		glUseProgram(m_RendererID);
		auto location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1)
			glUniform2f(location, values.x, values.y);
		else
			RE_LOG_UNIFORM("Uniform '{0}' not found!", name);
	}


	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		glUseProgram(m_RendererID);
		auto location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1)
			glUniform3f(location, values.x, values.y, values.z);
		else
			RE_LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		glUseProgram(m_RendererID);
		auto location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1)
			glUniform4f(location, values.x, values.y, values.z, values.w);
		else
			RE_LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& values)
	{
		glUseProgram(m_RendererID);
		auto location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1)
			glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)&values);
		else
			RE_LOG_UNIFORM("Uniform '{0}' not found!", name);
	}
}