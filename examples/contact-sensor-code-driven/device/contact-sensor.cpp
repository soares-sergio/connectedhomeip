/**
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

#include "contact-sensor.h"

using namespace chip::app::Clusters;

namespace chip::app {

ContactSensor::ContactSensor(EndpointId parentEndpointId, EndpointId endpointId) :
    mRegistration(*this,
                  {
                      .id                 = endpointId,
                      .parentId           = parentEndpointId,
                      .compositionPattern = DataModel::EndpointCompositionPattern::kFullFamily,
                  }),                 //
    mBooleanStateCluster(endpointId), //
    mIdentifyCluster(endpointId)
{}

CHIP_ERROR ContactSensor::Register(CodeDrivenDataModelProvider & dataModelProvider)
{
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mBooleanStateCluster.Registration()));
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mIdentifyCluster.Registration()));

    return dataModelProvider.AddEndpoint(mRegistration);
}

CHIP_ERROR ContactSensor::SemanticTags(ReadOnlyBufferBuilder<SemanticTag> & out) const
{
    return CHIP_NO_ERROR;
}

CHIP_ERROR ContactSensor::DeviceTypes(ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out) const
{
    constexpr uint16_t kContactSensorDeviceType                 = 0x0015;
    constexpr uint16_t kContactSensorDeviceVersion              = 2;
    constexpr DataModel::DeviceTypeEntry kContactSensorDevice[] = { { .deviceTypeId       = kContactSensorDeviceType,
                                                                      .deviceTypeRevision = kContactSensorDeviceVersion } };
    return out.ReferenceExisting(kContactSensorDevice);
}

CHIP_ERROR ContactSensor::ClientClusters(ReadOnlyBufferBuilder<ClusterId> & out) const
{
    return CHIP_NO_ERROR;
}

} // namespace chip::app
