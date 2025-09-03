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

#include "boolean-state-cluster.h"

#include <app/server-cluster/AttributeListBuilder.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <clusters/BooleanState/ClusterId.h>
#include <clusters/BooleanState/Metadata.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::BooleanState;
using namespace chip::app::Clusters::BooleanState::Attributes;
using namespace chip::DeviceLayer;
using chip::Protocols::InteractionModel::Status;

namespace chip {
namespace app {
namespace Clusters {

void BooleanStateCluster::SetStateValue(bool value)
{
    VerifyOrReturn(value != mStateValue);

    mStateValue = value;
    NotifyAttributeChanged(Attributes::StateValue::Id);
}

DataModel::ActionReturnStatus BooleanStateCluster::ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                                 AttributeValueEncoder & encoder)
{
    VerifyOrDie(request.path.mClusterId == app::Clusters::BooleanState::Id);

    switch (request.path.mAttributeId)
    {
    case Attributes::FeatureMap::Id:
        return encoder.Encode(static_cast<uint32_t>(0));
    case Attributes::ClusterRevision::Id:
        return encoder.Encode(BooleanState::kRevision);
    case Attributes::StateValue::Id: {
        return encoder.Encode(mStateValue);
    }
    default:
        return Status::UnsupportedAttribute;
    }
}

DataModel::ActionReturnStatus BooleanStateCluster::WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                                  AttributeValueDecoder & decoder)
{
    VerifyOrDie(request.path.mClusterId == app::Clusters::BooleanState::Id);

    switch (request.path.mAttributeId)
    {
    case Attributes::StateValue::Id: {
        bool newStateValue;
        ReturnErrorOnFailure(decoder.Decode(newStateValue));
        SetStateValue(newStateValue);
        return CHIP_NO_ERROR;
    }
    default:
        return Status::UnsupportedAttribute;
    }
}

CHIP_ERROR BooleanStateCluster::Attributes(const ConcreteClusterPath & path,
                                           ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    AttributeListBuilder listBuilder(builder);
    return listBuilder.Append(Span(BooleanState::Attributes::kMandatoryMetadata), {}, {});
}

} // namespace Clusters
} // namespace app
} // namespace chip
