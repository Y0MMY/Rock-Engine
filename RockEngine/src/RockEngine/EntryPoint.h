#pragma once

extern RockEngine::Application* RockEngine::CreateApplication();

int main(int arc, char** argv)
{

	auto app = RockEngine::CreateApplication();
	app->Run();
	delete app;
	return 0;
}