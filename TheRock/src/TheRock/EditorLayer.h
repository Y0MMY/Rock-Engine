#pragma once

#include <TheRock.h>
#include "RockEngine/Core/Layer.h"
#include "GLFW/include/GLFW/glfw3.h"

namespace RockEngine
{
	class Editor : public Layer
	{
		virtual void OnAttach() 
		{
			static float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
			};

			static unsigned int indices[] = {
				0, 1, 2
			};

			m_VB = VertexBuffer::Create(vertices, sizeof(vertices));
		

			m_IB = IndexBuffer::Create(indices, sizeof(indices));
		}
		virtual void OnDetach() { }
		virtual void OnUpdate() 
		{
			RockEngine::Renderer::Clear(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);

			m_VB->Bind();
			m_IB->Bind();
			Renderer::DrawIndexed(3);
		}
		virtual void OnImGuiRender()
		{
			ImGui::Begin("GameLayer");
			ImGui::ColorEdit4("Clear Color", m_ClearColor);
			ImGui::End();
		}
	private:
		VertexBuffer* m_VB;
		IndexBuffer* m_IB;
		float m_ClearColor[4];
	};
}