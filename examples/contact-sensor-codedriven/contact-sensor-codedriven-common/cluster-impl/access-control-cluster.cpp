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

#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
#include "ArlEncoder.h"
#include <access/AccessRestrictionProvider.h>
#endif

#include "access-control-cluster.h"
#include <app-common/zap-generated/cluster-objects.h>
#include <app/server-cluster/AttributeListBuilder.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server/Server.h>

#include <app/AttributeAccessInterface.h>
#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/CommandHandler.h>
#include <app/ConcreteCommandPath.h>
#include <app/EventLogging.h>
#include <app/data-model/Encode.h>
#include <app/reporting/reporting.h>
#include <app/server/AclStorage.h>
#include <app/server/Server.h>
// #include <app/util/attribute-storage.h>

#include <clusters/AccessControl/ClusterId.h>
#include <clusters/AccessControl/Metadata.h>
#include <lib/support/TypeTraits.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::AccessControl;
using namespace chip::app::Clusters::AccessControl::Attributes;
using namespace chip::DeviceLayer;
using namespace chip::Access;

using AclEvent       = Clusters::AccessControl::Events::AccessControlEntryChanged::Type;
using ChangeTypeEnum = Clusters::AccessControl::ChangeTypeEnum;
using Entry          = AccessControl::Entry;
using EntryListener  = AccessControl::EntryListener;

namespace chip {
namespace app {
namespace Clusters {

DataModel::ActionReturnStatus AccessControlCluster::ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                                  AttributeValueEncoder & encoder)
{
    switch (request.path.mAttributeId)
    {
    case AccessControl::Attributes::Acl::Id:
        return ReadAcl(encoder);
#if CHIP_CONFIG_ENABLE_ACL_EXTENSIONS
    case AccessControl::Attributes::Extension::Id:
        return ReadExtension(encoder);
#endif // CHIP_CONFIG_ENABLE_ACL_EXTENSIONS
    case AccessControl::Attributes::SubjectsPerAccessControlEntry::Id: {
        size_t value = 0;
        ReturnErrorOnFailure(chip::Access::GetAccessControl().GetMaxSubjectsPerEntry(value));
        return encoder.Encode(static_cast<uint16_t>(value));
    }
    case AccessControl::Attributes::TargetsPerAccessControlEntry::Id: {
        size_t value = 0;
        ReturnErrorOnFailure(chip::Access::GetAccessControl().GetMaxTargetsPerEntry(value));
        return encoder.Encode(static_cast<uint16_t>(value));
    }
    case AccessControl::Attributes::AccessControlEntriesPerFabric::Id: {
        size_t value = 0;
        ReturnErrorOnFailure(chip::Access::GetAccessControl().GetMaxEntriesPerFabric(value));
        return encoder.Encode(static_cast<uint16_t>(value));
    }
#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    case AccessControl::Attributes::CommissioningARL::Id:
        return ReadCommissioningArl(encoder);
    case AccessControl::Attributes::Arl::Id:
        return ReadArl(encoder);
#endif
    case AccessControl::Attributes::FeatureMap::Id: {
        uint32_t featureMap = 0;

#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
        featureMap |= to_underlying(AccessControlCluster::Feature::kManagedDevice);
#endif // CHIP_CONFIG_USE_ACCESS_RESTRICTIONS

#if CHIP_CONFIG_ENABLE_ACL_EXTENSIONS
        featureMap |= to_underlying(AccessControlCluster::Feature::kExtension);
#endif // CHIP_CONFIG_ENABLE_ACL_EXTENSIONS

        return encoder.Encode(featureMap);
    }
    case AccessControl::Attributes::ClusterRevision::Id:
        return encoder.Encode(AccessControl::kRevision);
    default:
        return CHIP_NO_ERROR;
    }
}

DataModel::ActionReturnStatus AccessControlCluster::WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                                   AttributeValueDecoder & decoder)
{
    switch (request.path.mAttributeId)
    {
    case Acl::Id: {
        return NotifyAttributeChangedIfSuccess(request.path.mAttributeId, WriteAcl(request.path, decoder));
    }
#if CHIP_CONFIG_ENABLE_ACL_EXTENSIONS
    case Extension::Id: {
        return NotifyAttributeChangedIfSuccess(request.path.mAttributeId, WriteExtension(request.path, decoder));
    }
#endif
    default:
        return Protocols::InteractionModel::Status::UnsupportedWrite;
    }
}

std::optional<DataModel::ActionReturnStatus> AccessControlCluster::InvokeCommand(const DataModel::InvokeRequest & request,
                                                                                 chip::TLV::TLVReader & input_arguments,
                                                                                 CommandHandler * handler)
{
    switch (request.path.mCommandId)
    {
#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    case AccessControl::Commands::ReviewFabricRestrictions::Id: {
        AccessControl::Commands::ReviewFabricRestrictions::DecodableType data;
        ReturnErrorOnFailure(data.Decode(input_arguments, handler->GetAccessingFabricIndex()));
        return HandleReviewFabricRestrictions(handler, request.path, data);
    }
#endif
    default:
        return Protocols::InteractionModel::Status::UnsupportedCommand;
    }
}

CHIP_ERROR AccessControlCluster::Attributes(const ConcreteClusterPath & path,
                                            ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    AttributeListBuilder listBuilder(builder);

    static constexpr DataModel::AttributeEntry optionalEntries[] = {
        Attributes::Extension::kMetadataEntry,
        Attributes::CommissioningARL::kMetadataEntry,
        Attributes::Arl::kMetadataEntry,
    };

    return listBuilder.Append(Span(AccessControl::Attributes::kMandatoryMetadata), Span(optionalEntries), mOptionalAttributeSet);
}

CHIP_ERROR AccessControlCluster::AcceptedCommands(const ConcreteClusterPath & path,
                                                  ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder)
{
#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    static constexpr DataModel::AcceptedCommandEntry kAcceptedCommands[] = {
        Commands::ReviewFabricRestrictions::kMetadataEntry,
    } return builder.ReferenceExisting(kAcceptedCommands);
#else
    return CHIP_NO_ERROR;
#endif
}

CHIP_ERROR AccessControlCluster::GeneratedCommands(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<CommandId> & builder)
{
#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    static constexpr CommandId kGeneratedCommands[] = {
        Commands::ReviewFabricRestrictionsResponse,
    };
    return builder.ReferenceExisting(kGeneratedCommands);
#else
    return CHIP_NO_ERROR;
#endif
}

CHIP_ERROR AccessControlCluster::ReadAcl(AttributeValueEncoder & aEncoder)
{
    chip::Access::AccessControl::EntryIterator iterator;
    chip::Access::AccessControl::Entry entry;
    AclStorage::EncodableEntry encodableEntry(entry);
    return aEncoder.EncodeList([&](const auto & encoder) -> CHIP_ERROR {
        for (auto & info : Server::GetInstance().GetFabricTable())
        {
            auto fabric = info.GetFabricIndex();
            ReturnErrorOnFailure(chip::Access::GetAccessControl().Entries(fabric, iterator));
            CHIP_ERROR err = CHIP_NO_ERROR;
            while ((err = iterator.Next(entry)) == CHIP_NO_ERROR)
            {
                ReturnErrorOnFailure(encoder.Encode(encodableEntry));
            }
            VerifyOrReturnError(err == CHIP_NO_ERROR || err == CHIP_ERROR_SENTINEL, err);
        }
        return CHIP_NO_ERROR;
    });
}

CHIP_ERROR AccessControlCluster::IsValidAclEntryList(const DataModel::DecodableList<AclStorage::DecodableEntry> & list)
{
    auto validationIterator = list.begin();
    while (validationIterator.Next())
    {
        VerifyOrReturnError(validationIterator.GetValue().GetEntry().IsValid(), CHIP_ERROR_INVALID_ARGUMENT);
    }
    ReturnErrorOnFailure(validationIterator.GetStatus());

    return CHIP_NO_ERROR;
}

CHIP_ERROR AccessControlCluster::WriteAcl(const ConcreteDataAttributePath & aPath, AttributeValueDecoder & aDecoder)
{
    FabricIndex accessingFabricIndex = aDecoder.AccessingFabricIndex();

    size_t oldCount;
    ReturnErrorOnFailure(chip::Access::GetAccessControl().GetEntryCount(accessingFabricIndex, oldCount));
    size_t maxCount;
    ReturnErrorOnFailure(chip::Access::GetAccessControl().GetMaxEntriesPerFabric(maxCount));

    if (!aPath.IsListItemOperation())
    {
        DataModel::DecodableList<AclStorage::DecodableEntry> list;
        ReturnErrorOnFailure(aDecoder.Decode(list));

        size_t newCount;
        ReturnErrorOnFailure(list.ComputeSize(&newCount));

        VerifyOrReturnError(newCount <= maxCount, CHIP_IM_GLOBAL_STATUS(ResourceExhausted));

        // Validating all ACL entries in the ReplaceAll list before Updating or Deleting any entries. If any of the entries has an
        // invalid field, the whole "ReplaceAll" list will be rejected.
        ReturnErrorOnFailure(IsValidAclEntryList(list));

        auto iterator = list.begin();
        size_t i      = 0;
        while (iterator.Next())
        {
            if (i < oldCount)
            {
                ReturnErrorOnFailure(chip::Access::GetAccessControl().UpdateEntry(
                    &aDecoder.GetSubjectDescriptor(), accessingFabricIndex, i, iterator.GetValue().GetEntry()));
            }
            else
            {
                ReturnErrorOnFailure(chip::Access::GetAccessControl().CreateEntry(
                    &aDecoder.GetSubjectDescriptor(), accessingFabricIndex, nullptr, iterator.GetValue().GetEntry()));
            }
            ++i;
        }
        ReturnErrorOnFailure(iterator.GetStatus());

        while (i < oldCount)
        {
            --oldCount;
            ReturnErrorOnFailure(
                chip::Access::GetAccessControl().DeleteEntry(&aDecoder.GetSubjectDescriptor(), accessingFabricIndex, oldCount));
        }
    }
    else if (aPath.mListOp == ConcreteDataAttributePath::ListOperation::AppendItem)
    {
        VerifyOrReturnError((oldCount + 1) <= maxCount, CHIP_IM_GLOBAL_STATUS(ResourceExhausted));

        AclStorage::DecodableEntry decodableEntry;
        ReturnErrorOnFailure(aDecoder.Decode(decodableEntry));

        ReturnErrorOnFailure(chip::Access::GetAccessControl().CreateEntry(&aDecoder.GetSubjectDescriptor(), accessingFabricIndex,
                                                                          nullptr, decodableEntry.GetEntry()));
    }
    else
    {
        return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
    }

    return CHIP_NO_ERROR;
}

void AccessControlCluster::OnEntryChanged(const chip::Access::SubjectDescriptor * subjectDescriptor, FabricIndex fabric,
                                          size_t index, const chip::Access::AccessControl::Entry * entry,
                                          chip::Access::AccessControl::EntryListener::ChangeType changeType)
{
    {
        // NOTE: If the entry was changed internally by the system (e.g. creating
        // entries at startup from persistent storage, or deleting entries when a
        // fabric is removed), then there won't be a subject descriptor, and also
        // it won't be appropriate to create an event.
        if (subjectDescriptor == nullptr)
        {
            return;
        }

        CHIP_ERROR err;
        AclEvent event{ .changeType = ChangeTypeEnum::kChanged, .fabricIndex = subjectDescriptor->fabricIndex };

        if (changeType == Access::AccessControl::EntryListener::ChangeType::kAdded)
        {
            event.changeType = ChangeTypeEnum::kAdded;
        }
        else if (changeType == Access::AccessControl::EntryListener::ChangeType::kRemoved)
        {
            event.changeType = ChangeTypeEnum::kRemoved;
        }

        if (subjectDescriptor->authMode == Access::AuthMode::kCase)
        {
            event.adminNodeID.SetNonNull(subjectDescriptor->subject);
        }
        else if (subjectDescriptor->authMode == Access::AuthMode::kPase)
        {
            event.adminPasscodeID.SetNonNull(PAKEKeyIdFromNodeId(subjectDescriptor->subject));
        }

        EventNumber eventNumber;

        if (entry != nullptr)
        {
            // NOTE: don't destroy encodable entry before staging entry is used!
            AclStorage::EncodableEntry encodableEntry(*entry);
            SuccessOrExit(err = encodableEntry.Stage());
            event.latestValue.SetNonNull(encodableEntry.GetStagingEntry());
            SuccessOrExit(err = LogEvent(event, 0, eventNumber));
        }
        else
        {
            SuccessOrExit(err = LogEvent(event, 0, eventNumber));
        }

        return;

    exit:
        ChipLogError(DataManagement, "AccessControlCluster: event failed %" CHIP_ERROR_FORMAT, err.Format());
    }

#if CHIP_CONFIG_ENABLE_ACL_EXTENSIONS
    CHIP_ERROR AccessControlCluster::ReadExtension(AttributeValueEncoder & aEncoder)
    {
        auto & storage = Server::GetInstance().GetPersistentStorage();
        auto & fabrics = Server::GetInstance().GetFabricTable();

        return aEncoder.EncodeList([&](const auto & encoder) -> CHIP_ERROR {
            for (auto & fabric : fabrics)
            {
                uint8_t buffer[kExtensionDataMaxLength] = { 0 };
                uint16_t size                           = static_cast<uint16_t>(sizeof(buffer));
                CHIP_ERROR errStorage                   = storage.SyncGetKeyValue(
                    DefaultStorageKeyAllocator::AccessControlExtensionEntry(fabric.GetFabricIndex()).KeyName(), buffer, size);
                VerifyOrReturnError(errStorage != CHIP_ERROR_BUFFER_TOO_SMALL, CHIP_ERROR_INCORRECT_STATE);
                if (errStorage == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND)
                {
                    continue;
                }
                ReturnErrorOnFailure(errStorage);
                AccessControlCluster::Structs::AccessControlExtensionStruct::Type item = {
                    .data        = ByteSpan(buffer, size),
                    .fabricIndex = fabric.GetFabricIndex(),
                };
                ReturnErrorOnFailure(encoder.Encode(item));
            }
            return CHIP_NO_ERROR;
        });
    }
    CHIP_ERROR AccessControlCluster::LogExtensionChangedEvent(
        const AccessControlCluster::Structs::AccessControlExtensionStruct::Type & item,
        const Access::SubjectDescriptor & subjectDescriptor, AccessControlCluster::ChangeTypeEnum changeType)
    {
        ExtensionEvent event{ .changeType = changeType, .fabricIndex = subjectDescriptor.fabricIndex };

        if (subjectDescriptor.authMode == Access::AuthMode::kCase)
        {
            event.adminNodeID.SetNonNull(subjectDescriptor.subject);
        }
        else if (subjectDescriptor.authMode == Access::AuthMode::kPase)
        {
            event.adminPasscodeID.SetNonNull(PAKEKeyIdFromNodeId(subjectDescriptor.subject));
        }

        event.latestValue.SetNonNull(item);

        EventNumber eventNumber;
        CHIP_ERROR err = LogEvent(event, 0, eventNumber);
        if (CHIP_NO_ERROR != err)
        {
            ChipLogError(DataManagement, "AccessControlCluster: log event failed %" CHIP_ERROR_FORMAT, err.Format());
        }

        return err;
    }

    CHIP_ERROR AccessControlCluster::CheckExtensionEntryDataFormat(const ByteSpan & data)
    {
        CHIP_ERROR err;

        TLV::TLVReader reader;
        reader.Init(data);

        auto containerType = TLV::kTLVType_List;
        err                = reader.Next(containerType, TLV::AnonymousTag());
        VerifyOrReturnError(err == CHIP_NO_ERROR, CHIP_IM_GLOBAL_STATUS(ConstraintError));

        err = reader.EnterContainer(containerType);
        VerifyOrReturnError(err == CHIP_NO_ERROR, CHIP_IM_GLOBAL_STATUS(ConstraintError));

        while ((err = reader.Next()) == CHIP_NO_ERROR)
        {
            VerifyOrReturnError(TLV::IsProfileTag(reader.GetTag()), CHIP_IM_GLOBAL_STATUS(ConstraintError));
        }
        VerifyOrReturnError(err == CHIP_END_OF_TLV, CHIP_IM_GLOBAL_STATUS(ConstraintError));

        err = reader.ExitContainer(containerType);
        VerifyOrReturnError(err == CHIP_NO_ERROR, CHIP_IM_GLOBAL_STATUS(ConstraintError));

        err = reader.Next();
        VerifyOrReturnError(err == CHIP_END_OF_TLV, CHIP_IM_GLOBAL_STATUS(ConstraintError));

        return CHIP_NO_ERROR;
    }

    CHIP_ERROR AccessControlCluster::WriteExtension(const ConcreteDataAttributePath & aPath, AttributeValueDecoder & aDecoder)
    {
        auto & storage = Server::GetInstance().GetPersistentStorage();

        FabricIndex accessingFabricIndex = aDecoder.AccessingFabricIndex();

        uint8_t buffer[kExtensionDataMaxLength] = { 0 };
        uint16_t size                           = static_cast<uint16_t>(sizeof(buffer));
        CHIP_ERROR errStorage                   = storage.SyncGetKeyValue(
            DefaultStorageKeyAllocator::AccessControlExtensionEntry(accessingFabricIndex).KeyName(), buffer, size);
        VerifyOrReturnError(errStorage != CHIP_ERROR_BUFFER_TOO_SMALL, CHIP_ERROR_INCORRECT_STATE);
        VerifyOrReturnError(errStorage == CHIP_NO_ERROR || errStorage == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND, errStorage);

        if (!aPath.IsListItemOperation())
        {
            DataModel::DecodableList<AccessControlCluster::Structs::AccessControlExtensionStruct::DecodableType> list;
            ReturnErrorOnFailure(aDecoder.Decode(list));

            size_t count = 0;
            ReturnErrorOnFailure(list.ComputeSize(&count));

            if (count == 0)
            {
                VerifyOrReturnError(errStorage != CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND, CHIP_NO_ERROR);
                ReturnErrorOnFailure(storage.SyncDeleteKeyValue(
                    DefaultStorageKeyAllocator::AccessControlExtensionEntry(accessingFabricIndex).KeyName()));
                AccessControlCluster::Structs::AccessControlExtensionStruct::Type item = {
                    .data        = ByteSpan(buffer, size),
                    .fabricIndex = accessingFabricIndex,
                };
                ReturnErrorOnFailure(LogExtensionChangedEvent(item, aDecoder.GetSubjectDescriptor(),
                                                              AccessControlCluster::ChangeTypeEnum::kRemoved));
            }
            else if (count == 1)
            {
                auto iterator = list.begin();
                if (!iterator.Next())
                {
                    ReturnErrorOnFailure(iterator.GetStatus());
                    // If counted an item, iterator doesn't return it, iterator has no error, that's bad.
                    return CHIP_ERROR_INCORRECT_STATE;
                }
                auto & item = iterator.GetValue();
                // TODO(#13590): generated code doesn't automatically handle max length so do it manually
                VerifyOrReturnError(item.data.size() <= kExtensionDataMaxLength, CHIP_IM_GLOBAL_STATUS(ConstraintError));

                ReturnErrorOnFailure(CheckExtensionEntryDataFormat(item.data));

                ReturnErrorOnFailure(
                    storage.SyncSetKeyValue(DefaultStorageKeyAllocator::AccessControlExtensionEntry(accessingFabricIndex).KeyName(),
                                            item.data.data(), static_cast<uint16_t>(item.data.size())));
                ReturnErrorOnFailure(LogExtensionChangedEvent(item, aDecoder.GetSubjectDescriptor(),
                                                              errStorage == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND
                                                                  ? AccessControlCluster::ChangeTypeEnum::kAdded
                                                                  : AccessControlCluster::ChangeTypeEnum::kChanged));
            }
            else
            {
                return CHIP_IM_GLOBAL_STATUS(ConstraintError);
            }
        }
        else if (aPath.mListOp == ConcreteDataAttributePath::ListOperation::AppendItem)
        {
            VerifyOrReturnError(errStorage == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND, CHIP_IM_GLOBAL_STATUS(ConstraintError));
            AccessControlCluster::Structs::AccessControlExtensionStruct::DecodableType item;
            ReturnErrorOnFailure(aDecoder.Decode(item));
            // TODO(#13590): generated code doesn't automatically handle max length so do it manually
            VerifyOrReturnError(item.data.size() <= kExtensionDataMaxLength, CHIP_IM_GLOBAL_STATUS(ConstraintError));

            ReturnErrorOnFailure(CheckExtensionEntryDataFormat(item.data));

            ReturnErrorOnFailure(
                storage.SyncSetKeyValue(DefaultStorageKeyAllocator::AccessControlExtensionEntry(accessingFabricIndex).KeyName(),
                                        item.data.data(), static_cast<uint16_t>(item.data.size())));
            ReturnErrorOnFailure(
                LogExtensionChangedEvent(item, aDecoder.GetSubjectDescriptor(), AccessControlCluster::ChangeTypeEnum::kAdded));
        }
        else
        {
            return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
        }

        return CHIP_NO_ERROR;
    }
#endif

#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    CHIP_ERROR AccessControlCluster::ReadCommissioningArl(AttributeValueEncoder & aEncoder)
    {
        auto accessRestrictionProvider = chip::Access::GetAccessControl().GetAccessRestrictionProvider();

        return aEncoder.EncodeList([&](const auto & encoder) -> CHIP_ERROR {
            if (accessRestrictionProvider != nullptr)
            {
                auto entries = accessRestrictionProvider->GetCommissioningEntries();

                for (auto & entry : entries)
                {
                    ArlEncoder::CommissioningEncodableEntry encodableEntry(entry);
                    ReturnErrorOnFailure(encoder.Encode(encodableEntry));
                }
            }
            return CHIP_NO_ERROR;
        });
    }

    CHIP_ERROR AccessControlCluster::ReadArl(AttributeValueEncoder & aEncoder)
    {
        auto accessRestrictionProvider = chip::Access::GetAccessControl().GetAccessRestrictionProvider();

        return aEncoder.EncodeList([&](const auto & encoder) -> CHIP_ERROR {
            if (accessRestrictionProvider != nullptr)
            {
                for (const auto & info : Server::GetInstance().GetFabricTable())
                {
                    auto fabric = info.GetFabricIndex();
                    // get entries for fabric
                    std::vector<AccessRestrictionProvider::Entry> entries;
                    ReturnErrorOnFailure(accessRestrictionProvider->GetEntries(fabric, entries));
                    for (const auto & entry : entries)
                    {
                        ArlEncoder::EncodableEntry encodableEntry(entry);
                        ReturnErrorOnFailure(encoder.Encode(encodableEntry));
                    }
                }
            }
            return CHIP_NO_ERROR;
        });
    }

    // void AccessControlCluster::OnFabricRestrictionReviewUpdate(FabricIndex fabricIndex, uint64_t token,
    //                                                              Optional<CharSpan> instruction, Optional<CharSpan>
    //                                                              arlRequestFlowUrl)
    // {
    //     CHIP_ERROR err;
    //     ArlReviewEvent event{ .token = token, .fabricIndex = fabricIndex };

    //     event.instruction       = instruction;
    //     event.ARLRequestFlowUrl = arlRequestFlowUrl;

    //     EventNumber eventNumber;
    //     SuccessOrExit(err = LogEvent(event, kRootEndpointId, eventNumber));

    //     return;

    // exit:
    //     ChipLogError(DataManagement, "AccessControlCluster: review event failed: %" CHIP_ERROR_FORMAT, err.Format());
    // }

    std::optional<DataModel::ActionReturnStatus> AccessControlCluster::HandleReviewFabricRestrictions(
        CommandHandler * commandObj, const ConcreteCommandPath & commandPath,
        const Clusters::AccessControl::Commands::ReviewFabricRestrictions::DecodableType & commandData)
    {
        if (commandPath.mEndpointId != kRootEndpointId)
        {
            ChipLogError(DataManagement, "AccessControlCluster: invalid endpoint in ReviewFabricRestrictions request");
            return Protocols::InteractionModel::Status::InvalidCommand;
        }

        uint64_t token;
        std::vector<AccessRestrictionProvider::Entry> entries;
        auto entryIter = commandData.arl.begin();
        while (entryIter.Next())
        {
            AccessRestrictionProvider::Entry entry;
            entry.fabricIndex    = commandObj->GetAccessingFabricIndex();
            entry.endpointNumber = entryIter.GetValue().endpoint;
            entry.clusterId      = entryIter.GetValue().cluster;

            auto restrictionIter = entryIter.GetValue().restrictions.begin();
            while (restrictionIter.Next())
            {
                AccessRestrictionProvider::Restriction restriction;
                if (ArlEncoder::Convert(restrictionIter.GetValue().type, restriction.restrictionType) != CHIP_NO_ERROR)
                {
                    ChipLogError(DataManagement, "AccessControlCluster: invalid restriction type conversion");
                    return Protocols::InteractionModel::Status::InvalidCommand;
                }

                if (!restrictionIter.GetValue().id.IsNull())
                {
                    restriction.id.SetValue(restrictionIter.GetValue().id.Value());
                }
                entry.restrictions.push_back(restriction);
            }

            if (restrictionIter.GetStatus() != CHIP_NO_ERROR)
            {
                ChipLogError(DataManagement, "AccessControlCluster: invalid ARL data");
                return Protocols::InteractionModel::Status::InvalidCommand;
            }

            entries.push_back(entry);
        }

        if (entryIter.GetStatus() != CHIP_NO_ERROR)
        {
            ChipLogError(DataManagement, "AccessControlCluster: invalid ARL data");
            return Protocols::InteractionModel::Status::InvalidCommand;
        }

        CHIP_ERROR err = chip::Access::GetAccessControl().GetAccessRestrictionProvider()->RequestFabricRestrictionReview(
            commandObj->GetAccessingFabricIndex(), entries, token);

        if (err == CHIP_NO_ERROR)
        {
            Clusters::AccessControl::Commands::ReviewFabricRestrictionsResponse::Type response;
            response.token = token;
            commandObj->AddResponse(commandPath, response);
        }
        else
        {
            ChipLogError(DataManagement, "AccessControlCluster: restriction review failed: %" CHIP_ERROR_FORMAT, err.Format());
            return Protocols::InteractionModel::ClusterStatusCode(err);
        }

        return Protocols::InteractionModel::Status::Success;
    }
#endif
}
} // namespace Clusters
} // namespace app
} // namespace chip
