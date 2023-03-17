#pragma once

#include <TheRock.h>
#include "RockEngine/Core/Layer.h"
#include "GLFW/include/GLFW/glfw3.h"

namespace RockEngine
{
	class Editor : public Layer
	{
		virtual void OnAttach() {
			
		}
		virtual void OnDetach() { }
		virtual void OnUpdate() 
		{
			RockEngine::Renderer::Clear(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
		}
		virtual void OnImGuiRender()
		{
			ImGui::Begin("GameLayer");
			ImGui::ColorEdit4("Clear Color", m_ClearColor);
			ImGui::End();
		}
	private:
		float m_ClearColor[4];
	};
}