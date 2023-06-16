#pragma once

#include "RockEngine/Renderer/Texture.h"
#include "RockEngine/Utilities/FileSystem.h"

namespace RockEngine {

	class EditorResources
	{
	public:
		// Generic
		inline static Ref<Texture2D> PlusIcon = nullptr;

		static void Init()
		{
			// Generic
			PlusIcon = LoadTexture("Generic/Plus.png", "PlusIcon");
		}

		static void Shutdown()
		{
			// Generic
			PlusIcon.Reset();
		}

	private:
		static Ref<Texture2D> LoadTexture(const std::filesystem::path& relativePath, const std::string& name)
		{
			std::filesystem::path path = std::filesystem::path("Resources") / "Editor" / relativePath;

			if (!Utils::FileSystem::Exists(path))
			{
				RE_CORE_FATAL("Failed to load icon {0}! The file doesn't exist.", path.string());
				return nullptr;
			}

			return Texture2D::Create(path.string());
		}
	};

}
