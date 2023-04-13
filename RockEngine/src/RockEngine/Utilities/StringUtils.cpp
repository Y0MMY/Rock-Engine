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

	auto ReadFromFile(std::string_view path) -> std::string
	{
		std::ifstream in(path.data(), std::ios::out);
		std::string result;

		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());

			in.seekg(std::ios::beg);
			in.read(&result[0], result.size());
		}
		else
		{
			RE_CORE_ASSERT(false, "Could not open file!");
		}
		in.close();

		return result;
	}

	void SaveToFile(std::string_view path, std::string_view source)
	{
		std::ofstream output(path.data());
		output << (source);
		output.close();
	}

	std::string GetExtension(const std::string& filename)
	{
		auto file = std::filesystem::path(filename);
		return file.extension().string();
	}

}