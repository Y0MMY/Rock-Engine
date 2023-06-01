#pragma once

#include <TheRock.h>
#include "RockEngine/Core/Layer.h"
#include "GLFW/include/GLFW/glfw3.h"

#include "RockEngine/Project/UserPreferences.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Launcher
{
	struct LauncherProperties
	{
		RockEngine::Ref<RockEngine::UserPreferences> UserPreferences;
		std::string InstallPath;
	};

	class LauncherLayer : public RockEngine::Layer
	{
	public:
		LauncherLayer(const LauncherProperties& properties);
		virtual ~LauncherLayer() override;

		void OnImGuiRender() override;
		void OnAttach() override;
	private:
		LauncherProperties m_Properties;
		ImGuiID m_HoveredProjectID;
		RockEngine::Ref<RockEngine::Texture2D> m_RecentsTexture;
	};
}