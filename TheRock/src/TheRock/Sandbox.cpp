#include "TheRock.h"

class Sandbox : public RockEngine::Application
{
public:
	Sandbox(const RockEngine::ApplicationProps& props)
		: RockEngine::Application(props)
	{}


};

RockEngine::Application* RockEngine::CreateApplication()
{
	RockEngine::ApplicationProps props;
	props.WindowHeight = 600;
	props.WindowWidth = 600;
	props.Name = "Name";
	return new Sandbox(props);
}