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

#include "occupancy-sensor-cluster.h"

#include <app/server-cluster/AttributeListBuilder.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <clusters/OccupancySensing/ClusterId.h>
#include <clusters/OccupancySensing/Metadata.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::OccupancySensing;
using namespace chip::app::Clusters::OccupancySensing::Attributes;
using namespace chip::DeviceLayer;
using chip::Protocols::InteractionModel::Status;

namespace chip {
namespace app {
namespace Clusters {

DataModel::ActionReturnStatus OccupancySensingCluster::ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                                     AttributeValueEncoder & encoder)
{
    VerifyOrDie(request.path.mClusterId == app::Clusters::OccupancySensing::Id);

    switch (request.path.mAttributeId)
    {
    case Attributes::FeatureMap::Id:
        return encoder.Encode(mFeatures);
    case Attributes::ClusterRevision::Id:
        return encoder.Encode(OccupancySensing::kRevision);
    case Attributes::Occupancy::Id: {
        BitFlags<OccupancyBitmap> state;
        state.Set(OccupancyBitmap::kOccupied, true);
        return encoder.Encode(state);
    }
    case Attributes::OccupancySensorType::Id: {
        const OccupancySensing::OccupancySensorTypeEnum type = OccupancySensing::OccupancySensorTypeEnum::kPir;
        return encoder.Encode(type);
     }
    case Attributes::OccupancySensorTypeBitmap::Id: {
        BitFlags<OccupancySensing::OccupancySensorTypeBitmap> state;
        state.Set(OccupancySensing::OccupancySensorTypeBitmap::kPir, true);
        return encoder.Encode(state);
    }
    case Attributes::HoldTime::Id:
    case Attributes::PIROccupiedToUnoccupiedDelay::Id:
    case Attributes::UltrasonicOccupiedToUnoccupiedDelay::Id:
    case Attributes::PhysicalContactOccupiedToUnoccupiedDelay::Id: {
        // HoldTime is equivalent to the legacy *OccupiedToUnoccupiedDelay attributes.
        // The AAI will read/write these attributes at the same storage for one endpoint.
        // TODO add proper get for hold time
        return encoder.Encode(mHoldTime);
    }
    case Attributes::HoldTimeLimits::Id: {
        // TODO add proper get for hold time limit struct
        return encoder.Encode(mHoldTimeLimits);
    }
    default:
        return Status::UnsupportedAttribute;
    }
}

DataModel::ActionReturnStatus OccupancySensingCluster::WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                                      AttributeValueDecoder & decoder)
{
    VerifyOrDie(request.path.mClusterId == app::Clusters::OccupancySensing::Id);

    switch (request.path.mAttributeId)
    {
    case Attributes::HoldTime::Id:
    case Attributes::PIROccupiedToUnoccupiedDelay::Id:
    case Attributes::UltrasonicOccupiedToUnoccupiedDelay::Id:
    case Attributes::PhysicalContactOccupiedToUnoccupiedDelay::Id: {
        uint16_t newHoldTime;
        ReturnErrorOnFailure(decoder.Decode(newHoldTime));

        // TODO take hold limits into account before setting the hold time.
        return NotifyAttributeChangedIfSuccess(request.path.mAttributeId, SetHoldTime(request.path.mEndpointId, newHoldTime));
    }
    default:
        return CHIP_NO_ERROR;
    }
}

CHIP_ERROR OccupancySensingCluster::Attributes(const ConcreteClusterPath & path,
                                               ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    AttributeListBuilder listBuilder(builder);
    return listBuilder.Append(Span(OccupancySensing::Attributes::kMandatoryMetadata), {}, {});
}

CHIP_ERROR OccupancySensingCluster::SetHoldTime(EndpointId endpointId, uint16_t newHoldTime)
{
    VerifyOrReturnError(kInvalidEndpointId != endpointId, CHIP_ERROR_INVALID_ARGUMENT);
    mHoldTime = newHoldTime;
    return CHIP_NO_ERROR;
}

} // namespace Clusters
} // namespace app
} // namespace chip
