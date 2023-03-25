#pragma once

#include <RockEngine/Renderer/Renderer.h>
#include <RockEngine/Renderer/Shader.h>
#include "Texture.h"

#include <RockEngine/Core/Buffer.h>

#include <unordered_set>

namespace RockEngine
{
	class Material : public RefCounted
	{
		friend class MaterialInstance;
	public:
		Material(const Ref<Shader>& shader);

		Ref<Shader> GetShader() { return m_Shader; }

		void Bind();

		static Ref<Material> Create(const Ref<Shader>& shader);

		template <typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = FindUniformDeclaration(name);
			RE_CORE_ASSERT(decl, "Could not find uniform with name {}", name);
			auto& buffer = GetUniformBufferTarget(decl);
			buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

			for (auto mi : m_MaterialInstances)
				mi->OnMaterialValueUpdated(decl);
		}

		void Set(const std::string& name, const Ref<Texture>& texture)
		{
			auto decl = FindResourceDeclaration(name);
			uint32_t slot = decl->GetRegister();
			if (m_Textures.size() <= slot)
				m_Textures.resize((size_t)slot + 1);
			m_Textures[slot] = texture;
		}

		void Set(const std::string& name, const Ref<Texture2D>& texture)
		{
			Set(name, (const Ref<Texture>&)texture);
		}

		void Set(const std::string& name, const Ref<TextureCube>& texture)
		{
			Set(name, (const Ref<Texture>&)texture);
		}

		template<typename T>
		T& Get(const std::string& name)
		{
			auto decl = FindUniformDeclaration(name);
			RE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
			auto& buffer = GetUniformBufferTarget(decl);
			return buffer.Read<T>(decl->GetOffset());
		}

		template<typename T>
		Ref<T> GetResource(const std::string& name)
		{
			auto decl = FindResourceDeclaration(name);
			uint32_t slot = decl->GetRegister();
			RE_CORE_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
			return m_Textures[slot];
		}

		ShaderResourceDecl* FindResourceDeclaration(const std::string& name);
	private:
		void AllocateStorage();
		void OnShaderReloaded() const;
		void BindTextures();

		ShaderUniformDecl* FindUniformDeclaration(const std::string& name);
		Buffer& GetUniformBufferTarget(ShaderUniformDecl* uniformDeclaration);
	private:
		Buffer m_VSUniformStorageBuffer;
		Buffer m_PSUniformStorageBuffer;

		std::unordered_set<MaterialInstance*> m_MaterialInstances;

		std::vector<Ref<Texture>> m_Textures;
		Ref<Shader> m_Shader;
	};

	class MaterialInstance : public RefCounted
	{
		friend class Material;
	public:
		MaterialInstance(const Ref<Material>& material, const std::string& name = "");
		~MaterialInstance();

		template <typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = m_Material->FindUniformDeclaration(name);
			if (!decl)
				return;
			// RE_CORE_ASSERT(decl, "Could not find uniform with name '{0}'", name);
			RE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
			auto& buffer = GetUniformBufferTarget(decl);
			buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

			m_OverriddenValues.insert(name);
		}

		void Set(const std::string& name, const Ref<Texture>& texture)
		{
			auto decl = m_Material->FindResourceDeclaration(name);
			if (!decl)
			{
				RE_CORE_WARN("Cannot find material property: ", name);
				return;
			}
			uint32_t slot = decl->GetRegister();
			if (m_Textures.size() <= slot)
				m_Textures.resize((size_t)slot + 1);
			m_Textures[slot] = texture;
		}

		void Set(const std::string& name, const Ref<Texture2D>& texture)
		{
			Set(name, (const Ref<Texture>&)texture);
		}

		void Set(const std::string& name, const Ref<TextureCube>& texture)
		{
			Set(name, (const Ref<Texture>&)texture);
		}

		template<typename T>
		T& Get(const std::string& name)
		{
			auto decl = m_Material->FindUniformDeclaration(name);
			RE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
			auto& buffer = GetUniformBufferTarget(decl);
			return buffer.Read<T>(decl->GetOffset());
		}

		template<typename T>
		Ref<T> GetResource(const std::string& name)
		{
			auto decl = m_Material->FindResourceDeclaration(name);
			RE_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
			uint32_t slot = decl->GetRegister();
			RE_CORE_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
			return Ref<T>(m_Textures[slot]);
		}

		template<typename T>
		Ref<T> TryGetResource(const std::string& name)
		{
			auto decl = m_Material->FindResourceDeclaration(name);
			if (!decl)
				return nullptr;

			uint32_t slot = decl->GetRegister();
			if (slot >= m_Textures.size())
				return nullptr;

			return Ref<T>(m_Textures[slot]);
		}

		void Bind();

		Ref<Shader> GetShader() { return m_Material->m_Shader; }

		const std::string& GetName() const { return m_Name; }


		static Ref<MaterialInstance> Create(const Ref<Material>& material);
	private:
		void AllocateStorage();
		void OnShaderReloaded();
		Buffer& GetUniformBufferTarget(ShaderUniformDecl* uniformDeclaration);
		void OnMaterialValueUpdated(ShaderUniformDecl* decl);
	private:
		std::string m_Name;
		Ref<Material> m_Material;

		Buffer m_VSUniformStorageBuffer;
		Buffer m_PSUniformStorageBuffer;
		std::vector<Ref<Texture>> m_Textures;

		std::unordered_set<std::string> m_OverriddenValues;
	};
}