//========================================================================
// EventManagerImpl.cpp : implementation side of the event system
//========================================================================
#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
//---------------------------------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------------------------------
#include <list>
#include <map>

#include "../../include/events/IEventManager.h"
//---------------------------------------------------------------------------------------------------------------------
// Forward declaration & typedefs
//---------------------------------------------------------------------------------------------------------------------
const unsigned int EVENTMANAGER_NUM_QUEUES = 2;
typedef std::list<EventListenerDelegate> EventListenerList;
typedef std::map<std::string, EventListenerList> EventListenerMap;
typedef std::list<IEventPtr> EventQueue;
//========================================================================
// EventManager : implements a multi-listener multi-sender event system
//========================================================================
class EventManager : public IEventManager
{
//---------------------------------------------------------------------------------------------------------------------
// Members
//---------------------------------------------------------------------------------------------------------------------
public:
    EventListenerMap m_eventListeners;
    EventQueue m_queues[EVENTMANAGER_NUM_QUEUES];
    int m_activeQueue;  // index of actively processing queue; events enque to the opposing queue
//---------------------------------------------------------------------------------------------------------------------
// Public Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	explicit EventManager(bool setAsGlobal);
	virtual ~EventManager(void);

    virtual void On(const std::string& eventName, const EventListenerDelegate& listener);
    virtual bool AddListener(const std::string& eventName, const EventListenerDelegate& listener);
    virtual bool RemoveListener(const std::string& eventName, const EventListenerDelegate& listener);

    virtual bool TriggerEvent(const IEventPtr& pEvent) const;
    virtual bool QueueEvent(const IEventPtr& pEvent);
    virtual bool AbortEvent(const std::string& eventName, bool allOfType = false);

    virtual bool Update(unsigned long maxMillis = INFINITE);
};
#endif //EVENTMANAGER_H
