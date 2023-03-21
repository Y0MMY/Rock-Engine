#pragma once

#include "Event.h"

namespace RockEngine
{
	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			:m_Width(width), m_Height(height)
		{

		}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		EVENT_CLASS_TYPE(WindowResize)
	private:
		uint32_t m_Width;
		uint32_t m_Height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent()
		{

		}
		EVENT_CLASS_TYPE(WindowClose)
	};
}