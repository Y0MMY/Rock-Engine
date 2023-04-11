#include "pch.h"
#include "StringUtils.h"

#include <filesystem>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <regex>

namespace RockEngine::Utils
{
	std::string GetFilename(const std::string& filepath)
	{
		auto file = std::filesystem::path(filepath);
		return file.filename().string();
	}

	std::vector<std::string> SplitString(const std::string_view string, const std::string_view& delimiters)
	{
		size_t first = 0;

		std::vector<std::string> result;

		while (first <= string.size())
		{
			const auto second = string.find_first_of(delimiters, first);

			if (first != second)
				result.emplace_back(string.substr(first, second - first));

			if (second == std::string_view::npos)
				break;

			first = second + 1;
		}

		return result;
	}

}