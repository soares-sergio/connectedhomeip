#include "Manager.h"

#include <lib/support/CodeUtils.h>

#include <string>

namespace chip::app {

DeviceManager::DeviceManager(EndpointId startEndpointId, CodeDrivenDataModelProvider & provider) :
    mEndpointIdToAdd(startEndpointId), mDataModelProvider(provider)
{}

DeviceManager::~DeviceManager()
{
    for (auto const & [id, device] : mActiveDevices)
    {
        device->UnRegister(mDataModelProvider);
    }
}

CHIP_ERROR DeviceManager::AddDevice(std::unique_ptr<Device> device)
{
    VerifyOrReturnError(device, CHIP_ERROR_INVALID_ARGUMENT);

    const std::string & deviceId = device->GetUniqueId();
    if (mActiveDevices.find(deviceId) != mActiveDevices.end())
    {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    ReturnErrorOnFailure(device->Register(mEndpointIdToAdd, mDataModelProvider));

    mEndpointIdToAdd++;
    mActiveDevices[deviceId] = std::move(device);

    return CHIP_NO_ERROR;
}

Device * DeviceManager::GetDevice(const char * unique_id)
{
    auto it = mActiveDevices.find(unique_id);
    if (it != mActiveDevices.end())
    {
        return it->second.get();
    }
    return nullptr;
}

std::list<Device *> DeviceManager::AllDevices()
{
    std::list<Device *> devices;
    for (auto const & [key, val] : mActiveDevices)
    {
        devices.push_back(val.get());
    }
    return devices;
}

CHIP_ERROR DeviceManager::RemoveDevice(const char * unique_id)
{
    auto it = mActiveDevices.find(unique_id);
    VerifyOrReturnError(it != mActiveDevices.end(), CHIP_ERROR_KEY_NOT_FOUND);

    it->second->UnRegister(mDataModelProvider);
    mActiveDevices.erase(it);
    return CHIP_NO_ERROR;
}

} // namespace chip::app
