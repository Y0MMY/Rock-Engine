#pragma once

#include "RockEngine/Core/Layer.h"

namespace RockEngine
{ 
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		ImGuiLayer(const std::string& name);
		virtual ~ImGuiLayer();

		void Begin();
		void End();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		static void SetDarkTheme();
		static void SetLightTheme();
	private:
		float m_Time = 0.0f;
	};

}