#include "BridgeService.h"
#include "bridge_service/bridge_service.pb.h"

#include <devices/Device.h>
#include <devices/OccupancySensorDevice.h>
#include <devices/ContactSensorDevice.h>
#include <devices/BridgedNodeDevice.h>
#include <platform/PlatformManager.h>

#include <lib/support/CHIPMemString.h>
#include <lib/support/logging/CHIPLogging.h>
#include <memory>

using namespace chip;

namespace all_devices::rpc {

pw::Status Bridge::AddDevice(const all_devices_rpc_AddDeviceRequest & request, pw_protobuf_Empty & response)
{
    ChipLogProgress(AppServer, "Request to add device:");
    ChipLogProgress(AppServer, "  UniqueID: %s", request.unique_id);
    ChipLogProgress(AppServer, "  Type: %d", static_cast<int>(request.device_type));

    DeviceLayer::StackLock chipStackLock;

    switch (request.device_type)
    {
    case all_devices_rpc_DeviceType_OCCUPANCY_SENSOR: {
        std::string bridge_id = request.unique_id + std::string("_bridged_node_device");
        CHIP_ERROR err1 = mDeviceManager.AddDevice(std::make_unique<chip::app::BridgedNodeDevice>(bridge_id));
        if (err1 != CHIP_NO_ERROR)
        {
            ChipLogError(AppServer, "Bridged Node Device add failed: %" CHIP_ERROR_FORMAT, err1.Format());
            return pw::Status::Internal();
        }

        CHIP_ERROR err2 = mDeviceManager.AddDevice(std::make_unique<chip::app::OccupancySensorDevice>(request.unique_id));
        if (err2 != CHIP_NO_ERROR)
        {
            ChipLogError(AppServer, "Device add failed: %" CHIP_ERROR_FORMAT, err2.Format());
            return pw::Status::Internal();
        }

        return pw::OkStatus();
    }
    case all_devices_rpc_DeviceType_CONTACT_SENSOR: {
        std::string bridge_id = request.unique_id + std::string("_bridged_node_device");
        CHIP_ERROR err1 = mDeviceManager.AddDevice(std::make_unique<chip::app::BridgedNodeDevice>(bridge_id));
        if (err1 != CHIP_NO_ERROR)
        {
            ChipLogError(AppServer, "Bridged Node Device add failed: %" CHIP_ERROR_FORMAT, err1.Format());
            return pw::Status::Internal();
        }
        
        CHIP_ERROR err = mDeviceManager.AddDevice(std::make_unique<chip::app::ContactSensorDevice>(request.unique_id));
        if (err != CHIP_NO_ERROR)
        {
            ChipLogError(AppServer, "Device add failed: %" CHIP_ERROR_FORMAT, err.Format());
            return pw::Status::Internal();
        }

        return pw::OkStatus();
    }
    default:
        ChipLogError(AppServer, "Cannot handle this device type.");
        return pw::Status::Unimplemented();
    }
}

pw::Status Bridge::RemoveDevice(const all_devices_rpc_RemoveDeviceRequest & request, pw_protobuf_Empty & response)
{
    DeviceLayer::StackLock chipStackLock;

    ChipLogProgress(AppServer, "Request to remove device '%s'", request.unique_id);
    CHIP_ERROR err = mDeviceManager.RemoveDevice(request.unique_id);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Device remove failed: %" CHIP_ERROR_FORMAT, err.Format());
        return pw::Status::Internal();
    }
    return pw::OkStatus();
}

pw::Status Bridge::ListDevices(const pw_protobuf_Empty & request, all_devices_rpc_DeviceList & response)
{
    DeviceLayer::StackLock chipStackLock;

    ChipLogProgress(AppServer, "Request to list devices");

    response.devices_count = 0;
    for (auto & device : mDeviceManager.AllDevices())
    {
        if (response.devices_count == MATTER_ARRAY_SIZE(response.devices))
        {
            ChipLogError(AppServer, "Too many devices to return in API call. Response is truncated.");
            break;
        }
        chip::Platform::CopyString(response.devices[response.devices_count].unique_id, device->GetUniqueId().c_str());
        switch (device->GetDeviceType())
        {
        case chip::app::BridgedDeviceType::kContactSensor:
            response.devices[response.devices_count].device_type = all_devices_rpc_DeviceType::all_devices_rpc_DeviceType_CONTACT_SENSOR;
            break;
        case chip::app::BridgedDeviceType::kOccupancySensor:
            response.devices[response.devices_count].device_type = all_devices_rpc_DeviceType::all_devices_rpc_DeviceType_OCCUPANCY_SENSOR;
            break;
        default:
            response.devices[response.devices_count].device_type = all_devices_rpc_DeviceType::all_devices_rpc_DeviceType_UNKNOWN;
            break;
        }
        response.devices_count++;
    }

    return pw::OkStatus();
}

pw::Status Bridge::UpdateDevice(const all_devices_rpc_UpdateDeviceRequest & request, pw_protobuf_Empty & response)
{
    DeviceLayer::StackLock chipStackLock;

    ChipLogProgress(AppServer, "Searching for device '%s'", request.unique_id);

    auto device = mDeviceManager.GetDevice(request.unique_id);
    VerifyOrReturnError(device != nullptr, pw::Status::NotFound());

    if (request.has_occupied)
    {
        VerifyOrReturnError(device->GetDeviceType() == chip::app::BridgedDeviceType::kOccupancySensor,
                            pw::Status::InvalidArgument());

        chip::app::OccupancySensorDevice * occ = static_cast<chip::app::OccupancySensorDevice *>(device);
        occ->Cluster().SetOccupied(request.occupied);
    } 
    else if (request.has_contact)
    {
        VerifyOrReturnError(device->GetDeviceType() == chip::app::BridgedDeviceType::kContactSensor,
                            pw::Status::InvalidArgument());

        chip::app::ContactSensorDevice * con = static_cast<chip::app::ContactSensorDevice *>(device);
        con->Cluster().SetStateValue(request.contact);
    }

    return pw::OkStatus();
}

} // namespace all_devices::rpc
