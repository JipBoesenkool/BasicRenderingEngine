//========================================================================
// EventManager.cpp : implements a multi-listener multi-sender event system
//========================================================================
#include "IEventManager.h"
//---------------------------------------------------------------------------------------------------------------------
// Global
//---------------------------------------------------------------------------------------------------------------------
static IEventManager* g_pEventMgr = NULL;
GenericObjectFactory<std::string> g_eventFactory;
//---------------------------------------------------------------------------------------------------------------------
// Getters & Setters
//---------------------------------------------------------------------------------------------------------------------
IEventManager* IEventManager::Get(void)
{
	return g_pEventMgr;
}
//---------------------------------------------------------------------------------------------------------------------
// Public Functions
//---------------------------------------------------------------------------------------------------------------------
IEventManager::IEventManager(bool setAsGlobal)
{
	if (setAsGlobal)
    {
        if (g_pEventMgr)
        {
            //GCC_ERROR("Attempting to create two global event managers! The old one will be destroyed and overwritten with this one.");
            delete g_pEventMgr;
        }
		g_pEventMgr = this;
    }
}

IEventManager::~IEventManager(void)
{
	if (g_pEventMgr == this)
    {
		g_pEventMgr = NULL;
    }
}
