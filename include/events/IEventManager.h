//========================================================================
// EventManager.h : implements a multi-listener multi-sender event system
//========================================================================
#ifndef IEVENTMANAGER_H
#define IEVENTMANAGER_H
//---------------------------------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------------------------------
#include <strstream>

#include "../utilities/GenericObjectFactory.h"
#include "../deps/FastDelegate/FastDelegate.h"
#include "IEvent.h"
#include "BaseEvent.h"
//All events
#include "../../include/events/WindowResizeEvent.h"
#include "../../include/events/KeyEvent.h"
#include "../../include/events/MouseMoveEvent.h"
#include "../../include/events/MouseScrollEvent.h"
//---------------------------------------------------------------------------------------------------------------------
// Forward declaration & typedefs
//---------------------------------------------------------------------------------------------------------------------
typedef fastdelegate::FastDelegate1<IEventPtr> EventListenerDelegate;
#define INFINITE 0xffffffff 
//---------------------------------------------------------------------------------------------------------------------
// Macro for event registration
//---------------------------------------------------------------------------------------------------------------------
extern GenericObjectFactory<std::string> g_eventFactory;
#define REGISTER_EVENT(eventClass) g_eventFactory.Register<eventClass>(eventClass::m_eventName)
#define CREATE_EVENT(eventName) g_eventFactory.Create(eventName)
//---------------------------------------------------------------------------------------------------------------------
// IEventManager
//
// This is the object which maintains the list of registered events and their listeners.
//
// This is a many-to-many relationship, as both one listener can be configured to process multiple event types and 
// of course multiple listeners can be registered to each event type.
//
// The interface to this construct uses smart pointer wrapped objects, the purpose being to ensure that no object 
// that the registry is referring to is destroyed before it is removed from the registry AND to allow for the registry 
// to be the only place where this list is kept ... the application code does not need to maintain a second list.
//
// Simply tearing down the registry (e.g.: destroying it) will automatically clean up all pointed-to objects (so long 
// as there are no other outstanding references, of course).
//---------------------------------------------------------------------------------------------------------------------
class IEventManager
{
//---------------------------------------------------------------------------------------------------------------------
// Getters & Setters
//---------------------------------------------------------------------------------------------------------------------
public:
    /**
    // Getter for the main global event manager. This is the event manager that is used by the majority of the 
    // engine, though you are free to define your own as long as you instantiate it with setAsGlobal set to false.
    // It is not valid to have more than one global event manager.
    */
	static IEventManager* Get(void);
//---------------------------------------------------------------------------------------------------------------------
// Public Functions
//---------------------------------------------------------------------------------------------------------------------
public:
	explicit IEventManager(bool setAsGlobal);
	virtual ~IEventManager(void);
    // Registers a delegate function that will get called when the event type is triggered.  Returns true if 
    // successful, false if not.
    virtual void On(const std::string& eventName, const EventListenerDelegate& listener) = 0;
    virtual bool AddListener(const std::string& eventName, const EventListenerDelegate& listener) = 0;

	// Removes a delegate / event type pairing from the internal tables.  Returns false if the pairing was not found.
	virtual bool RemoveListener(const std::string& eventName, const EventListenerDelegate& listener) = 0;

	// Fire off event NOW.  This bypasses the queue entirely and immediately calls all delegate functions registered 
    // for the event.
	virtual bool TriggerEvent(const IEventPtr& pEvent) const = 0;
	// Fire off event.  This uses the queue and will call the delegate function on the next call to Update(), assuming
    // there's enough time.
	virtual bool QueueEvent(const IEventPtr& pEvent) = 0;

	// Find the next-available instance of the named event type and remove it from the processing queue.  This 
    // may be done up to the point that it is actively being processed ...  e.g.: is safe to happen during event
	// processing itself.
	//
	// if allOfType is true, then all events of that type are cleared from the input queue.
	//
	// returns true if the event was found and removed, false otherwise
	virtual bool AbortEvent(const std::string& eventName, bool allOfType = false) = 0;

	// Allow for processing of any queued messages, optionally specify a processing time limit so that the event 
    // processing does not take too long. Note the danger of using this artificial limiter is that all messages 
    // may not in fact get processed.
	//
	// returns true if all messages ready for processing were completed, false otherwise (e.g. timeout )
	virtual bool Update(unsigned long maxMillis = INFINITE) = 0;
};
#endif //IEVENTMANAGER_H
