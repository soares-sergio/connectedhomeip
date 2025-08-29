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
#include <devices/ContactSensorDevice.h>

using namespace chip::app::Clusters;

namespace chip::app {

constexpr DeviceTypeId kContactSensorDeviceType         = 0x0015;
constexpr DeviceTypeId kContactSensorDeviceTypeRevision = 2;

BridgedDeviceType ContactSensorDevice::GetDeviceType() const
{
    return BridgedDeviceType::kContactSensor;
}

CHIP_ERROR ContactSensorDevice::Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider, EndpointId parentId)
{
    const DescriptorCluster::DeviceType deviceType = { .deviceType = kContactSensorDeviceType,
                                                       .revision   = kContactSensorDeviceTypeRevision };
    ReturnErrorOnFailure(RegisterDescriptor(endpoint, provider, deviceType, parentId));

    mIdentifyCluster.Create(endpoint);
    ReturnErrorOnFailure(provider.AddCluster(mIdentifyCluster.Registration()));

    mBooleanStateCluster.Create(endpoint);
    ReturnErrorOnFailure(provider.AddCluster(mBooleanStateCluster.Registration()));

    return provider.AddEndpoint(mEndpointRegistration);
}

void ContactSensorDevice::UnRegister(CodeDrivenDataModelProvider & provider)
{
    provider.RemoveEndpoint(mEndpointId);
    UnRegisterBridgedNodeClusters(provider);
    if (mBooleanStateCluster.IsConstructed())
    {
        provider.RemoveCluster(&mBooleanStateCluster.Cluster());
        mBooleanStateCluster.Destroy();
    }
    if (mIdentifyCluster.IsConstructed())
    {
        provider.RemoveCluster(&mIdentifyCluster.Cluster());
        mIdentifyCluster.Destroy();
    }
}

}