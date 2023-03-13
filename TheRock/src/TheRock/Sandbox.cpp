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
	props.WindowHeight = 1600;
	props.WindowWidth = 1600;
	props.Name = "Title";
	return new Sandbox(props);
}