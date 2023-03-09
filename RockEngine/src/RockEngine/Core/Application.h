#pragma once

#include <string>

namespace RockEngine
{
	struct ApplicationProps {
		uint32_t WindowWidth, WindowHeight;
		std::string Name;
	};

	class Application
	{
	public:
		Application(const ApplicationProps& props);
		virtual ~Application() = default;

		virtual void Run();
	};

	//Iml. by client
	Application* CreateApplication();
}