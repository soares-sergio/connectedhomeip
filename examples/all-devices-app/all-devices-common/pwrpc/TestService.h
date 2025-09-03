#pragma once

#include "test_service/test_service.rpc.pb.h"

namespace all_devices::rpc {

class TestService final : public pw_rpc::nanopb::TestService::Service<TestService>
{
public:
    pw::Status GetDeviceInfo(const pw_protobuf_Empty & request, all_devices_rpc_MatterDeviceInfo & response);
};

} // namespace all_devices::rpc
