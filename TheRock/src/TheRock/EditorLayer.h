#pragma once

#include <TheRock.h>
#include "RockEngine/Core/Layer.h"
#include "GLFW/include/GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

			m_Shader = Shader::Create("assets/shaders/shader.glsl");
			m_Shader->Bind();
		}
		virtual void OnDetach() { }
		virtual void OnUpdate() 
		{
			RockEngine::Renderer::Clear(1.0f,1.0f,1.0f,1.0f);
			m_Shader->SetFloat3("u_Color", m_ClearColor);
			m_IB->Bind();
			m_VB->Bind();
			Renderer::DrawIndexed(3);
		}
		virtual void OnImGuiRender()
		{
			ImGui::Begin("GameLayer");
			ImGui::ColorEdit3("Clear Color", glm::value_ptr(m_ClearColor));
			ImGui::End();
		}
	private:
		VertexBuffer* m_VB;
		IndexBuffer* m_IB;
		glm::vec3 m_ClearColor;

		Shader* m_Shader;
	};
}