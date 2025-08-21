#include "Manager.h"

#include <clusters/Descriptor/AttributeIds.h>
#include <clusters/Descriptor/ClusterId.h>
#include <lib/support/CodeUtils.h>
#include <platform/PlatformManager.h>

#include <memory>
#include <string>

namespace chip::app {

DeviceManager::DeviceManager(EndpointId startEndpointId, CodeDrivenDataModelProvider & provider) :
    mEndpointIdToAdd(startEndpointId), mDataModelProvider(provider)
{}

DeviceManager::~DeviceManager()
{
    Clear();
}

void DeviceManager::Clear()
{
    for (auto const & [id, deviceData] : mActiveDevices)
    {
        mDataModelProvider.RemoveEndpoint(deviceData.endpointRegistration->GetEndpointEntry().id);
        deviceData.device->UnRegister(mDataModelProvider);
    }

    mDataModelProvider.Temporary_ReportAttributeChanged(
        AttributePathParams{ kRootEndpointId, Clusters::Descriptor::Id, Clusters::Descriptor::Attributes::PartsList::Id });
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
    auto endpointRegistration = std::make_unique<EndpointInterfaceRegistration>(
        *device,
        DataModel::EndpointEntry{
            .id                 = mEndpointIdToAdd,                                  //
            //TODO: Fix the parentId. Devices should map to their respective bridged node device
            .parentId           = 0,                                                 //
            .compositionPattern = DataModel::EndpointCompositionPattern::kFullFamily //
        });

    ReturnErrorOnFailure(mDataModelProvider.AddEndpoint(*endpointRegistration));

    // TODO: this should NOT be needed, but as long as we use the SHIM, the descriptor cluster is
    //       EMBER so we need to notify. See the notifications of emberAfEndpointChanged
    mDataModelProvider.Temporary_ReportAttributeChanged(AttributePathParams{ mEndpointIdToAdd });
    mDataModelProvider.Temporary_ReportAttributeChanged(
        AttributePathParams{ kRootEndpointId, Clusters::Descriptor::Id, Clusters::Descriptor::Attributes::PartsList::Id });

    DeviceData deviceData = {
        .device               = std::move(device),
        .endpointRegistration = std::move(endpointRegistration),
    };

    mEndpointIdToAdd++;
    mActiveDevices[deviceId] = std::move(deviceData);

    return CHIP_NO_ERROR;
}

Device * DeviceManager::GetDevice(const char * unique_id)
{
    auto it = mActiveDevices.find(unique_id);
    if (it != mActiveDevices.end())
    {
        return it->second.device.get();
    }
    return nullptr;
}

std::list<Device *> DeviceManager::AllDevices()
{
    std::list<Device *> devices;
    for (auto const & [key, val] : mActiveDevices)
    {
        devices.push_back(val.device.get());
    }
    return devices;
}

CHIP_ERROR DeviceManager::RemoveDevice(const char * unique_id)
{
    auto it = mActiveDevices.find(unique_id);
    VerifyOrReturnError(it != mActiveDevices.end(), CHIP_ERROR_KEY_NOT_FOUND);

    mDataModelProvider.RemoveEndpoint(it->second.endpointRegistration->GetEndpointEntry().id);
    it->second.device->UnRegister(mDataModelProvider);
    mActiveDevices.erase(it);

    mDataModelProvider.Temporary_ReportAttributeChanged(
        AttributePathParams{ kRootEndpointId, Clusters::Descriptor::Id, Clusters::Descriptor::Attributes::PartsList::Id });

    return CHIP_NO_ERROR;
}

} // namespace chip::app
