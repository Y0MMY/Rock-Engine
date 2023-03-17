#include "TheRock.h"

class Sandbox : public RockEngine::Application
{
public:
	Sandbox(const RockEngine::ApplicationProps& props)
		: RockEngine::Application(props)
	{}

	/*void OnInit() override
	{
		PushLayer(new RockEngine::Editor());
	}
	virtual void OnShutdown(){}
	virtual void OnUpdate(){}*/
};

RockEngine::Application* RockEngine::CreateApplication()
{
	RockEngine::ApplicationProps props;
	props.WindowHeight = 1600;
	props.WindowWidth = 1600;
	props.Name = "Title";
	return new Sandbox(props);
}