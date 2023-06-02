#include "pch.h"
#include "LayerStack.h"

namespace RockEngine
{
	LayerStack::LayerStack()
	{

	}

	LayerStack::~LayerStack()
	{
		
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		RE_CORE_INFO("Adding Layer {}", layer->GetName());
		m_Layers.push_back(layer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = m_Layers.begin();
		while (it != m_Layers.end()) {
			if (*it == layer)
			{
				m_Layers.erase(it);
				break;
			}
			++it;
		}
	}
}