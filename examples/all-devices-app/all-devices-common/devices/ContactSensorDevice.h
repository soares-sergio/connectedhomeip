/*
 *
 *    Copyright (c) 2025 Project CHIP Authors
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

#include <clusters/identify-cluster.h>
#include <clusters/boolean-state-cluster.h>
#include <devices/Device.h>

namespace chip {
namespace app {

class ContactSensorDevice : public Device 
{
public: 
    ContactSensorDevice(std::string id) : Device(std::move(id)) {}
    ~ContactSensorDevice() override = default;

    BridgedDeviceType GetDeviceType() const override;

    CHIP_ERROR Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider) override;
    void UnRegister(CodeDrivenDataModelProvider & provider) override;

    Clusters::BooleanStateCluster & Cluster() { return mBooleanStateCluster.Cluster(); }

private:
    LazyRegisteredServerCluster<Clusters::IdentifyCluster> mIdentifyCluster;
    LazyRegisteredServerCluster<Clusters::BooleanStateCluster> mBooleanStateCluster;
};

}
}