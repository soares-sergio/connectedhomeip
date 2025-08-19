#pragma once

#include "Device.h"

#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <lib/core/CHIPError.h>

#include <list>
#include <map>
#include <memory>

namespace chip::app {

/// Maintains a set of active devices
/// Devices are identified by a unique ID
class DeviceManager
{
public:
    /// Creates a device manager which will start adding devices
    /// at startEndpointId
    DeviceManager(EndpointId startEndpointId, CodeDrivenDataModelProvider & provider);
    ~DeviceManager();

    /// Adds the given device to the internal list of devices
    CHIP_ERROR AddDevice(std::unique_ptr<Device> device);

    /// Return the device with the given id or nullptr if device is not found
    Device * GetDevice(const char * unique_id);

    /// Remove the given device from the manager
    CHIP_ERROR RemoveDevice(const char * unique_id);

    /// Returns pointers to all active devices maintained by this manager
    std::list<Device *> AllDevices();

private:
    EndpointId mEndpointIdToAdd;
    CodeDrivenDataModelProvider & mDataModelProvider;
    std::map<std::string, std::unique_ptr<Device>> mActiveDevices;
};

} // namespace chip::app
