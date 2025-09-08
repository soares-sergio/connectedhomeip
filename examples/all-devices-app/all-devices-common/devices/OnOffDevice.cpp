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
#include <devices/OnOffDevice.h>

using namespace chip::app::Clusters;

namespace {
class IsDeviceIdendifyingImpl : public IsDeviceIdentifying
{
public:
    bool IsIdentifying() const override { return false; }
};

constexpr uint16_t kOnOffLightDeviceTypeRevision = 3;
constexpr uint16_t kOnOffPlugDeviceTypeRevision  = 4;

} // namespace

namespace chip::app {

DeviceType OnOffDevice::GetDeviceType() const
{
    return mDeviceType;
}

CHIP_ERROR OnOffDevice::Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider, EndpointId parentId)
{
    DeviceTypeId deviceTypeId;
    uint16_t revision;

    switch (mDeviceType)
    {
    case DeviceType::kOnOffLight:
        deviceTypeId = static_cast<DeviceTypeId>(DeviceType::kOnOffLight);
        revision     = kOnOffLightDeviceTypeRevision;
        break;
    case DeviceType::kOnOffPlug:
        deviceTypeId = static_cast<DeviceTypeId>(DeviceType::kOnOffPlug);
        revision     = kOnOffPlugDeviceTypeRevision;
        break;
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    const DescriptorCluster::DeviceType deviceType = { .deviceType = deviceTypeId, .revision = revision };
    ReturnErrorOnFailure(RegisterDescriptor(endpoint, provider, deviceType, parentId));

    mIdentifyCluster.Create(endpoint);
    ReturnErrorOnFailure(provider.AddCluster(mIdentifyCluster.Registration()));

    const OnOffCluster::OptionalAttributeSet optionalAttributeSet;
    mOnOffCluster.Create(endpoint, optionalAttributeSet, BitFlags<OnOff::Feature>(0));
    ReturnErrorOnFailure(provider.AddCluster(mOnOffCluster.Registration()));

    const IsDeviceIdendifyingImpl identifying;
    mGroupsCluster.Create(endpoint, BitFlags<Groups::Feature>(0), identifying);
    ReturnErrorOnFailure(provider.AddCluster(mGroupsCluster.Registration()));

    return provider.AddEndpoint(mEndpointRegistration);
}

void OnOffDevice::UnRegister(CodeDrivenDataModelProvider & provider)
{
    provider.RemoveEndpoint(mEndpointId);
    UnRegisterBridgedNodeClusters(provider);
    if (mOnOffCluster.IsConstructed())
    {
        provider.RemoveCluster(&mOnOffCluster.Cluster());
        mOnOffCluster.Destroy();
    }
    if (mIdentifyCluster.IsConstructed())
    {
        provider.RemoveCluster(&mIdentifyCluster.Cluster());
        mIdentifyCluster.Destroy();
    }
    if (mGroupsCluster.IsConstructed())
    {
        provider.RemoveCluster(&mGroupsCluster.Cluster());
        mGroupsCluster.Destroy();
    }
}

} // namespace chip::app
