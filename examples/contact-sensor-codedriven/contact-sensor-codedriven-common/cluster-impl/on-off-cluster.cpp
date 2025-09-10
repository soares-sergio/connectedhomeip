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

#include "on-off-cluster.h"
#include <app-common/zap-generated/cluster-objects.h>
#include <app/server-cluster/AttributeListBuilder.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <clusters/OnOff/ClusterId.h>
#include <clusters/OnOff/Metadata.h>

using namespace chip;
using namespace chip::app;
using namespace chip::Credentials;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OnOff;
using namespace chip::app::Clusters::OnOff::Attributes;
using namespace chip::DeviceLayer;
using chip::Protocols::InteractionModel::Status;

namespace chip {
namespace app {
namespace Clusters {

void OnOffCluster::SetOnOff(bool value)
{
    VerifyOrReturn(value != mOnOff);

    mOnOff = value;
    NotifyAttributeChanged(Attributes::OnOff::Id);
}

std::optional<DataModel::ActionReturnStatus> OnOffCluster::InvokeCommand(const DataModel::InvokeRequest & request,
                                                                         chip::TLV::TLVReader & input_arguments,
                                                                         CommandHandler * handler)
{
    switch (request.path.mCommandId)
    {
    case OnOff::Commands::Off::Id: {
        OnOff::Commands::Off::DecodableType request_data;
        ReturnErrorOnFailure(request_data.Decode(input_arguments));
        SetOnOff(false);
        return CHIP_NO_ERROR;
    }
    case OnOff::Commands::On::Id: {
        if (mFeatureFlags.Has(Feature::kOffOnly))
        {
            return Protocols::InteractionModel::Status::UnsupportedCommand;
        }
        OnOff::Commands::On::DecodableType request_data;
        ReturnErrorOnFailure(request_data.Decode(input_arguments));
        SetOnOff(true);
        return CHIP_NO_ERROR;
    }
    case OnOff::Commands::Toggle::Id: {
        if (mFeatureFlags.Has(Feature::kOffOnly))
        {
            return Protocols::InteractionModel::Status::UnsupportedCommand;
        }
        OnOff::Commands::Toggle::DecodableType request_data;
        SetOnOff(!mOnOff);
        ReturnErrorOnFailure(request_data.Decode(input_arguments));
        return CHIP_NO_ERROR;
    }
    default:
        return Protocols::InteractionModel::Status::UnsupportedCommand;
    }
}

CHIP_ERROR OnOffCluster::Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    AttributeListBuilder listBuilder(builder);

    static constexpr DataModel::AttributeEntry optionalAttributeEntries[] = {
        OnOff::Attributes::GlobalSceneControl::kMetadataEntry,
        OnOff::Attributes::OnTime::kMetadataEntry,
        OnOff::Attributes::OffWaitTime::kMetadataEntry,
        OnOff::Attributes::StartUpOnOff::kMetadataEntry,
    };

    return listBuilder.Append(Span(OnOff::Attributes::kMandatoryMetadata), Span(optionalAttributeEntries), mOptionalAttributeSet);
}

DataModel::ActionReturnStatus OnOffCluster::ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                          AttributeValueEncoder & encoder)
{
    switch (request.path.mAttributeId)
    {
    case GroupKeyManagement::Attributes::ClusterRevision::Id:
        return encoder.Encode(OnOff::kRevision);
    case Attributes::FeatureMap::Id:
        return encoder.Encode(mFeatureFlags);
    case Attributes::GlobalSceneControl::Id: {
        if (!mFeatureFlags.Has(Feature::kLighting))
        {
            return Protocols::InteractionModel::Status::UnsupportedAttribute;
        }
        return encoder.Encode(mGlobalSceneControl);
    }
    case Attributes::OnTime::Id: {
        if (!mFeatureFlags.Has(Feature::kLighting))
        {
            return Protocols::InteractionModel::Status::UnsupportedAttribute;
        }
        return encoder.Encode(mOnTime);
    }
    case Attributes::OffWaitTime::Id: {
        if (!mFeatureFlags.Has(Feature::kLighting))
        {
            return Protocols::InteractionModel::Status::UnsupportedAttribute;
        }
        return encoder.Encode(mOffWaitTime);
    }
    case Attributes::StartUpOnOff::Id: {
        if (!mFeatureFlags.Has(Feature::kLighting))
        {
            return Protocols::InteractionModel::Status::UnsupportedAttribute;
        }
        return encoder.Encode(mStartUpOnOff);
    }
    case Attributes::OnOff::Id: {
        return encoder.Encode(mOnOff);
    }
    default:
        return Protocols::InteractionModel::Status::UnsupportedAttribute;
    }
}

DataModel::ActionReturnStatus OnOffCluster::WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                           AttributeValueDecoder & decoder)
{
    VerifyOrDie(request.path.mClusterId == app::Clusters::OnOff::Id);

    // All the attributes below currently require this flag to be enabled
    if (!mFeatureFlags.Has(Feature::kLighting))
    {
        return Protocols::InteractionModel::Status::UnsupportedWrite;
    }

    switch (request.path.mAttributeId)
    {
    case Attributes::OnTime::Id: {
        uint16_t newOnTime;
        ReturnErrorOnFailure(decoder.Decode(newOnTime));
        mOnTime = newOnTime;
        NotifyAttributeChanged(request.path.mAttributeId);
        return CHIP_NO_ERROR;
    }
    case Attributes::OffWaitTime::Id: {
        uint16_t newOffWaitTime;
        ReturnErrorOnFailure(decoder.Decode(newOffWaitTime));
        mOffWaitTime = newOffWaitTime;
        NotifyAttributeChanged(request.path.mAttributeId);
        return CHIP_NO_ERROR;
    }
    case Attributes::StartUpOnOff::Id:
        OnOff::StartUpOnOffEnum newStartUpOnOff;
        ReturnErrorOnFailure(decoder.Decode(newStartUpOnOff));
        mStartUpOnOff = newStartUpOnOff;
        NotifyAttributeChanged(request.path.mAttributeId);
        return CHIP_NO_ERROR;
    default:
        return Protocols::InteractionModel::Status::UnsupportedWrite;
    }
}

CHIP_ERROR OnOffCluster::AcceptedCommands(const ConcreteClusterPath & path,
                                          ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder)
{
    static constexpr DataModel::AcceptedCommandEntry kAcceptedCommands[] = {
        Commands::Off::kMetadataEntry,
        Commands::On::kMetadataEntry,
        Commands::Toggle::kMetadataEntry,
        Commands::OffWithEffect::kMetadataEntry,
        Commands::OnWithRecallGlobalScene::kMetadataEntry,
        Commands::OnWithTimedOff::kMetadataEntry,
    };
    return builder.ReferenceExisting(kAcceptedCommands);
}
} // namespace Clusters
} // namespace app
} // namespace chip
