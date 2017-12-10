#ifndef BASEEVENT_H
#define BASEEVENT_H
//---------------------------------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------------------------------
#include "IEvent.h"
//-------------------------------------------------------------------------------------------------------------------
// class BaseEvent
// Base type for event object hierarchy, may be used itself for simplest event notifications such as those that do 
// not carry additional payload data. If any event needs to propagate with payload data it must be defined separately.
//-------------------------------------------------------------------------------------------------------------------
class BaseEvent : public IEvent
{
//-------------------------------------------------------------------------------------------------------------------
// Members
//-------------------------------------------------------------------------------------------------------------------
private:
    const float m_timeStamp;
public:
	std::string m_eventName;
//-------------------------------------------------------------------------------------------------------------------
// Getters & Setters
//-------------------------------------------------------------------------------------------------------------------
public:
	virtual std::string& GetEventName(void) { return m_eventName; }
	float GetTimeStamp(void) const { return m_timeStamp; }
//-------------------------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------------------------
public:
	explicit BaseEvent(const float timeStamp = 0.0f) : m_timeStamp(timeStamp) { }
};
#endif //BASEEVENT_H