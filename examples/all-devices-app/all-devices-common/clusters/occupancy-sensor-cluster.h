/**
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

#include <app-common/zap-generated/cluster-objects.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/OptionalAttributeSet.h>
#include <clusters/OccupancySensing/ClusterId.h>
#include <clusters/OccupancySensing/Metadata.h>
#include <platform/DiagnosticDataProvider.h>

namespace chip {
namespace app {
namespace Clusters {

class OccupancySensingCluster : public DefaultServerCluster
{
public:
    OccupancySensingCluster(BitFlags<OccupancySensing::Feature> features) :
        DefaultServerCluster({ kRootEndpointId, OccupancySensing::Id }), mFeatures(features)
    {}

    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                AttributeValueEncoder & encoder) override;

    DataModel::ActionReturnStatus WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                 AttributeValueDecoder & decoder) override;

    CHIP_ERROR Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;

private:
    CHIP_ERROR SetHoldTime(EndpointId endpointId, uint16_t newHoldTime);

    BitFlags<OccupancySensing::Feature> mFeatures;
    uint16_t mHoldTime;
    OccupancySensing::Structs::HoldTimeLimitsStruct::Type mHoldTimeLimits;
};

} // namespace Clusters
} // namespace app
} // namespace chip
