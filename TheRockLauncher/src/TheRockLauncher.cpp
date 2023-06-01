#include <TheRock.h>
#include "RockEngine/EntryPoint.h"

#include "RockEngine/Utilities/FileSystem.h"

#include "LauncherLayer.h"

class TheRockLauncher : public RockEngine::Application
{
public:
	TheRockLauncher(const RockEngine::ApplicationSpecification& spec) 
		: RockEngine::Application(spec), m_UserPreferences(RockEngine::Ref<RockEngine::UserPreferences>::Create())
	{
	}

	void OnInit() override
	{
		Launcher::LauncherProperties launcherProperties;
		launcherProperties.UserPreferences = m_UserPreferences;

		// Installation Path
		{
			//TODO: Replace in installation string from / to \\

			if (RockEngine::Utils::FileSystem::HasEnvironmentVariable("THEROCK_DIR"))
				launcherProperties.InstallPath = RockEngine::Utils::FileSystem::GetEnvironmentVariable("THEROCK_DIR"); 
		}
		PushLayer(new Launcher::LauncherLayer(launcherProperties));
	}
private:
	RockEngine::Ref<RockEngine::UserPreferences> m_UserPreferences;
};

RockEngine::Application* RockEngine::CreateApplication(int argc, char** argv)
{
	RockEngine::ApplicationSpecification spec;
	spec.Name = "Launcher";
	spec.Mode = ApplicationSpecification::ApplicationMode::Launcher;
	spec.LoadFlags = ApplicationSpecification::EnableImGui;
	spec.WindowWidth = 1280;
	spec.WindowHeight = 720;
	return new TheRockLauncher(spec);
}