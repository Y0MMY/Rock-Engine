#pragma once

#include <string>

#include "RockEngine/Core/Core.h"
#include "RockEngine/Core/TimeStep.h"

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

		virtual void OnImGuiRender() {}

		inline const std::string& GetName() { return m_Name; }
	private:
		std::string m_Name;
	};
}