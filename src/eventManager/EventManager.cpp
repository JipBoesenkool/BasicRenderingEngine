//========================================================================
// EventManagerImpl.cpp : implementation side of the event system
//========================================================================
#include <iostream>

#include "EventManager.h"
//---------------------------------------------------------------------------------------------------------------------
// Public Functions
//---------------------------------------------------------------------------------------------------------------------
EventManager::EventManager(bool setAsGlobal)
	: IEventManager(setAsGlobal)
{
    m_activeQueue = 0;
}

EventManager::~EventManager()
{
    // Empty 
}

void EventManager::On(const std::string& eventName, const EventListenerDelegate& listener)
{
    AddListener(eventName, listener);
}

bool EventManager::AddListener(const std::string& eventName, const EventListenerDelegate& listener)
{
    //DEBUG:
    //std::cout << "EventsManager::AddListener: Attempting to add delegate function for event type: " << eventName << std::endl;
    // this will find or create the entry
    EventListenerList& eventListenerList = m_eventListeners[eventName];
    for (auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
    {
        if (listener == (*it))
        {
            std::cout << "ERROR::EventsManager::AddListener: Attempting to double-register a delegate" << std::endl;
            return false;
        }
    }
    eventListenerList.push_back(listener);
    //std::cout << "EventsManager::AddListener: Successfully added delegate for event type: " << eventName << std::endl;
    return true;
}

bool EventManager::RemoveListener(const std::string& eventName, const EventListenerDelegate& listener)
{
    //DEBUG:
    //std::cout << "EventsManager::RemoveListener: Attempting to remove delegate function from event type: " << eventName << std::endl;
    bool success = false;

    auto findIt = m_eventListeners.find(eventName);
    if (findIt != m_eventListeners.end())
    {
        EventListenerList& listeners = findIt->second;
        for (auto it = listeners.begin(); it != listeners.end(); ++it)
        {
            if (listener == (*it))
            {
                listeners.erase(it);
                //std::cout << "EventsManager::RemoveListener: Successfully removed delegate function from event type: " << eventName << std::endl;
                success = true;
                break;
            }
        }
    }
    return success;
}

bool EventManager::TriggerEvent(const IEventPtr& pEvent) const
{
    //DEBUG:
    //std::cout << "EventsManager::TriggerEvent: Attempting to trigger event: " << pEvent->GetEventName() << std::endl;
    bool processed = false;

    auto findIt = m_eventListeners.find(pEvent->GetEventName());
	if (findIt != m_eventListeners.end())
    {
	    const EventListenerList& eventListenerList = findIt->second;
	    for (EventListenerList::const_iterator it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
	    {
		    EventListenerDelegate listener = (*it);
            //std::cout << "EventsManager::TriggerEvent: Sending Event: " << pEvent->GetEventName()<< " to delegate." << std::endl;
		    listener(pEvent); // call the delegate
            processed = true;
	    }
    }
	return processed;
}

bool EventManager::QueueEvent(const IEventPtr& pEvent)
{
    // make sure the event is valid
    if (!pEvent)
    {
        std::cerr << "ERROR::EventsManager::QueueEvent: Invalid event parameter." << std::endl;
        return false;
    }

    //DEBUG:
    //std::cout << "EventsManager::QueueEvent: Attempting to queue event: " << pEvent->GetEventName() << std::endl;

	auto findIt = m_eventListeners.find(pEvent->GetEventName());
    if (findIt != m_eventListeners.end())
    {
        m_queues[m_activeQueue].push_back(pEvent);
        //std::cout << "EventsManager::QueueEvent: Successfully queued event: " << pEvent->GetEventName() << std::endl;
        return true;
    }
    else
    {
        //std::cout << "EventsManager::QueueEvent: Skipping event since there are no delegates registered to receive it: " << pEvent->GetEventName() << std::endl;
        return false;
    }
}

bool EventManager::AbortEvent(const std::string& eventName, bool allOfType)
{
    bool success = false;
	EventListenerMap::iterator findIt = m_eventListeners.find(eventName);
	if (findIt != m_eventListeners.end())
    {
        EventQueue& eventQueue = m_queues[m_activeQueue];
        auto it = eventQueue.begin();
        while (it != eventQueue.end())
        {
            // Removing an item from the queue will invalidate the iterator, so have it point to the next member.  All
            // work inside this loop will be done using thisIt.
            auto thisIt = it;
            ++it;
	        if ((*thisIt)->GetEventName() == eventName)
	        {
		        eventQueue.erase(thisIt);
		        success = true;
		        if (!allOfType)
                {
			        break;
                }
	        }
        }
    }
	return success;
}

bool EventManager::Update(unsigned long maxMillis)
{
	//TODO: GetTickCount?
	// unsigned long currMs = GetTickCount();
	// unsigned long maxMs = ((maxMillis == INFINITE) ? (INFINITE) : (currMs + maxMillis));

	// swap active queues and clear the new queue after the swap
    int queueToProcess = m_activeQueue;
	m_activeQueue = (m_activeQueue + 1) % EVENTMANAGER_NUM_QUEUES;
	m_queues[m_activeQueue].clear();

    //DEBUG:
    //std::cout << "EventsManager::EventLoop: Processing Event Queue: " << queueToProcess << "; " << m_queues[queueToProcess].size() << " events to process" << std::endl;

	// Process the queue
	while (!m_queues[queueToProcess].empty())
	{
        // pop the front of the queue
		IEventPtr pEvent = m_queues[queueToProcess].front();
        m_queues[queueToProcess].pop_front();
        //std::cout << "EventsManager::EventLoop: Processing event: " << pEvent->GetEventName() << std::endl;

		const std::string& eventName = pEvent->GetEventName();

        // find all the delegate functions registered for this event
		auto findIt = m_eventListeners.find(eventName);
		if (findIt != m_eventListeners.end())
		{
			const EventListenerList& eventListeners = findIt->second;
            //std::cout << "EventsManager::EventLoop: Found " << eventListeners.size() << " delegates" << std::endl;
            // call each listener
			for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
			{
                EventListenerDelegate listener = (*it);
                //std::cout << "EventsManager::EventLoop: Sending event " << pEvent->GetEventName() << " to delegate" << std::endl;
				listener(pEvent);
			}
		}

        // check to see if time ran out
		//currMs = GetTickCount();
		//if (maxMillis != INFINITE && currMs >= maxMs)
        //{
        //    std::cout << "EventsManager::EventLoop: Aborting event processing; time ran out." << std::endl;
		//	break;
        //}
	}
	
	// If we couldn't process all of the events, push the remaining events to the new active queue.
	// Note: To preserve sequencing, go back-to-front, inserting them at the head of the active queue
	bool queueFlushed = (m_queues[queueToProcess].empty());
	if (!queueFlushed)
	{
		while (!m_queues[queueToProcess].empty())
		{
			IEventPtr pEvent = m_queues[queueToProcess].back();
			m_queues[queueToProcess].pop_back();
			m_queues[m_activeQueue].push_front(pEvent);
		}
	}
	return queueFlushed;
}
