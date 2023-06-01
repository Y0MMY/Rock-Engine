#include "TestTemplate.h"

namespace TestTemplate
{
	using namespace RockEngine;
	class SandboxLayer : public RockEngine::Layer
	{
	public:
		void OnAttach() override
		{
			printf("OnAttach");
		}	

		void OnUpdate(RockEngine::Timestep ts) override
		{
			
			//RE_CORE_INFO("A");

		}
	};
}

extern "C" __declspec(dllexport) RockEngine::Layer* CreateInstanceApplication()
{
	return new TestTemplate::SandboxLayer();
}