#ifndef KEYEVENT_H
#define KEYEVENT_H
//---------------------------------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------------------------------
#include "BaseEvent.h"
//-------------------------------------------------------------------------------------------------------------------
// class KeyEvent
//-------------------------------------------------------------------------------------------------------------------
class KeyEvent : public BaseEvent
{
//-------------------------------------------------------------------------------------------------------------------
// Members
//-------------------------------------------------------------------------------------------------------------------
public:
	int m_key;
	int m_scancode;
	int m_action;
	int m_mods;
//-------------------------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------------------------
public:
	KeyEvent(void)
	{
		m_eventName = "KeyEvent";
	}

	explicit KeyEvent(int key, int scancode, int action, int mods)
	{
		m_eventName = "KeyEvent";
		m_key = key;
		m_scancode = scancode;
		m_action = action;
		m_mods = mods;
	}

	virtual IEventPtr Copy(void) const
	{
		return IEventPtr( new KeyEvent(m_key, m_scancode, m_action, m_mods) );
	}
};
#endif //KEYEVENT_H
