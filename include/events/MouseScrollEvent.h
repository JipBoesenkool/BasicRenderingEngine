#ifndef MOUSESCROLLEVENT_H
#define MOUSESCROLLEVENT_H
//---------------------------------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------------------------------
#include "BaseEvent.h"
//-------------------------------------------------------------------------------------------------------------------
// class MouseScrollEvent
//-------------------------------------------------------------------------------------------------------------------
class MouseScrollEvent : public BaseEvent
{
//-------------------------------------------------------------------------------------------------------------------
// Members
//-------------------------------------------------------------------------------------------------------------------
public:
	double m_xOffset;
	double m_yOffset;
//-------------------------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------------------------
public:
	MouseScrollEvent(void)
	{
		m_eventName = "MouseScrollEvent";
	}

	explicit MouseScrollEvent(double xOffset, double yOffset)
	{
		m_eventName = "MouseScrollEvent";
		m_xOffset = xOffset;
		m_yOffset = yOffset;
	}

	virtual IEventPtr Copy(void) const
	{
		return IEventPtr( new MouseScrollEvent(m_xOffset, m_yOffset) );
	}
};
#endif //MOUSESCROLLEVENT_H
