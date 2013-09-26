#!/bin/bash

echo Patching /Applications/coconutBattery.app for compatibility with ACPIBatteryManager.kext

set -x
sudo cp /Applications/coconutBattery.app/Contents/MacOS/coconutBattery /Applications/coconutBattery.app/Contents/MacOS/coconutBattery.orig
sudo perl -pi -e 's|AppleSmartBattery|rehab_ACPIBattery|g' /Applications/coconutBattery.app/Contents/MacOS/coconutBattery
