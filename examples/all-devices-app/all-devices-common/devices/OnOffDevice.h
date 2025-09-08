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

#include <cluster-impl/groups-cluster.h>
#include <cluster-impl/identify-cluster.h>
#include <cluster-impl/on-off-cluster.h>
#include <devices/Device.h>

namespace chip {
namespace app {

class OnOffDevice : public Device
{
public:
    OnOffDevice(std::string id, DeviceTypeId onOffDeviceType, uint16_t onOffDeviceRevision) : Device(std::move(id)), mOnOffDeviceType(onOffDeviceType), mOnOffDeviceRevision(onOffDeviceRevision) {}
    ~OnOffDevice() override = default;

    DeviceType GetDeviceType() const override;

    CHIP_ERROR Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider,
                        EndpointId parentId = kInvalidEndpointId) override;
    void UnRegister(CodeDrivenDataModelProvider & provider) override;

    Clusters::OnOffCluster & OnOffCluster() { return mOnOffCluster.Cluster(); }
    Clusters::GroupsCluster & GroupsCluster() { return mGroupsCluster.Cluster(); }

private:
    DeviceTypeId mOnOffDeviceType;
    uint16_t mOnOffDeviceRevision;

    LazyRegisteredServerCluster<Clusters::IdentifyCluster> mIdentifyCluster;
    LazyRegisteredServerCluster<Clusters::OnOffCluster> mOnOffCluster;
    LazyRegisteredServerCluster<Clusters::GroupsCluster> mGroupsCluster;
};

} // namespace app
} // namespace chip
