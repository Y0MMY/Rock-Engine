#include "pch.h"
#include "ScriptEngine.h"

namespace RockEngine
{
	static const ScriptEngineConfig m_Config;
	static HMODULE m_Handle;

	bool ScriptEngine::Init(const ScriptEngineConfig& config)
	{
		const_cast<ScriptEngineConfig&>(m_Config) = config;
		m_Handle = LoadLibraryA(config.FilePath.c_str());
		RE_CORE_ASSERT(m_Handle, "Could not read script file");
		return m_Handle != NULL;
	}

	REInstance ScriptEngine::GetInstanceObject(const std::string& namemethod)
	{
		REInstance addres = GetProcAddress(m_Handle, namemethod.empty() ? m_Config.NameMethod.c_str() : namemethod.c_str());
		RE_CORE_ASSERT(addres, "ScriptEngine can't read the function addres");
		return addres;
	}
}