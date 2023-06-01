#pragma once

namespace RockEngine
{
	typedef void* REInstance;
	struct ScriptEngineConfig
	{
		std::string FilePath = "Resources/Scripts/TestTemplate.dll";
		std::string NameMethod = "CreateInstanceApplication";
	};

	class ScriptEngine //TODO: move ScriptEngine to platform
	{
	public:
		static bool Init(const ScriptEngineConfig& config = ScriptEngineConfig());
		static REInstance GetInstanceObject(const std::string& namemethod = "");
		static void Shutdown();
	private:
	};
}