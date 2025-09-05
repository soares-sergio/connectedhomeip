/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "Rpc.h"

#include "pw_rpc/server.h"
#include "pw_rpc_system_server/rpc_server.h"
#include "pw_rpc_system_server/socket.h"

#include <memory>
#include <platform/PlatformManager.h>
#include <thread>

#include "BridgeService.h"
#include "TestService.h"

namespace chip::rpc {
namespace {

all_devices::rpc::TestService test_service;
std::unique_ptr<all_devices::rpc::Bridge> bridge;

} // namespace

void RunRpcService(app::DeviceManager & deviceManager)
{

    VerifyOrDie(!bridge);
    bridge = std::make_unique<all_devices::rpc::Bridge>(deviceManager);

    pw::rpc::system_server::Server().RegisterService(test_service);
    pw::rpc::system_server::Server().RegisterService(*bridge);

    // Init may block on accept (it waits for the forst accept)
    // Start will loop.
    pw::rpc::system_server::Init();
    VerifyOrDie(pw::rpc::system_server::Start().ok());
}

void Start(uint16_t rpcServerPort, app::DeviceManager & deviceManager)
{
    pw::rpc::system_server::set_socket_port(rpcServerPort);
    std::thread rpc_service(RunRpcService, std::ref(deviceManager));
    rpc_service.detach();
}

void Stop()
{
    pw::rpc::system_server::Server().UnregisterService(test_service, *bridge);
    bridge.reset();
}

} // namespace chip::rpc
