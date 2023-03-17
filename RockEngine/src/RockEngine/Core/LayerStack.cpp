#include "pch.h"
#include "LayerStack.h"

namespace RockEngine
{
	LayerStack::LayerStack()
	{

	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
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