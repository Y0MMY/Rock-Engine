#include "pch.h"
#include "Material.h"

namespace RockEngine
{
	Material::Material(const Ref<Shader>& shader)
		: m_Shader(shader)
	{
		m_Shader->AddShaderReloadedCallback([this]()
			{
				this->OnShaderReloaded();
			});
		AllocateStorage();
	}

	void Material::Bind()
	{
		m_Shader->Bind();

		if (m_VSUniformStorageBuffer)
			m_Shader->SetVSMaterialUniformBuffer(m_VSUniformStorageBuffer);

		if (m_PSUniformStorageBuffer)
			m_Shader->SetVSMaterialUniformBuffer(m_PSUniformStorageBuffer);

		BindTextures();
	}

	void Material::BindTextures()
	{
		for (size_t i = 0; i < m_Textures.size(); i++)
		{
			auto& texture = m_Textures[i];
			if (texture)
				texture->Bind(i);
		}
	}

	ShaderUniformDecl* Material::FindUniformDeclaration(const std::string& name)
	{
		if (m_VSUniformStorageBuffer)
		{
			auto& decl = m_Shader->GetVSMaterialUniformBuffer().GetUniformDeclarations();
			for (auto* u : decl)
			{
				if (u->GetName() == name)
					return u;
			}
		}

		if (m_PSUniformStorageBuffer)
		{
			auto& decl = m_Shader->GetPSMaterialUniformBuffer().GetUniformDeclarations();
			for (auto* u : decl)
			{
				if (u->GetName() == name)
					return u;
			}
		}
		return nullptr;
	}

	Buffer& Material::GetUniformBufferTarget(ShaderUniformDecl* uniformDeclaration)
	{
		switch (uniformDeclaration->GetDomain())
		{
			case ShaderDomain::Vertex:    return m_VSUniformStorageBuffer;
			case ShaderDomain::Pixel:     return m_PSUniformStorageBuffer;
		}

		RE_CORE_ASSERT(false, "Invalid uniform declaration domain! Material does not support this shader type.");
		return m_VSUniformStorageBuffer;
	}

	ShaderResourceDecl* Material::FindResourceDeclaration(const std::string& name)
	{
		auto& resources = m_Shader->GetResources();
		for (ShaderResourceDecl* resource : resources)
		{
			if (resource->GetName() == name)
				return resource;
		}
		return nullptr;
	}

	void Material::OnShaderReloaded() const
	{
		
	}

	void Material::AllocateStorage()
	{
		if(m_Shader->HasVSMaterialUniformBuffer())
		{
			const auto& vsBuffer = m_Shader->GetVSMaterialUniformBuffer();
			m_VSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
			m_VSUniformStorageBuffer.ZeroInitialize();
		}
		
		if(m_Shader->HasPSMaterialUniformBuffer())
		{
			const auto& psBuffer = m_Shader->GetPSMaterialUniformBuffer();
			m_PSUniformStorageBuffer.Allocate(psBuffer.GetSize());
			m_PSUniformStorageBuffer.ZeroInitialize();
		}
	}

	Ref<Material> Material::Create(const Ref<Shader>& shader)
	{
		return Ref<Material>::Create(shader);
	}

	//////////////////////////////////////////////////////////////////////////////////
	// MaterialInstance
	//////////////////////////////////////////////////////////////////////////////////

	Ref<MaterialInstance> MaterialInstance::Create(const Ref<Material>& material)
	{
		return Ref<MaterialInstance>::Create(material);
	}


	MaterialInstance::MaterialInstance(const Ref<Material>& material, const std::string& name)
		: m_Material(material), m_Name(name)
	{
		m_Material->m_MaterialInstances.insert(this);
		AllocateStorage();
	}

	MaterialInstance::~MaterialInstance()
	{
		m_Material->m_MaterialInstances.erase(this);
	}

	void MaterialInstance::OnShaderReloaded()
	{
		AllocateStorage();
		m_OverriddenValues.clear();
	}

	void MaterialInstance::AllocateStorage()
	{
		if (m_Material->m_Shader->HasVSMaterialUniformBuffer())
		{
			const auto& vsBuffer = m_Material->m_Shader->GetVSMaterialUniformBuffer();
			m_VSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
			memcpy(m_VSUniformStorageBuffer.Data, m_Material->m_VSUniformStorageBuffer.Data, vsBuffer.GetSize());
		}

		if (m_Material->m_Shader->HasPSMaterialUniformBuffer())
		{
			const auto& psBuffer = m_Material->m_Shader->GetPSMaterialUniformBuffer();
			m_PSUniformStorageBuffer.Allocate(psBuffer.GetSize());
			memcpy(m_PSUniformStorageBuffer.Data, m_Material->m_PSUniformStorageBuffer.Data, psBuffer.GetSize());
		}
	}

	void MaterialInstance::Bind()
	{
		m_Material->m_Shader->Bind();

		if (m_VSUniformStorageBuffer)
			m_Material->m_Shader->SetVSMaterialUniformBuffer(m_VSUniformStorageBuffer);

		if (m_PSUniformStorageBuffer)
			m_Material->m_Shader->SetPSMaterialUniformBuffer(m_PSUniformStorageBuffer);

		m_Material->BindTextures();
		for (size_t i = 0; i < m_Textures.size(); i++)
		{
			auto& texture = m_Textures[i];
			if (texture)
				texture->Bind(i);
		}
	}

	Buffer& MaterialInstance::GetUniformBufferTarget(ShaderUniformDecl* uniformDeclaration)
	{
		switch (uniformDeclaration->GetDomain())
		{
		case ShaderDomain::Vertex:    return m_VSUniformStorageBuffer;
		case ShaderDomain::Pixel:     return m_PSUniformStorageBuffer;
		}

		RE_CORE_ASSERT(false, "Invalid uniform declaration domain! Material does not support this shader type.");
		return m_VSUniformStorageBuffer;
	}

	void MaterialInstance::OnMaterialValueUpdated(ShaderUniformDecl* decl)
	{
		if (m_OverriddenValues.find(decl->GetName()) == m_OverriddenValues.end())
		{
			auto& buffer = GetUniformBufferTarget(decl);
			auto& materialBuffer = m_Material->GetUniformBufferTarget(decl);
			buffer.Write(materialBuffer.Data + decl->GetOffset(), decl->GetSize(), decl->GetOffset());
		}
	}
}
