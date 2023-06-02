#pragma once

namespace RockEngine::Utils 
{
	auto ReadFromFile(const std::filesystem::path& path) -> std::string;
	void SaveToFile(const std::filesystem::path& path, std::string_view source);
}
