#pragma once

#include <imgui.h>

namespace RockEngine::UI {

	class Fonts
	{
	public:
		static void Add(const std::string& fontName, const std::string& filepath, float fontSize, bool isDefault = false);
		static ImFont* Get(const std::string& fontName);
	};

}
