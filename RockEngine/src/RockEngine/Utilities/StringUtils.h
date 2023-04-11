#pragma once

namespace RockEngine::Utils 
{
	std::string GetFilename(const std::string& filepath);
	std::vector<std::string> SplitString(const std::string_view string, const std::string_view& delimiters);
}
