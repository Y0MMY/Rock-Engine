#pragma once

#include "Event.h"
#include "RockEngine/Core/KeyCodes.h"

namespace RockEngine
{
	class KeyEvent : public Event
	{
	protected:
		KeyEvent(KeyCode keycode)
			: m_KeyCode(keycode)
		{}
		KeyCode m_KeyCode;
	public:
		inline KeyCode GetKeyCode() const { return m_KeyCode; }
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyCode keycode, int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) {}
		
		inline int GetRepeatCount() const { return m_RepeatCount; }
		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};
}