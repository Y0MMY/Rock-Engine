#include "pch.h"
#include "ImGuiFonts.h"

namespace RockEngine::UI 
{
	static std::unordered_map<std::string, ImFont*> s_Fonts;

	void Fonts::Add(const std::string& fontName, const std::string& filepath, float fontSize, bool isDefault /*= false*/)
	{
		if(s_Fonts.find(fontName) !=s_Fonts.end())
		{ 
			RE_CORE_WARN("Tried to add font with name '{0}' but that name is already taken!", fontName);
			return;
		}

		auto& io = ImGui::GetIO();
		ImFont* font = io.Fonts->AddFontFromFileTTF(filepath.c_str(), fontSize, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		RE_CORE_ASSERT(font, "Failed to load font file!");
		s_Fonts[fontName] = font;

		if (isDefault)
			io.FontDefault = font;

	}

	ImFont* Fonts::Get(const std::string& fontName)
	{
		RE_CORE_ASSERT(s_Fonts.find(fontName) != s_Fonts.end(), "Failed to find font with that name!");
		return s_Fonts[fontName];

	}

}