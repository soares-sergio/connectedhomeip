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

#include "groups-cluster.h"
#include <app-common/zap-generated/cluster-objects.h>
#include <app/reporting/reporting.h>
#include <app/server-cluster/AttributeListBuilder.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <clusters/Groups/ClusterId.h>
#include <clusters/Groups/Metadata.h>
#include <credentials/GroupDataProvider.h>
#include <tracing/macros.h>

using namespace chip;
using namespace chip::app;
using namespace chip::Credentials;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Groups;
using namespace chip::app::Clusters::Groups::Attributes;
using namespace chip::DeviceLayer;
using namespace chip::Credentials;
using chip::Protocols::InteractionModel::Status;

namespace {
struct GroupMembershipResponse
{
    // A null capacity means that it is unknown if any further groups MAY be added.
    const chip::app::DataModel::Nullable<uint8_t> kCapacityUnknown;

    // Use GetCommandId instead of commandId directly to avoid naming conflict with CommandIdentification in ExecutionOfACommand
    static constexpr CommandId GetCommandId() { return Commands::GetGroupMembershipResponse::Id; }
    static constexpr ClusterId GetClusterId() { return Groups::Id; }

    GroupMembershipResponse(const Commands::GetGroupMembership::DecodableType & data, chip::EndpointId endpoint,
                            GroupDataProvider::EndpointIterator * iter) :
        mCommandData(data),
        mEndpoint(endpoint), mIterator(iter)
    {}

    const Commands::GetGroupMembership::DecodableType & mCommandData;
    chip::EndpointId mEndpoint                      = kInvalidEndpointId;
    GroupDataProvider::EndpointIterator * mIterator = nullptr;

    CHIP_ERROR Encode(TLV::TLVWriter & writer, TLV::Tag tag) const
    {
        TLV::TLVType outer;

        ReturnErrorOnFailure(writer.StartContainer(tag, TLV::kTLVType_Structure, outer));

        ReturnErrorOnFailure(app::DataModel::Encode(
            writer, TLV::ContextTag(Commands::GetGroupMembershipResponse::Fields::kCapacity), kCapacityUnknown));
        {
            TLV::TLVType type;
            ReturnErrorOnFailure(writer.StartContainer(TLV::ContextTag(Commands::GetGroupMembershipResponse::Fields::kGroupList),
                                                       TLV::kTLVType_Array, type));
            {
                GroupDataProvider::GroupEndpoint mapping;
                size_t requestedCount = 0;
                ReturnErrorOnFailure(mCommandData.groupList.ComputeSize(&requestedCount));

                if (0 == requestedCount)
                {
                    // 1.3.6.3.1. If the GroupList field is empty, the entity SHALL respond with all group identifiers of which the
                    // entity is a member.
                    while (mIterator && mIterator->Next(mapping))
                    {
                        if (mapping.endpoint_id == mEndpoint)
                        {
                            ReturnErrorOnFailure(app::DataModel::Encode(writer, TLV::AnonymousTag(), mapping.group_id));
                            ChipLogDetail(Zcl, " 0x%02x", mapping.group_id);
                        }
                    }
                }
                else
                {
                    while (mIterator && mIterator->Next(mapping))
                    {
                        auto iter = mCommandData.groupList.begin();
                        while (iter.Next())
                        {
                            if (mapping.endpoint_id == mEndpoint && mapping.group_id == iter.GetValue())
                            {
                                ReturnErrorOnFailure(app::DataModel::Encode(writer, TLV::AnonymousTag(), mapping.group_id));
                                ChipLogDetail(Zcl, " 0x%02x", mapping.group_id);
                                break;
                            }
                        }
                        ReturnErrorOnFailure(iter.GetStatus());
                    }
                }
                ChipLogDetail(Zcl, "]");
            }
            ReturnErrorOnFailure(writer.EndContainer(type));
        }
        ReturnErrorOnFailure(writer.EndContainer(outer));
        return CHIP_NO_ERROR;
    }
};

/**
 * @brief Checks if there are key set associated with the given GroupId
 */
static bool KeyExists(FabricIndex fabricIndex, GroupId groupId)
{
    GroupDataProvider * provider = GetGroupDataProvider();
    VerifyOrReturnError(nullptr != provider, false);
    GroupDataProvider::GroupKey entry;

    auto it    = provider->IterateGroupKeys(fabricIndex);
    bool found = false;
    while (!found && it->Next(entry))
    {
        if (entry.group_id == groupId)
        {
            GroupDataProvider::KeySet keys;
            found = (CHIP_NO_ERROR == provider->GetKeySet(fabricIndex, entry.keyset_id, keys));
        }
    }
    it->Release();

    return found;
}

static Status GroupAdd(FabricIndex fabricIndex, EndpointId endpointId, GroupId groupId, const CharSpan & groupName)
{
    VerifyOrReturnError(IsValidGroupId(groupId), Status::ConstraintError);
    VerifyOrReturnError(groupName.size() <= GroupDataProvider::GroupInfo::kGroupNameMax, Status::ConstraintError);

    GroupDataProvider * provider = GetGroupDataProvider();
    VerifyOrReturnError(nullptr != provider, Status::NotFound);
    VerifyOrReturnError(KeyExists(fabricIndex, groupId), Status::UnsupportedAccess);

    // Add a new entry to the GroupTable
    CHIP_ERROR err = provider->SetGroupInfo(fabricIndex, GroupDataProvider::GroupInfo(groupId, groupName));
    if (CHIP_NO_ERROR == err)
    {
        err = provider->AddEndpoint(fabricIndex, groupId, endpointId);
    }
    if (CHIP_NO_ERROR == err)
    {
        MatterReportingAttributeChangeCallback(kRootEndpointId, GroupKeyManagement::Id,
                                               GroupKeyManagement::Attributes::GroupTable::Id);
        return Status::Success;
    }

    ChipLogDetail(Zcl, "ERR: Failed to add mapping (end:%d, group:0x%x), err:%" CHIP_ERROR_FORMAT, endpointId, groupId,
                  err.Format());
    return Status::ResourceExhausted;
}

std::optional<DataModel::ActionReturnStatus> HandleAddGroup(app::CommandHandler * commandObj,
                                                            const app::ConcreteCommandPath & commandPath,
                                                            const Commands::AddGroup::DecodableType & commandData)
{
    MATTER_TRACE_SCOPE("AddGroup", "Groups");
    auto fabricIndex = commandObj->GetAccessingFabricIndex();
    Groups::Commands::AddGroupResponse::Type response;

    response.groupID = commandData.groupID;
    response.status  = to_underlying(GroupAdd(fabricIndex, commandPath.mEndpointId, commandData.groupID, commandData.groupName));
    commandObj->AddResponse(commandPath, response);
    return Status::Success;
}

std::optional<DataModel::ActionReturnStatus> HandleViewGroup(app::CommandHandler * commandObj,
                                                             const app::ConcreteCommandPath & commandPath,
                                                             const Commands::ViewGroup::DecodableType & commandData)
{
    MATTER_TRACE_SCOPE("ViewGroup", "Groups");
    auto fabricIndex             = commandObj->GetAccessingFabricIndex();
    auto groupId                 = commandData.groupID;
    GroupDataProvider * provider = GetGroupDataProvider();
    GroupDataProvider::GroupInfo info;
    Groups::Commands::ViewGroupResponse::Type response;
    CHIP_ERROR err = CHIP_NO_ERROR;
    Status status  = Status::NotFound;

    VerifyOrExit(IsValidGroupId(groupId), status = Status::ConstraintError);
    VerifyOrExit(nullptr != provider, status = Status::Failure);
    VerifyOrExit(provider->HasEndpoint(fabricIndex, groupId, commandPath.mEndpointId), status = Status::NotFound);

    err = provider->GetGroupInfo(fabricIndex, groupId, info);
    VerifyOrExit(CHIP_NO_ERROR == err, status = Status::NotFound);

    response.groupName = CharSpan(info.name, strnlen(info.name, GroupDataProvider::GroupInfo::kGroupNameMax));
    status             = Status::Success;
exit:
    response.groupID = groupId;
    response.status  = to_underlying(status);
    commandObj->AddResponse(commandPath, response);
    return status;
}

std::optional<DataModel::ActionReturnStatus>
HandleGetGroupMembership(app::CommandHandler * commandObj, const app::ConcreteCommandPath & commandPath,
                         const Commands::GetGroupMembership::DecodableType & commandData)
{
    MATTER_TRACE_SCOPE("GetGroupMembership", "Groups");
    auto fabricIndex = commandObj->GetAccessingFabricIndex();
    auto * provider  = GetGroupDataProvider();
    Status status    = Status::Failure;

    VerifyOrExit(nullptr != provider, status = Status::Failure);

    {
        GroupDataProvider::EndpointIterator * iter = nullptr;

        iter = provider->IterateEndpoints(fabricIndex);
        VerifyOrExit(nullptr != iter, status = Status::Failure);

        commandObj->AddResponse(commandPath, GroupMembershipResponse(commandData, commandPath.mEndpointId, iter));
        iter->Release();
        status = Status::Success;
    }

exit:
    if (Status::Success != status)
    {
        ChipLogDetail(Zcl, "GroupsCluster: GetGroupMembership failed: failed: 0x%x", to_underlying(status));
        commandObj->AddStatus(commandPath, status);
    }
    return status;
}

/**
 * @brief Checks if group-endpoint association exist for the given fabric
 */
static bool GroupExists(FabricIndex fabricIndex, EndpointId endpointId, GroupId groupId)
{
    GroupDataProvider * provider = GetGroupDataProvider();
    VerifyOrReturnError(nullptr != provider, false);

    return provider->HasEndpoint(fabricIndex, groupId, endpointId);
}

static Status GroupRemove(FabricIndex fabricIndex, EndpointId endpointId, GroupId groupId)
{
    VerifyOrReturnError(IsValidGroupId(groupId), Status::ConstraintError);
    VerifyOrReturnError(GroupExists(fabricIndex, endpointId, groupId), Status::NotFound);

    GroupDataProvider * provider = GetGroupDataProvider();
    VerifyOrReturnError(nullptr != provider, Status::NotFound);

    CHIP_ERROR err = provider->RemoveEndpoint(fabricIndex, groupId, endpointId);
    if (CHIP_NO_ERROR == err)
    {
        MatterReportingAttributeChangeCallback(kRootEndpointId, GroupKeyManagement::Id,
                                               GroupKeyManagement::Attributes::GroupTable::Id);
        return Status::Success;
    }

    ChipLogDetail(Zcl, "ERR: Failed to remove mapping (end:%d, group:0x%x), err:%" CHIP_ERROR_FORMAT, endpointId, groupId,
                  err.Format());
    return Status::NotFound;
}

std::optional<DataModel::ActionReturnStatus> HandleRemoveGroup(app::CommandHandler * commandObj,
                                                               const app::ConcreteCommandPath & commandPath,
                                                               const Commands::RemoveGroup::DecodableType & commandData)
{
    MATTER_TRACE_SCOPE("RemoveGroup", "Groups");
    auto fabricIndex = commandObj->GetAccessingFabricIndex();
    Groups::Commands::RemoveGroupResponse::Type response;

#ifdef MATTER_DM_PLUGIN_SCENES_MANAGEMENT
    // If a group is removed the scenes associated with that group SHOULD be removed.
    ScenesManagement::ScenesServer::Instance().GroupWillBeRemoved(fabricIndex, commandPath.mEndpointId, commandData.groupID);
#endif
    response.groupID = commandData.groupID;
    response.status  = to_underlying(GroupRemove(fabricIndex, commandPath.mEndpointId, commandData.groupID));

    commandObj->AddResponse(commandPath, response);
    return Status::Success;
}

std::optional<DataModel::ActionReturnStatus> HandleRemoveAllGroups(app::CommandHandler * commandObj,
                                                                   const app::ConcreteCommandPath & commandPath,
                                                                   const Commands::RemoveAllGroups::DecodableType & commandData)
{
    MATTER_TRACE_SCOPE("RemoveAllGroups", "Groups");
    auto fabricIndex = commandObj->GetAccessingFabricIndex();
    auto * provider  = GetGroupDataProvider();
    Status status    = Status::Failure;

    VerifyOrExit(nullptr != provider, status = Status::Failure);

#ifdef MATTER_DM_PLUGIN_SCENES_MANAGEMENT
    {
        GroupDataProvider::EndpointIterator * iter = provider->IterateEndpoints(fabricIndex);
        GroupDataProvider::GroupEndpoint mapping;

        VerifyOrExit(nullptr != iter, status = Status::Failure);
        while (iter->Next(mapping))
        {
            if (commandPath.mEndpointId == mapping.endpoint_id)
            {
                ScenesManagement::ScenesServer::Instance().GroupWillBeRemoved(fabricIndex, mapping.endpoint_id, mapping.group_id);
            }
        }
        iter->Release();
        ScenesManagement::ScenesServer::Instance().GroupWillBeRemoved(fabricIndex, commandPath.mEndpointId,
                                                                      ScenesManagement::ScenesServer::kGlobalSceneGroupId);
    }
#endif

    provider->RemoveEndpoint(fabricIndex, commandPath.mEndpointId);
    status = Status::Success;
    MatterReportingAttributeChangeCallback(kRootEndpointId, GroupKeyManagement::Id, GroupKeyManagement::Attributes::GroupTable::Id);
exit:
    commandObj->AddStatus(commandPath, status);
    if (Status::Success != status)
    {
        ChipLogDetail(Zcl, "GroupsCluster: RemoveAllGroups failed: 0x%x", to_underlying(status));
    }
    return status;
}

std::optional<DataModel::ActionReturnStatus>
HandleAddGroupIfIdentifying(app::CommandHandler * commandObj, const app::ConcreteCommandPath & commandPath,
                            const Commands::AddGroupIfIdentifying::DecodableType & commandData, bool isIdentifying)
{
    MATTER_TRACE_SCOPE("AddGroupIfIdentifying", "Groups");
    auto fabricIndex = commandObj->GetAccessingFabricIndex();
    auto groupId     = commandData.groupID;
    auto groupName   = commandData.groupName;
    auto endpointId  = commandPath.mEndpointId;

    Status status;

    // TODO: Ensure the delegagte to check for is the device is identifying is not a hardcoded implementation
    if (!isIdentifying)
    {
        // If not identifying, ignore add group -> success; not a failure.
        status = Status::Success;
    }
    else
    {
        status = GroupAdd(fabricIndex, endpointId, groupId, groupName);
    }
    status = GroupAdd(fabricIndex, endpointId, groupId, groupName);

    commandObj->AddStatus(commandPath, status);
    return status;
}
} // namespace

namespace chip {
namespace app {
namespace Clusters {

std::optional<DataModel::ActionReturnStatus> GroupsCluster::InvokeCommand(const DataModel::InvokeRequest & request,
                                                                          chip::TLV::TLVReader & input_arguments,
                                                                          CommandHandler * handler)
{
    switch (request.path.mCommandId)
    {
    case Groups::Commands::AddGroup::Id: {
        Groups::Commands::AddGroup::DecodableType request_data;
        ReturnErrorOnFailure(request_data.Decode(input_arguments, handler->GetAccessingFabricIndex()));
        return HandleAddGroup(handler, request.path, request_data);
    }
    case Groups::Commands::ViewGroup::Id: {
        Groups::Commands::ViewGroup::DecodableType request_data;
        ReturnErrorOnFailure(request_data.Decode(input_arguments, handler->GetAccessingFabricIndex()));
        return HandleViewGroup(handler, request.path, request_data);
    }
    case Groups::Commands::GetGroupMembership::Id: {
        Groups::Commands::GetGroupMembership::DecodableType request_data;
        ReturnErrorOnFailure(request_data.Decode(input_arguments, handler->GetAccessingFabricIndex()));
        return HandleGetGroupMembership(handler, request.path, request_data);
    }
    case Groups::Commands::RemoveGroup::Id: {
        Groups::Commands::RemoveGroup::DecodableType request_data;
        ReturnErrorOnFailure(request_data.Decode(input_arguments, handler->GetAccessingFabricIndex()));
        return HandleRemoveGroup(handler, request.path, request_data);
    }
    case Groups::Commands::RemoveAllGroups::Id: {
        Groups::Commands::RemoveAllGroups::DecodableType request_data;
        ReturnErrorOnFailure(request_data.Decode(input_arguments, handler->GetAccessingFabricIndex()));
        return HandleRemoveAllGroups(handler, request.path, request_data);
    }
    case Groups::Commands::AddGroupIfIdentifying::Id: {
        Groups::Commands::AddGroupIfIdentifying::DecodableType request_data;
        ReturnErrorOnFailure(request_data.Decode(input_arguments, handler->GetAccessingFabricIndex()));
        return HandleAddGroupIfIdentifying(handler, request.path, request_data, isIdentifyingBool);
    }
    default:
        return Protocols::InteractionModel::Status::UnsupportedCommand;
    }
}

DataModel::ActionReturnStatus GroupsCluster::ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                           AttributeValueEncoder & encoder)
{
    switch (request.path.mAttributeId)
    {
    case Groups::Attributes::FeatureMap::Id:
        return encoder.Encode(mFeatureFlags);
    case Groups::Attributes::ClusterRevision::Id:
        return encoder.Encode(Groups::kRevision);
    case Groups::Attributes::NameSupport::Id:
        return encoder.Encode(mNameSupport);
    default:
        return Protocols::InteractionModel::Status::UnsupportedAttribute;
    }
}

CHIP_ERROR GroupsCluster::Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    AttributeListBuilder listBuilder(builder);
    return listBuilder.Append(Span(Groups::Attributes::kMandatoryMetadata), {});
}

CHIP_ERROR GroupsCluster::AcceptedCommands(const ConcreteClusterPath & path,
                                           ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder)
{
    static constexpr DataModel::AcceptedCommandEntry kAcceptedCommands[] = {
        Commands::AddGroup::kMetadataEntry,           Commands::ViewGroup::kMetadataEntry,
        Commands::GetGroupMembership::kMetadataEntry, Commands::RemoveGroup::kMetadataEntry,
        Commands::RemoveAllGroups::kMetadataEntry,    Commands::AddGroupIfIdentifying::kMetadataEntry,
    };
    return builder.ReferenceExisting(kAcceptedCommands);
}

CHIP_ERROR GroupsCluster::GeneratedCommands(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<CommandId> & builder)
{
    static constexpr CommandId kGeneratedCommands[] = {
        Commands::AddGroupResponse::Id,
        Commands::ViewGroupResponse::Id,
        Commands::GetGroupMembershipResponse::Id,
        Commands::RemoveGroupResponse::Id,
    };
    return builder.ReferenceExisting(kGeneratedCommands);
}

} // namespace Clusters
} // namespace app
} // namespace chip
