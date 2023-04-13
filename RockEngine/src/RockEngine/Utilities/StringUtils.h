#pragma once

namespace RockEngine::Utils 
{
	std::string GetFilename(const std::string& filepath);
	std::vector<std::string> SplitString(const std::string_view string, const std::string_view& delimiters);
	std::string GetExtension(const std::string& filename);

	auto ReadFromFile(std::string_view path) -> std::string;
	void SaveToFile(std::string_view path, std::string_view source);
}
