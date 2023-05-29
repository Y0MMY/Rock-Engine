#include "TheRock.h"
#include "RockEngine/EntryPoint.h"
#include "EditorLayer.h"

class Sandbox : public RockEngine::Application
{
public:
	Sandbox(const RockEngine::ApplicationProps& props)
		: RockEngine::Application(props)
	{}

	void OnInit() override
	{
		auto dllHandle = LoadLibraryA("TestTemplate.dll");
		if (dllHandle != NULL)
		{
			typedef RockEngine::Layer* (*ScriptClass)();
			ScriptClass LoadScriptClass = (ScriptClass)GetProcAddress(dllHandle, "CreateInstanceApplication");
			if (LoadScriptClass != NULL)
			{
				auto object = LoadScriptClass();
				PushLayer(object);
			}
		}
		//FreeLibrary(dllHandle);
		PushLayer(new RockEngine::EditorLayer());
	}
};

RockEngine::Application* RockEngine::CreateApplication()
{
	RockEngine::ApplicationProps props;
	props.WindowHeight = 1600;
	props.WindowWidth = 1600;
	props.Name = "Title";
	return new Sandbox(props);
}