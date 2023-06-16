#pragma once

namespace RockEngine::Utils 
{
	auto ReadFromFile(const std::filesystem::path& path) -> std::string;
	void SaveToFile(const std::filesystem::path& path, std::string_view source);
	std::string ToUpper(const std::string_view& string);
}
