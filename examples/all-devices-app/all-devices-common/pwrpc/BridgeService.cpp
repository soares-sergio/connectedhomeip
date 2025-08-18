#include "BridgeService.h"

#include <lib/support/CHIPMemString.h>
#include <lib/support/logging/CHIPLogging.h>

namespace all_devices::rpc {

pw::Status Bridge::AddDevice(const all_devices_rpc_AddDeviceRequest & request, pw_protobuf_Empty & response)
{
    ChipLogProgress(AppServer, "Request to add device:");
    ChipLogProgress(AppServer, "  UniqueID: %s", request.unique_id);
    ChipLogProgress(AppServer, "  Type: %d", static_cast<int>(request.device_type));
    return pw::Status::Unimplemented();
}

pw::Status Bridge::RemoveDevice(const all_devices_rpc_RemoveDeviceRequest & request, pw_protobuf_Empty & response)
{
    ChipLogProgress(AppServer, "Request to remove device '%s'", request.unique_id);
    return pw::Status::Unimplemented();
}

pw::Status Bridge::ListDevices(const pw_protobuf_Empty & request, all_devices_rpc_DeviceList & response)
{
    ChipLogProgress(AppServer, "Request to list devices");

    // Set up some fake data replies
    chip::Platform::CopyString(response.devices[0].unique_id, "Unique-1");
    response.devices[0].device_type = all_devices_rpc_DeviceType::all_devices_rpc_DeviceType_CONTACT_SENSOR;

    chip::Platform::CopyString(response.devices[0].unique_id, "ABC-123-XYZ");
    response.devices[0].device_type = all_devices_rpc_DeviceType::all_devices_rpc_DeviceType_OCCUPANCY_SENSOR;

    chip::Platform::CopyString(response.devices[0].unique_id, "ABC-234-547");
    response.devices[0].device_type = all_devices_rpc_DeviceType::all_devices_rpc_DeviceType_OCCUPANCY_SENSOR;

    response.devices_count = 3;

    return pw::OkStatus();
}

} // namespace all_devices::rpc
