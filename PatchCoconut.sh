#!/bin/bash

echo Patching /Applications/coconutBattery.app for compatibility with ACPIBatteryManager.kext

set -x

# -- simple patch (only works with ACPIBatteryManager.kext)
# AppleSmartBattery
# rehab_ACPIBattery

#sudo cp /Applications/coconutBattery.app/Contents/MacOS/coconutBattery /Applications/coconutBattery.app/Contents/MacOS/coconutBattery.orig
#sudo perl -pi -e 's|AppleSmartBattery|rehab_ACPIBattery|g' /Applications/coconutBattery.app/Contents/MacOS/coconutBattery


# -- alternate patch (works with all battery managers)
# AppleSmartBattery
# IOPMPowerSource

sudo perl -pi -e 's|AppleSmartBattery\x00|IOPMPowerSource\x00\x00\x00|g' /Applications/coconutBattery.app/Contents/MacOS/coconutBattery
sudo perl -pi -e 's|/usr/sbin/ioreg -rc "AppleSmartBattery"\x00|/usr/sbin/ioreg -rc "IOPMPowerSource"\x00\x00\x00|g' /Applications/coconutBattery.app/Contents/MacOS/coconutBattery
