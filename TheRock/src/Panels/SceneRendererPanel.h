#pragma once

#include "RockEngine/Memory/Ref.h"

#include "RockEngine/Renderer/Renderer.h"
#include "RockEngine/Renderer/SceneRenderer.h"

namespace RockEngine {

	class SceneRendererPanel 
	{
	public:
		SceneRendererPanel() = default;
		virtual ~SceneRendererPanel() = default;

		void SetContext(const Ref<SceneRenderer>& context) { m_Context = context; }
		void OnImGuiRender(bool isOpen);
	private:
		Ref<SceneRenderer> m_Context = nullptr;
	};

}

