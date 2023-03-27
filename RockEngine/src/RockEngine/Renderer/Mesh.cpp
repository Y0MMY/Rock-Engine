#include "pch.h"
#include "Mesh.h"

#include <glad/glad.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <RockEngine/Renderer/Renderer.h>


namespace RockEngine
{
	namespace {
		const unsigned int ImportFlags =
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_PreTransformVertices |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_OptimizeMeshes |
			aiProcess_Debone |
			aiProcess_ValidateDataStructure;
	}

	struct LogStream : public Assimp::LogStream
	{
		static void Initialize()
		{
			if (Assimp::DefaultLogger::isNullLogger())
			{
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
			}
		}

		void write(const char* message) override
		{
			RE_CORE_ERROR("Assimp error: {0}", message);
		}

	};

	Mesh::Mesh(const std::string& filename)
		: m_FilePath(filename)
	{
		LogStream::Initialize();

		RE_CORE_INFO("Loading mesh: {0}", filename.c_str());

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filename, ImportFlags);
		if (!scene || !scene->HasMeshes())
			RE_CORE_ERROR("Failed to load mesh file: {0}", filename);

		aiMesh* mesh = scene->mMeshes[0];

		m_MeshShader = Renderer::GetShaderLibrary()->Get("shader.glsl");
		m_BaseMaterial = Material::Create(m_MeshShader);

		RE_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
		RE_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

		m_Vertices.reserve(mesh->mNumVertices);

		// Extract vertices from model
		for (size_t i = 0; i < m_Vertices.capacity(); i++)
		{
			Vertex vertex;
			vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}

			if (mesh->HasTextureCoords(0))
				vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			m_Vertices.push_back(vertex);
		}

		m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

		// Extract indices from model
		m_Indices.reserve(mesh->mNumFaces);
		for (size_t i = 0; i < m_Indices.capacity(); i++)
		{
			RE_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
			m_Indices.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
		}

		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * sizeof(Index));

		PipelineSpecification pipelineSpecification;
		pipelineSpecification.Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Binormal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
		};
		m_Pipeline = Pipeline::Create(pipelineSpecification);

		// Materials
		if (scene->HasMaterials())
		{
			m_Textures.resize(scene->mNumMaterials);
			m_Materials.resize(scene->mNumMaterials);
			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				auto aiMaterial = scene->mMaterials[i];
				auto aiMaterialName = aiMaterial->GetName();

				auto mi = MaterialInstance::Create(m_BaseMaterial);
				m_Materials[i] = mi;

				RE_CORE_INFO("Material Name = {0}; Index = {1}", aiMaterialName.data, i);
				aiString aiTexPath;
				uint32_t textureCount = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
				RE_CORE_TRACE("  TextureCount = {0}", textureCount);

				aiColor3D aiColor;
				aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
				RE_CORE_TRACE("COLOR = {0}, {1}, {2}", aiColor.r, aiColor.g, aiColor.b);

				if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS)
				{
					
					std::filesystem::path path = filename;
					auto parentPath = path.parent_path();
					parentPath /= std::string(aiTexPath.data);
					std::string texturePath = parentPath.string();

					auto texture = Texture2D::Create(texturePath, true);
					if (texture->Loaded())
					{
						m_Textures[i] = texture;
						RE_CORE_TRACE("  Texture Path = {0}", texturePath);
						mi->Set("u_AlbedoTexture", m_Textures[i]);
						mi->Set("u_AlbedoTexToggle", 1.0f);
					}
					else
					{
						RE_CORE_ERROR("Could not load texture: {0}", texturePath);
						mi->Set("u_AlbedoTexToggle", 0.0f);
						mi->Set("u_AlbedoColor", glm::vec3{ aiColor.r, aiColor.g, aiColor.b });
					}
				}
				else
				{
					mi->Set("u_AlbedoTexToggle", 0.0f);
					mi->Set("u_AlbedoColor", glm::vec3{ aiColor.r, aiColor.g, aiColor.b });
				}

				for (uint32_t i = 0; i < aiMaterial->mNumProperties; i++)
				{
					auto prop = aiMaterial->mProperties[i];
					RE_CORE_TRACE("Material Property:");
					RE_CORE_TRACE("  Name = {0}", prop->mKey.data);

					switch (prop->mSemantic)
					{
					case aiTextureType_NONE:
						RE_CORE_TRACE("  Semantic = aiTextureType_NONE");
						break;
					case aiTextureType_DIFFUSE:
						RE_CORE_TRACE("  Semantic = aiTextureType_DIFFUSE");
						break;
					case aiTextureType_SPECULAR:
						RE_CORE_TRACE("  Semantic = aiTextureType_SPECULAR");
						break;
					case aiTextureType_AMBIENT:
						RE_CORE_TRACE("  Semantic = aiTextureType_AMBIENT");
						break;
					case aiTextureType_EMISSIVE:
						RE_CORE_TRACE("  Semantic = aiTextureType_EMISSIVE");
						break;
					case aiTextureType_HEIGHT:
						RE_CORE_TRACE("  Semantic = aiTextureType_HEIGHT");
						break;
					case aiTextureType_NORMALS:
						RE_CORE_TRACE("  Semantic = aiTextureType_NORMALS");
						break;
					case aiTextureType_SHININESS:
						RE_CORE_TRACE("  Semantic = aiTextureType_SHININESS");
						break;
					case aiTextureType_OPACITY:
						RE_CORE_TRACE("  Semantic = aiTextureType_OPACITY");
						break;
					case aiTextureType_DISPLACEMENT:
						RE_CORE_TRACE("  Semantic = aiTextureType_DISPLACEMENT");
						break;
					case aiTextureType_LIGHTMAP:
						RE_CORE_TRACE("  Semantic = aiTextureType_LIGHTMAP");
						break;
					case aiTextureType_REFLECTION:
						RE_CORE_TRACE("  Semantic = aiTextureType_REFLECTION");
						break;
					case aiTextureType_UNKNOWN:
						RE_CORE_TRACE("  Semantic = aiTextureType_UNKNOWN");
						break;
					}

					if (prop->mType == aiPTI_String)
					{
						uint32_t strLength = *(uint32_t*)prop->mData;
						std::string str(prop->mData + 4, strLength);
						RE_CORE_TRACE("  Value = {0}", str);

						std::string key = prop->mKey.data;
						if (key == "$raw.ReflectionFactor|file")
						{
							// TODO: Temp - this should be handled by Hazel's filesystem
							std::filesystem::path path = filename;
							auto parentPath = path.parent_path();
							parentPath /= str;
							std::string texturePath = parentPath.string();

							auto texture = Texture2D::Create(texturePath);
							if (texture->Loaded())
							{
								RE_CORE_TRACE("  Metalness map path = {0}", texturePath);
								mi->Set("u_MetalnessTexture", texture);
								mi->Set("u_MetalnessTexToggle", 1.0f);
							}
							else
							{
								RE_CORE_ERROR("Could not load texture: {0}", texturePath);
								mi->Set("u_Metalness", 0.5f);
								mi->Set("u_MetalnessTexToggle", 1.0f);
							}
						}
					}
				}


				// Normal maps
				if (aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS)
				{
					// TODO: Temp - this should be handled by Hazel's filesystem
					std::filesystem::path path = filename;
					auto parentPath = path.parent_path();
					parentPath /= std::string(aiTexPath.data);
					std::string texturePath = parentPath.string();

					auto texture = Texture2D::Create(texturePath);
					if (texture->Loaded())
					{
						RE_CORE_TRACE("  Normal map path = {0}", texturePath);
						mi->Set("u_NormalTexture", texture);
						mi->Set("u_NormalTexToggle", 1.0f);
					}
					else
					{
						RE_CORE_ERROR("Could not load texture: {0}", texturePath);
						//mi->Set("u_AlbedoTexToggle", 0.0f);
						// mi->Set("u_AlbedoColor", glm::vec3{ color.r, color.g, color.b });
					}
				}

				// Roughness map
				if (aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS)
				{
					// TODO: Temp - this should be handled by Hazel's filesystem
					std::filesystem::path path = filename;
					auto parentPath = path.parent_path();
					parentPath /= std::string(aiTexPath.data);
					std::string texturePath = parentPath.string();

					auto texture = Texture2D::Create(texturePath);
					if (texture->Loaded())
					{
						RE_CORE_TRACE("  Roughness map path = {0}", texturePath);
						mi->Set("u_RoughnessTexture", texture);
						mi->Set("u_RoughnessTexToggle", 1.0f);
					}
					else
					{
						RE_CORE_ERROR("Could not load texture: {0}", texturePath);
						mi->Set("u_RoughnessTexToggle", 1.0f);
						mi->Set("u_Roughness", 0.5f);
					}
				}

				// Metalness map
				if (aiMaterial->Get("$raw.ReflectionFactor|file", aiPTI_String, 0, aiTexPath) == AI_SUCCESS)
				{
					// TODO: Temp - this should be handled by Hazel's filesystem
					std::filesystem::path path = filename;
					auto parentPath = path.parent_path();
					parentPath /= std::string(aiTexPath.data);
					std::string texturePath = parentPath.string();

					auto texture = Texture2D::Create(texturePath);
					if (texture->Loaded())
					{
						RE_CORE_TRACE("  Metalness map path = {0}", texturePath);
						mi->Set("u_MetalnessTexture", texture);
						mi->Set("u_MetalnessTexToggle", 1.0f);
					}
					else
					{
						RE_CORE_ERROR("Could not load texture: {0}", texturePath);
						mi->Set("u_Metalness", 0.5f);
						mi->Set("u_MetalnessTexToggle", 1.0f);
					}
				}
			}
		}
	}


	void Mesh::Render()
	{
		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();
		m_Pipeline->Bind();
		Renderer::DrawIndexed(m_IndexBuffer->GetCount(),true);
	}

}