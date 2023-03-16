#pragma once

#include <string>

#include "RockEngine/Core/Core.h"

namespace RockEngine
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() {}

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnUpdate() = 0;

		inline const std::string& GetName() { return m_Name; }
	private:
		std::string m_Name;
	};
}