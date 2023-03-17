#include "TheRock.h"
#include "EditorLayer.h"

class Sandbox : public RockEngine::Application
{
public:
	Sandbox(const RockEngine::ApplicationProps& props)
		: RockEngine::Application(props)
	{}

	void OnInit() override
	{
		PushLayer(new RockEngine::Editor());
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