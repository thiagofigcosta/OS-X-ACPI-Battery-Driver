//
//  BatteryTracker.cpp
//  ACPIBatteryManager
//
//  Created by RehabMan on 12/3/13.
//
//

#include "BatteryTracker.h"

OSDefineMetaClassAndStructors(BatteryTracker, IOService)

bool BatteryTracker::init(OSDictionary* dict)
{
    if (!IOService::init(dict))
        return false;
    
    m_pBatteryList = NULL;
    m_pLock = NULL;
    
    return true;
}

bool BatteryTracker::start(IOService* provider)
{
    IOLog("tracker: entering start\n");
    
    if (!IOService::start(provider))
    {
        IOLog("tracker: IOService::start failed!\n");
        return false;
    }
    
	IOLog("ACPIBatteryManager: Version 1.50 starting BatteryTracker\n");
    
    m_pBatteryList = OSArray::withCapacity(2);
    m_pLock = IOLockAlloc();
    
    registerService();
    
    return true;
}

void BatteryTracker::stop(IOService* provider)
{
    IOLog("tracker: entering stop\n");
    
    OSSafeReleaseNULL(m_pBatteryList);
    if (NULL != m_pLock)
    {
        IOLockFree(m_pLock);
        m_pLock = NULL;
    }
    IOService::stop(provider);
}

bool BatteryTracker::addBatteryManager(AppleSmartBatteryManager* pManager)
{
    IOLog("tracker: entering addBatteryManager(%p)\n", pManager);
    
    bool result = false;
    
    IOLockLock(m_pLock);
    unsigned count = m_pBatteryList->getCount();
    unsigned i = 0;
    for (; i < count; ++i)
    {
        OSObject* pTemp = m_pBatteryList->getObject(i);
        if (pTemp == pManager)
            break;
        if (pTemp == NULL)
        {
            result = true;
            m_pBatteryList->setObject(i, pManager);
            break;
        }
    }
    if (i == count)
    {
        m_pBatteryList->setObject(pManager);
        result = true;
    }
    IOLockUnlock(m_pLock);
    
    return result;
}

bool BatteryTracker::removeBatteryManager(AppleSmartBatteryManager* pManager)
{
    IOLog("tracker: entering removeBatteryManager(%p)\n", pManager);
    
    bool result = false;
    
    IOLockLock(m_pLock);
    unsigned count = m_pBatteryList->getCount();
    for (unsigned i = 0; i < count; ++i)
    {
        if (m_pBatteryList->getObject(i) == pManager)
        {
            m_pBatteryList->setObject(i, NULL);
            result = true;
            break;
        }
    }
    IOLockUnlock(m_pLock);
    
    return result;
}

void BatteryTracker::notifyBatteryManagers(bool connected)
{
    IOLog("tracker: entering notifyBatteryManager(%s)\n", connected ? "connected" : "disconnected");
    
    IOLockLock(m_pLock);
    unsigned count = m_pBatteryList->getCount();
    for (unsigned i = 0; i < count; ++i)
    {
        AppleSmartBatteryManager* pManager = static_cast<AppleSmartBatteryManager*>(m_pBatteryList->getObject(i));
        if (NULL != pManager)
            pManager->notifyConnectedState(connected);
    }
    IOLockUnlock(m_pLock);
}

