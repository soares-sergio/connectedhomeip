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
#include <devices/AggregatorDevice.h>

using namespace chip::app::Clusters;

namespace chip::app {

constexpr DeviceTypeId kAggregatorDeviceTypeRevision = 2;

DeviceType AggregatorDevice::GetDeviceType() const
{
    return DeviceType::kAggregator;
}

CHIP_ERROR AggregatorDevice::Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider, EndpointId parentId)
{
    const DescriptorCluster::DeviceType deviceType = { .deviceType = static_cast<DeviceTypeId>(DeviceType::kAggregator),
                                                       .revision   = kAggregatorDeviceTypeRevision };
    ReturnErrorOnFailure(RegisterDescriptor(endpoint, provider, deviceType, parentId));

    mIdentifyCluster.Create(endpoint);
    ReturnErrorOnFailure(provider.AddCluster(mIdentifyCluster.Registration()));

    return provider.AddEndpoint(mEndpointRegistration);
}

void AggregatorDevice::UnRegister(CodeDrivenDataModelProvider & provider)
{
    provider.RemoveEndpoint(mEndpointId);
    UnRegisterBridgedNodeClusters(provider);
    if (mIdentifyCluster.IsConstructed())
    {
        provider.RemoveCluster(&mIdentifyCluster.Cluster());
        mIdentifyCluster.Destroy();
    }
}

} // namespace chip::app
