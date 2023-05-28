#include <TheRock.h>
#include "RockEngine/EntryPoint.h"

#include "LauncherLayer.h"

class TheRockLauncher : public RockEngine::Application
{
public:
	TheRockLauncher(const RockEngine::ApplicationSpecification& spec) 
		: RockEngine::Application(spec)
	{
	}

	void OnInit() override
	{
		PushLayer(new LauncherLayer());
	}
};

RockEngine::Application* RockEngine::CreateApplication(int argc, char** argv)
{
	RockEngine::ApplicationSpecification spec;
	spec.Name = "Launcher";
	spec.Mode = ApplicationSpecification::ApplicationMode::Launcher;
	spec.LoadFlags = ApplicationSpecification::EnableImGui | ApplicationSpecification::WindowDecorated;
	return new TheRockLauncher(spec);
}