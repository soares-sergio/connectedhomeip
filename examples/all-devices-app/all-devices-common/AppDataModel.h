/*
 *
 *    Copyright (c) 2025 Project CHIP Authors
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

#pragma once

#include <cstdint>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <devices/Device.h>
#include <devices/Manager.h>
#include <memory>

enum class AppDeviceType
{
    kUnknown         = 0,
    kContactSensor   = 1,
    kOccupancySensor = 2,
    kRootNode, // Minimal root node, only mandatory clusters
    kBridge,
    kBridgedNode, // Minimal bridged node, only mandatory clusters
    kLight,
    kPlug,
};

CHIP_ERROR RegisterNewDevice(AppDeviceType deviceType, std::string unique_id, chip::EndpointId parentEndpointId,
                             chip::app::DeviceManager & deviceManager);
