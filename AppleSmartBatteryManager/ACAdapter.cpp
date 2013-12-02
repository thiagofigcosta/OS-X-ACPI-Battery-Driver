//
//  ACAdapter.cpp
//  ACPIBatteryManager
//
//  Created by RehabMan on 11/16/13.
//
//  Code/ideas liberally borrowed from VoodooBattery...
//

#include "ACAdapter.h"
#include <IOKit/pwr_mgt/RootDomain.h>
#include <IOKit/pwr_mgt/IOPMPowerSource.h>
#include "IOPMPrivate.h"

OSDefineMetaClassAndStructors(ACPIACAdapter, IOService)

IOService* ACPIACAdapter::probe(IOService* provider, SInt32* score) {
    
    if (IOService::probe(provider, score) != this)
        return 0;
    
    IOACPIPlatformDevice* acpi = OSDynamicCast(IOACPIPlatformDevice, provider);
    if (!acpi || acpi->validateObject("_PRW"))
        return 0;
    
    return this;
}

bool ACPIACAdapter::start(IOService* provider)
{
    if (!IOService::start(provider))
        return false;
    
    fProvider = OSDynamicCast(IOACPIPlatformDevice, provider);
    if (!fProvider)
        return false;
    
    fProvider->retain();
	IOLog("ACPIBatteryManager: Version 1.50 starting ACPIACAdapter\n");
    
    return true;
}

void ACPIACAdapter::stop(IOService* provider)
{
    OSSafeReleaseNULL(fProvider);
    
    IOService::stop(provider);
}

void ACPIACAdapter::free(void)
{
    IOService::free();
}

IOReturn ACPIACAdapter::setPowerState(unsigned long state, IOService* device)
{
    DEBUG_LOG("ACPIACAdapter::setPowerState: state: %u, device: %s", (unsigned int) state, device->getName());
    
    return kIOPMAckImplied;
}

IOReturn ACPIACAdapter::message(UInt32 type, IOService* provider, void* argument)
{
    DEBUG_LOG("ACPIACAdapter::message: type: %08X provider: %s", (unsigned int)type, provider->getName());
    
    if (type == kIOACPIMessageDeviceNotification && fProvider)
    {
        UInt32 acpi = 0;
        if (kIOReturnSuccess == fProvider->evaluateInteger("_PSR", &acpi))
        {
            DEBUG_LOG("ACPIACAdapter: setting AC %s\n", (acpi ? "connected" : "disconnected"));
            IOPMrootDomain* root = getPMRootDomain();
            if (root)
            {
                if (acpi)
                    root->receivePowerNotification(kIOPMSetACAdaptorConnected | kIOPMSetValue);
                else
                    root->receivePowerNotification(kIOPMSetACAdaptorConnected);
            }
            else
            {
                DEBUG_LOG("ACPIACAdapter: couldn't notify OS about AC status\n");
            }
        }
    }
    return kIOReturnSuccess;
}

