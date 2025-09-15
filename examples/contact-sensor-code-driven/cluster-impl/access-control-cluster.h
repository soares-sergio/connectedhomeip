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

#include <access/AccessControl.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/OptionalAttributeSet.h>
#include <app/server/Server.h>
#include <clusters/AccessControl/ClusterId.h>
#include <clusters/AccessControl/Metadata.h>
#include <platform/DiagnosticDataProvider.h>

namespace chip {
namespace app {
namespace Clusters {

class AccessControlCluster : public DefaultServerCluster, public chip::Access::AccessControl::EntryListener
{
public:
    using OptionalAttributeSet =
        chip::app::OptionalAttributeSet<AccessControl::Attributes::Extension::Id, AccessControl::Attributes::CommissioningARL::Id,
                                        AccessControl::Attributes::Arl::Id>;

    AccessControlCluster(OptionalAttributeSet optionalAttributeSet) :
        DefaultServerCluster({ kRootEndpointId, AccessControl::Id }), mOptionalAttributeSet(optionalAttributeSet)
    {}

    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                AttributeValueEncoder & encoder) override;

    DataModel::ActionReturnStatus WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                 AttributeValueDecoder & decoder) override;

    std::optional<DataModel::ActionReturnStatus> InvokeCommand(const DataModel::InvokeRequest & request,
                                                               chip::TLV::TLVReader & input_arguments,
                                                               CommandHandler * handler) override;

    CHIP_ERROR Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;

    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath & path,
                                ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder) override;

    CHIP_ERROR GeneratedCommands(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<CommandId> & builder) override;

private:
    const OptionalAttributeSet mOptionalAttributeSet;

    CHIP_ERROR ReadAcl(AttributeValueEncoder & aEncoder);
    CHIP_ERROR IsValidAclEntryList(const DataModel::DecodableList<AclStorage::DecodableEntry> & list);
    CHIP_ERROR WriteAcl(const ConcreteDataAttributePath & aPath, AttributeValueDecoder & aDecoder);
    void OnEntryChanged(const chip::Access::SubjectDescriptor * subjectDescriptor, FabricIndex fabric, size_t index,
                        const chip::Access::AccessControl::Entry * entry,
                        chip::Access::AccessControl::EntryListener::ChangeType changeType) override;

#if CHIP_CONFIG_ENABLE_ACL_EXTENSIONS
    CHIP_ERROR ReadExtension(AttributeValueEncoder & aEncoder);
    CHIP_ERROR LogExtensionChangedEvent(const Structs::AccessControlExtensionStruct::Type & item,
                                        const Access::SubjectDescriptor & subjectDescriptor, ChangeTypeEnum changeType);
    CHIP_ERROR CheckExtensionEntryDataFormat(const ByteSpan & data);
    CHIP_ERROR WriteExtension(const ConcreteDataAttributePath & aPath, AttributeValueDecoder & aDecoder);

#endif

#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    CHIP_ERROR ReadCommissioningArl(AttributeValueEncoder & aEncoder);
    CHIP_ERROR ReadArl(AttributeValueEncoder & aEncoder);
    // void OnFabricRestrictionReviewUpdate(FabricIndex fabricIndex, uint64_t token,
    //                                                          Optional<CharSpan> instruction, Optional<CharSpan>
    //                                                          arlRequestFlowUrl) override;
    std::optional<DataModel::ActionReturnStatus>
    HandleReviewFabricRestrictions(CommandHandler * commandObj, const ConcreteCommandPath & commandPath,
                                   const Clusters::AccessControl::Commands::ReviewFabricRestrictions::DecodableType & commandData);
#endif
};

} // namespace Clusters
} // namespace app
} // namespace chip
