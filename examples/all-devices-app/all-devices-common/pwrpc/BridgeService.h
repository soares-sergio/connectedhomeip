#pragma once

#include "bridge_service/bridge_service.rpc.pb.h"

#include <devices/Manager.h>

namespace all_devices::rpc {

class Bridge final : public pw_rpc::nanopb::Bridge::Service<Bridge>
{
public:
    Bridge(chip::app::DeviceManager & deviceManager) : mDeviceManager(deviceManager) {}

    pw::Status AddDevice(const all_devices_rpc_AddDeviceRequest & request, pw_protobuf_Empty & response);
    pw::Status RemoveDevice(const all_devices_rpc_RemoveDeviceRequest & request, pw_protobuf_Empty & response);
    pw::Status ListDevices(const pw_protobuf_Empty & request, all_devices_rpc_DeviceList & response);
    pw::Status UpdateDevice(const all_devices_rpc_UpdateDeviceRequest & request, pw_protobuf_Empty & response);

private:
    chip::app::DeviceManager & mDeviceManager;
};

} // namespace all_devices::rpc
