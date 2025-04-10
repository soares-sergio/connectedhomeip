#pragma once

#include <app/CommandHandlerInterface.h>
#include <app/ConcreteCommandPath.h>
#include <app/data-model-provider/ActionReturnStatus.h>
#include <app/data-model-provider/MetadataTypes.h>
#include <app/data-model-provider/Provider.h>
#include <app/server-cluster/ServerClusterInterface.h>
#include <data-model-providers/codedriven/EndpointProviderInterface.h>
#include <lib/support/ReadOnlyBuffer.h>
#include <list>

namespace chip {
namespace app {

using DataModel::ClusterInfo;
using DataModel::ServerClusterEntry;

class CodeDrivenDataModelProvider : public DataModel::Provider
{
public:
    /// Generic model implementations

    CHIP_ERROR Startup(DataModel::InteractionModelContext context) override;
    CHIP_ERROR Shutdown() override;

    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                AttributeValueEncoder & encoder) override;
    DataModel::ActionReturnStatus WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                 AttributeValueDecoder & decoder) override;

    void ListAttributeWriteNotification(const ConcreteAttributePath & aPath, DataModel::ListWriteOperation opType) override;
    std::optional<DataModel::ActionReturnStatus> InvokeCommand(const DataModel::InvokeRequest & request,
                                                               TLV::TLVReader & input_arguments, CommandHandler * handler) override;

    void SetPersistentStorageDelegate(PersistentStorageDelegate * delegate) { mPersistentStorageDelegate = delegate; }
    PersistentStorageDelegate * GetPersistentStorageDelegate() { return mPersistentStorageDelegate; }

    /// attribute tree iteration
    CHIP_ERROR Endpoints(ReadOnlyBufferBuilder<DataModel::EndpointEntry> & out) override;
    CHIP_ERROR SemanticTags(EndpointId endpointId,
                            ReadOnlyBufferBuilder<Clusters::Descriptor::Structs::SemanticTagStruct::Type> & out) override;
    CHIP_ERROR DeviceTypes(EndpointId endpointId, ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out) override;
    CHIP_ERROR ClientClusters(EndpointId endpointId, ReadOnlyBufferBuilder<ClusterId> & out) override;
    CHIP_ERROR ServerClusters(EndpointId endpointId, ReadOnlyBufferBuilder<DataModel::ServerClusterEntry> & out) override;
    CHIP_ERROR GeneratedCommands(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<CommandId> & out) override;
    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath & path,
                                ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & out) override;
    CHIP_ERROR Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;

    void Temporary_ReportAttributeChanged(const AttributePathParams & path) override;

    // Data model tree construction
    CHIP_ERROR AddEndpoint(EndpointProviderInterface & provider);
    CHIP_ERROR RemoveEndpoint(EndpointId endpointId);

private:
    std::list<EndpointProviderInterface *> mEndpointProviders;
    EndpointProviderInterface * GetEndpointProvider(EndpointId endpointId);
    std::optional<ServerClusterContext> mServerClusterContext;
    PersistentStorageDelegate * mPersistentStorageDelegate = nullptr;
};

} // namespace app
} // namespace chip
