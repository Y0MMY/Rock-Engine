#include "TheRock.h"
#include "RockEngine/EntryPoint.h"
#include "EditorLayer.h"

class Sandbox : public RockEngine::Application
{
public:
	Sandbox(const RockEngine::ApplicationSpecification& props)
		: RockEngine::Application(props)
	{}

	void OnInit() override
	{
		PushLayer(new RockEngine::EditorLayer());
	}
};

RockEngine::Application* RockEngine::CreateApplication(int argc, char** argv)
{
	RockEngine::ApplicationSpecification specification;
	specification.WindowHeight = 1600;
	specification.WindowWidth = 1600;
	specification.Name = "Title";
	specification.LoadFlags = ApplicationSpecification::StartMaximized | ApplicationSpecification::EnableImGui;
	return new Sandbox(specification);
}