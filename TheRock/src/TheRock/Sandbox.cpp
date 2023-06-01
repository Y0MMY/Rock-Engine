#include "TheRock.h"
#include "RockEngine/EntryPoint.h"
#include "EditorLayer.h"

#include "RockEngine/Script/ScriptEngine.h"

class Sandbox : public RockEngine::Application
{
public:
	Sandbox(const RockEngine::ApplicationProps& props)
		: RockEngine::Application(props)
	{}

	void OnInit() override
	{
		typedef RockEngine::Layer* (*ScriptClass)();
		RockEngine::REInstance createmethod = RockEngine::ScriptEngine::GetInstanceObject();
		ScriptClass objecti = (ScriptClass)createmethod;
		auto object = objecti();
		PushLayer(object);
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