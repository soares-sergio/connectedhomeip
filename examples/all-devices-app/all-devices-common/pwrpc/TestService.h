#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "test_service/test_service.rpc.pb.h"

namespace chip::rpc {

class TestService final : public pw_rpc::nanopb::TestService::Service<TestService>
{
public:
    pw::Status GetDeviceInfo(const pw_protobuf_Empty & request, chip_rpc_DeviceInfo & response);
};

} // namespace chip::rpc
