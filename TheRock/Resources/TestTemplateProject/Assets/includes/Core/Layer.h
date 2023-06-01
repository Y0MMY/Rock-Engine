#pragma once

#include <string>
#include "TimeStep.h"
#include "Events/Event.h"

namespace RockEngine
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event& event) {}
		virtual void OnImGuiRender() {}

		inline const std::string& GetName() { return m_Name; }
	private:
		std::string m_Name;
	};
}