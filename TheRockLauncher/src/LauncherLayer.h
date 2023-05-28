#pragma once

#include <TheRock.h>
#include "RockEngine/Core/Layer.h"
#include "GLFW/include/GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class LauncherLayer : public RockEngine::Layer
{
public:
	void OnImGuiRender() override;
};