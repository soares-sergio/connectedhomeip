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
#include <devices/BridgedNodeDevice.h>

using namespace chip::app::Clusters;

namespace chip::app {

constexpr DeviceTypeId kBridgedNodeDeviceTypeRevision = 3;

DeviceType BridgedNodeDevice::GetDeviceType() const
{
    return DeviceType::kBridgedNodeDevice;
}

CHIP_ERROR BridgedNodeDevice::Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider, EndpointId parentId)
{
    const DescriptorCluster::DeviceType deviceType = { .deviceType = static_cast<DeviceTypeId>(DeviceType::kBridgedNodeDevice),
                                                       .revision   = kBridgedNodeDeviceTypeRevision };
    ReturnErrorOnFailure(RegisterDescriptor(endpoint, provider, deviceType, parentId));

    mBridgedDeviceBasicInformationCluster.Create(endpoint, mUniqueId);
    ReturnErrorOnFailure(provider.AddCluster(mBridgedDeviceBasicInformationCluster.Registration()));

    return provider.AddEndpoint(mEndpointRegistration);
}

void BridgedNodeDevice::UnRegister(CodeDrivenDataModelProvider & provider)
{
    provider.RemoveEndpoint(mEndpointId);
    UnRegisterBridgedNodeClusters(provider);
    if (mBridgedDeviceBasicInformationCluster.IsConstructed())
    {
        provider.RemoveCluster(&mBridgedDeviceBasicInformationCluster.Cluster());
        mBridgedDeviceBasicInformationCluster.Destroy();
    }
}

} // namespace chip::app
