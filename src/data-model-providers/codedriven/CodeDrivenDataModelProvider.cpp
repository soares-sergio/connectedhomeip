#include <app/server-cluster/ServerClusterContext.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <lib/core/CHIPError.h>
#include <lib/support/logging/CHIPLogging.h>
#include <protocols/interaction_model/StatusCode.h>

using chip::Protocols::InteractionModel::Status;
namespace chip {
namespace app {

CHIP_ERROR CodeDrivenDataModelProvider::Startup(DataModel::InteractionModelContext context)
{
    ReturnErrorOnFailure(DataModel::Provider::Startup(context));

    mServerClusterContext.reset();
    mServerClusterContext.emplace(ServerClusterContext({
        .provider           = this,
        .storage            = mPersistentStorageDelegate,
        .interactionContext = &mContext,
    }));

    // Startup all server clusters across all endpoints
    for (auto * endpointProvider : mEndpointProviders) // Iterate over EndpointProviderInterface pointers
    {
        ReadOnlyBufferBuilder<ServerClusterInterface *> serverClusterBuilder;
        ReturnErrorOnFailure(endpointProvider->ServerClusterInterfaces(serverClusterBuilder));
        auto serverClusters = serverClusterBuilder.TakeBuffer();

        for (auto * serverCluster : serverClusters)
        {
            CHIP_ERROR err = serverCluster->Startup(*mServerClusterContext);
            if (err != CHIP_NO_ERROR)
            {
#if CHIP_ERROR_LOGGING
                // Log startup failure for the specific cluster
                if (!serverCluster->GetPaths().empty())
                {
                    const ConcreteClusterPath path = serverCluster->GetPaths().front();
                    ChipLogError(
                        DataManagement, "Cluster %u/" ChipLogFormatMEI " on Endpoint %u startup failed: %" CHIP_ERROR_FORMAT,
                        path.mEndpointId, ChipLogValueMEI(path.mClusterId), endpointProvider->GetEndpointEntry().id, err.Format());
                }
                else
                {
                    ChipLogError(DataManagement, "A ServerCluster on Endpoint %u startup failed: %" CHIP_ERROR_FORMAT,
                                 endpointProvider->GetEndpointEntry().id, err.Format());
                }
#endif
            }
        }
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR CodeDrivenDataModelProvider::Shutdown()
{
    // Shutdown all server clusters across all endpoints
    for (auto * endpointProvider : mEndpointProviders) // Iterate over EndpointProviderInterface pointers
    {
        ReadOnlyBufferBuilder<ServerClusterInterface *> serverClusterBuilder;
        ReturnErrorOnFailure(endpointProvider->ServerClusterInterfaces(serverClusterBuilder));
        auto serverClusters = serverClusterBuilder.TakeBuffer();

        for (auto * serverCluster : serverClusters)
        {
            // TODO: The API for serverCluster->Shutdown is swallowing errors here. We should update it to return CHIP_ERROR.
            serverCluster->Shutdown();
        }
    }
    mServerClusterContext.reset();
    return CHIP_NO_ERROR;
}

DataModel::ActionReturnStatus CodeDrivenDataModelProvider::ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                                         AttributeValueEncoder & encoder)
{
    EndpointProviderInterface * epProvider = GetEndpointProvider(request.path.mEndpointId); // Use EndpointProviderInterface
    if (epProvider == nullptr)
    {
        return Status::UnsupportedEndpoint;
    }
    ServerClusterInterface * serverCluster = epProvider->GetServerCluster(request.path.mClusterId);
    if (serverCluster == nullptr)
    {
        return Status::UnsupportedCluster;
    }
    return serverCluster->ReadAttribute(request, encoder);
}

DataModel::ActionReturnStatus CodeDrivenDataModelProvider::WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                                          AttributeValueDecoder & decoder)
{
    EndpointProviderInterface * epProvider = GetEndpointProvider(request.path.mEndpointId); // Use EndpointProviderInterface
    if (epProvider == nullptr)
    {
        return Status::UnsupportedEndpoint;
    }
    ServerClusterInterface * serverCluster = epProvider->GetServerCluster(request.path.mClusterId);
    if (serverCluster == nullptr)
    {
        return Status::UnsupportedCluster;
    }
    return serverCluster->WriteAttribute(request, decoder);
}

void CodeDrivenDataModelProvider::ListAttributeWriteNotification(const ConcreteAttributePath & aPath,
                                                                 DataModel::ListWriteOperation opType)
{
    EndpointProviderInterface * epProvider = GetEndpointProvider(aPath.mEndpointId); // Use EndpointProviderInterface
    if (epProvider == nullptr)
    {
        return;
    }
    ServerClusterInterface * serverCluster = epProvider->GetServerCluster(aPath.mClusterId);
    if (serverCluster != nullptr)
    {
        serverCluster->ListAttributeWriteNotification(aPath, opType);
    }
}

std::optional<DataModel::ActionReturnStatus> CodeDrivenDataModelProvider::InvokeCommand(const DataModel::InvokeRequest & request,
                                                                                        TLV::TLVReader & input_arguments,
                                                                                        CommandHandler * handler)
{
    EndpointProviderInterface * epProvider = GetEndpointProvider(request.path.mEndpointId); // Use EndpointProviderInterface
    if (epProvider == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    ServerClusterInterface * serverCluster = epProvider->GetServerCluster(request.path.mClusterId);
    if (serverCluster == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    return serverCluster->InvokeCommand(request, input_arguments, handler);
}

CHIP_ERROR CodeDrivenDataModelProvider::Endpoints(ReadOnlyBufferBuilder<DataModel::EndpointEntry> & out)
{
    ReturnErrorOnFailure(out.EnsureAppendCapacity(mEndpointProviders.size()));
    for (const auto * epProvider : mEndpointProviders) // Iterate over EndpointProviderInterface pointers
    {
        ReturnErrorOnFailure(out.Append(epProvider->GetEndpointEntry()));
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR
CodeDrivenDataModelProvider::SemanticTags(EndpointId endpointId,
                                          ReadOnlyBufferBuilder<Clusters::Descriptor::Structs::SemanticTagStruct::Type> & out)
{
    EndpointProviderInterface * epProvider = GetEndpointProvider(endpointId); // Use EndpointProviderInterface
    if (epProvider == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    return epProvider->SemanticTags(out);
}

CHIP_ERROR CodeDrivenDataModelProvider::DeviceTypes(EndpointId endpointId, ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out)
{
    EndpointProviderInterface * epProvider = GetEndpointProvider(endpointId); // Use EndpointProviderInterface
    if (epProvider == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    return epProvider->DeviceTypes(out);
}

CHIP_ERROR CodeDrivenDataModelProvider::ClientClusters(EndpointId endpointId, ReadOnlyBufferBuilder<ClusterId> & out)
{
    EndpointProviderInterface * epProvider = GetEndpointProvider(endpointId);
    if (epProvider == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    return epProvider->ClientClusters(out);
}

CHIP_ERROR CodeDrivenDataModelProvider::ServerClusters(EndpointId endpointId,
                                                       ReadOnlyBufferBuilder<DataModel::ServerClusterEntry> & out)
{
    EndpointProviderInterface * epProvider = GetEndpointProvider(endpointId); // Use EndpointProviderInterface
    if (epProvider == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }

    ReadOnlyBufferBuilder<ServerClusterInterface *> serverClusterBuilder;
    ReturnErrorOnFailure(epProvider->ServerClusterInterfaces(serverClusterBuilder));
    auto serverClusters = serverClusterBuilder.TakeBuffer();

    // Ensure capacity before appending.
    ReturnErrorOnFailure(out.EnsureAppendCapacity(serverClusters.size()));

    for (const auto * serverCluster : serverClusters)
    {
        if (!serverCluster->GetPaths().empty())
        {
            auto path = serverCluster->GetPaths().front();
            ReturnErrorOnFailure(
                out.Append({ path.mClusterId, serverCluster->GetDataVersion(path), serverCluster->GetClusterFlags(path) }));
        }
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR CodeDrivenDataModelProvider::GeneratedCommands(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<CommandId> & out)
{
    EndpointProviderInterface * epProvider = GetEndpointProvider(path.mEndpointId); // Use EndpointProviderInterface
    if (epProvider == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    ServerClusterInterface * serverCluster = epProvider->GetServerCluster(path.mClusterId);
    if (serverCluster == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    return serverCluster->GeneratedCommands(path, out);
}
CHIP_ERROR CodeDrivenDataModelProvider::AcceptedCommands(const ConcreteClusterPath & path,
                                                         ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & out)
{
    EndpointProviderInterface * epProvider = GetEndpointProvider(path.mEndpointId); // Use EndpointProviderInterface
    if (epProvider == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    ServerClusterInterface * serverCluster = epProvider->GetServerCluster(path.mClusterId);
    if (serverCluster == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    return serverCluster->AcceptedCommands(path, out);
}

CHIP_ERROR CodeDrivenDataModelProvider::Attributes(const ConcreteClusterPath & path,
                                                   ReadOnlyBufferBuilder<DataModel::AttributeEntry> & out)
{
    EndpointProviderInterface * epProvider = GetEndpointProvider(path.mEndpointId); // Use EndpointProviderInterface
    if (epProvider == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    ServerClusterInterface * serverCluster = epProvider->GetServerCluster(path.mClusterId);
    if (serverCluster == nullptr)
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    return serverCluster->Attributes(path, out);
}

void CodeDrivenDataModelProvider::Temporary_ReportAttributeChanged(const AttributePathParams & path)
{
    ChipLogDetail(DataManagement, "CodeDrivenDataModelProvider::Temporary_ReportAttributeChanged");
}

CHIP_ERROR CodeDrivenDataModelProvider::AddEndpoint(EndpointProviderInterface & provider) // Use EndpointProviderInterface
{
    if (GetEndpointProvider(provider.GetEndpointEntry().id) != nullptr)
    {
        return CHIP_ERROR_DUPLICATE_KEY_ID;
    }

    mEndpointProviders.push_back(&provider);

    // Startup clusters for the newly added endpoint provider
    if (mServerClusterContext.has_value())
    {
        ReadOnlyBufferBuilder<ServerClusterInterface *> serverClusterBuilder;
        ReturnErrorOnFailure(provider.ServerClusterInterfaces(serverClusterBuilder));
        auto serverClusters = serverClusterBuilder.TakeBuffer();

        for (auto * serverCluster : serverClusters)
        {
            CHIP_ERROR err = serverCluster->Startup(*mServerClusterContext);
            if (err != CHIP_NO_ERROR)
            {
                // Log startup failure for the specific cluster
                if (!serverCluster->GetPaths().empty())
                {
                    const ConcreteClusterPath path = serverCluster->GetPaths().front();
                    ChipLogError(DataManagement,
                                 "Cluster " ChipLogFormatMEI
                                 " on Endpoint %u (via AddEndpoint) startup failed: %" CHIP_ERROR_FORMAT,
                                 ChipLogValueMEI(path.mClusterId), provider.GetEndpointEntry().id, err.Format());
                }
                else
                {
                    ChipLogError(DataManagement,
                                 "A ServerCluster on Endpoint %u (via AddEndpoint) startup failed: %" CHIP_ERROR_FORMAT,
                                 provider.GetEndpointEntry().id, err.Format());
                }
            }
        }
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR CodeDrivenDataModelProvider::RemoveEndpoint(EndpointId endpointId)
{
    for (auto it = mEndpointProviders.begin(); it != mEndpointProviders.end(); ++it)
    {
        if ((*it)->GetEndpointEntry().id == endpointId)
        {
            // Shutdown clusters for the endpoint provider being removed
            if (mServerClusterContext.has_value())
            {
                ReadOnlyBufferBuilder<ServerClusterInterface *> serverClusterBuilder;
                ReturnErrorOnFailure((*it)->ServerClusterInterfaces(serverClusterBuilder));
                auto serverClusters = serverClusterBuilder.TakeBuffer();

                for (auto * serverCluster : serverClusters)
                {
                    // TODO: The API for serverCluster->Shutdown is swallowing errors here. We should update it to propagate up
                    serverCluster->Shutdown();
                }
            }
            mEndpointProviders.erase(it);
            return CHIP_NO_ERROR;
        }
    }
    return CHIP_ERROR_NOT_FOUND;
}

EndpointProviderInterface * CodeDrivenDataModelProvider::GetEndpointProvider(EndpointId endpointId)
{
    for (auto * epProvider : mEndpointProviders)
    {
        if (epProvider->GetEndpointEntry().id == endpointId)
        {
            return epProvider;
        }
    }
    return nullptr;
}
} // namespace app
} // namespace chip
