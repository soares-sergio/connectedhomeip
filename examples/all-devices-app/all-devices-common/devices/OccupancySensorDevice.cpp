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
#include <devices/OccupancySensorDevice.h>

using namespace chip::app::Clusters;

namespace chip::app {

constexpr DeviceTypeId kOccupancySensorDeviceType         = 0x0107;
constexpr DeviceTypeId kOccupancySensorDeviceTypeRevision = 4;

BridgedDeviceType OccupancySensorDevice::GetDeviceType() const
{
    return BridgedDeviceType::kOccupancySensor;
}

CHIP_ERROR OccupancySensorDevice::Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider)
{
    const DescriptorCluster::DeviceType deviceType = { .deviceType = kOccupancySensorDeviceType,
                                                       .revision   = kOccupancySensorDeviceTypeRevision };
    ReturnErrorOnFailure(RegisterDescriptor(endpoint, provider, deviceType));

    mIdentifyCluster.Create(endpoint);
    ReturnErrorOnFailure(provider.AddCluster(mIdentifyCluster.Registration()));

    mOccupancySensingCluster.Create(endpoint, BitFlags<OccupancySensing::Feature>(0));
    ReturnErrorOnFailure(provider.AddCluster(mOccupancySensingCluster.Registration()));

    return CHIP_NO_ERROR;
}

void OccupancySensorDevice::UnRegister(CodeDrivenDataModelProvider & provider)
{
    UnRegisterBridgedNodeClusters(provider);
    if (mOccupancySensingCluster.IsConstructed())
    {
        provider.RemoveCluster(&mOccupancySensingCluster.Cluster());
        mOccupancySensingCluster.Destroy();
    }
    if (mIdentifyCluster.IsConstructed())
    {
        provider.RemoveCluster(&mIdentifyCluster.Cluster());
        mIdentifyCluster.Destroy();
    }
}

} // namespace chip::app
