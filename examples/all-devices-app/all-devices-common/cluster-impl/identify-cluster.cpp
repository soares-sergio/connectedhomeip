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

#include "identify-cluster.h"

#include <app/server-cluster/AttributeListBuilder.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <clusters/Identify/ClusterId.h>
#include <clusters/Identify/Commands.h>
#include <clusters/Identify/Enums.h>
#include <clusters/Identify/Metadata.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::Identify;
using namespace chip::app::Clusters::Identify::Attributes;
using namespace chip::DeviceLayer;
using chip::Protocols::InteractionModel::Status;

namespace chip {
namespace app {
namespace Clusters {

DataModel::ActionReturnStatus IdentifyCluster::ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                             AttributeValueEncoder & encoder)
{
    switch (request.path.mAttributeId)
    {
    case Attributes::FeatureMap::Id:
        return encoder.Encode<uint32_t>(0);
    case Attributes::ClusterRevision::Id:
        return encoder.Encode(Identify::kRevision);
    case Attributes::IdentifyTime::Id:
        // fake: no identify because we refuse
        return encoder.Encode<uint32_t>(0);
    case Attributes::IdentifyType::Id:
        // fake: no identify because we refuse
        return encoder.Encode<IdentifyTypeEnum>(IdentifyTypeEnum::kAudibleBeep);
    default:
        return Status::UnsupportedAttribute;
    }
}

DataModel::ActionReturnStatus IdentifyCluster::WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                              AttributeValueDecoder & decoder)
{
    switch (request.path.mAttributeId)
    {
    case IdentifyTime::Id: {
        uint32_t identifyTime;
        ReturnErrorOnFailure(decoder.Decode(identifyTime));
        ChipLogProgress(AppServer, "Identify time requested: %d ... will do nothing though!", static_cast<int>(identifyTime));
        NotifyAttributeChanged(request.path.mAttributeId);
        return CHIP_NO_ERROR;
    }
    default:
        return CHIP_NO_ERROR;
    }
}

CHIP_ERROR IdentifyCluster::Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    AttributeListBuilder listBuilder(builder);
    return listBuilder.Append(Span(Identify::Attributes::kMandatoryMetadata), {}, {});
}

CHIP_ERROR IdentifyCluster::AcceptedCommands(const ConcreteClusterPath & path,
                                             ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder)
{
    ReturnErrorOnFailure(builder.EnsureAppendCapacity(1));
    ReturnErrorOnFailure(builder.Append(Commands::Identify::kMetadataEntry));
    return CHIP_NO_ERROR;
}

std::optional<DataModel::ActionReturnStatus> IdentifyCluster::InvokeCommand(const DataModel::InvokeRequest & request,
                                                                            chip::TLV::TLVReader & input_arguments,
                                                                            CommandHandler * handler)
{
    switch (request.path.mCommandId)
    {
    case Commands::Identify::Id: {
        Commands::Identify::DecodableType request_data;
        ReturnErrorOnFailure(request_data.Decode(input_arguments));
        ChipLogProgress(AppServer, "Received identify command for %d seconds", static_cast<int>(request_data.identifyTime));
        return CHIP_NO_ERROR;
    }
    default:
        return Protocols::InteractionModel::Status::UnsupportedCommand;
    }
}

} // namespace Clusters
} // namespace app
} // namespace chip
