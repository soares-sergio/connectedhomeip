#include "DynamicEndpointProvider.h"

#include <app/ConcreteClusterPath.h> // For logging and ID retrieval
#include <app/server-cluster/ServerClusterContext.h>
#include <lib/core/CHIPError.h>
#include <lib/support/Span.h>
#include <lib/support/logging/CHIPLogging.h>

namespace chip {
namespace app {

DynamicEndpointProvider::DynamicEndpointProvider(EndpointId id, DataModel::EndpointCompositionPattern composition,
                                                 EndpointId parentId) : mEndpointEntry({ id, parentId, composition })
{}

CHIP_ERROR DynamicEndpointProvider::AddServerCluster(ServerClusterInterface & cluster)
{
    mServerClusters.push_back(&cluster); // Store pointer to the cluster
    return CHIP_NO_ERROR;
}

CHIP_ERROR DynamicEndpointProvider::AddClientCluster(ClusterId clusterId)
{
    mClientClusters.push_back(clusterId); // Store ClusterId
    return CHIP_NO_ERROR;
}

CHIP_ERROR DynamicEndpointProvider::AddSemanticTag(const SemanticTag & semanticTag)
{
    mSemanticTags.push_back(semanticTag); // Store SemanticTag
    return CHIP_NO_ERROR;
}

CHIP_ERROR DynamicEndpointProvider::AddDeviceType(const DataModel::DeviceTypeEntry & deviceType)
{
    mDeviceTypes.push_back(deviceType);
    return CHIP_NO_ERROR;
}
CHIP_ERROR DynamicEndpointProvider::RemoveClientCluster(ClusterId clusterId)
{
    for (auto it = mClientClusters.begin(); it != mClientClusters.end(); ++it)
    {
        if (*it == clusterId)
        {
            mClientClusters.erase(it);
            return CHIP_NO_ERROR;
        }
    }
    return CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR DynamicEndpointProvider::RemoveServerCluster(ClusterId clusterId)
{
    for (auto it = mServerClusters.begin(); it != mServerClusters.end(); ++it)
    {
        // Ensure GetPaths() is not empty before accessing front().
        // This was an implicit assumption in the original code.
        if (!(*it)->GetPaths().empty() && (*it)->GetPaths().front().mClusterId == clusterId)
        {
            mServerClusters.erase(it);
            return CHIP_NO_ERROR;
        }
    }
    return CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR DynamicEndpointProvider::RemoveSemanticTag(const SemanticTag & semanticTag)
{
    for (auto it = mSemanticTags.begin(); it != mSemanticTags.end(); ++it)
    {
        if ((it->tag == semanticTag.tag) && (it->namespaceID == semanticTag.namespaceID))
        {
            mSemanticTags.erase(it);
            return CHIP_NO_ERROR;
        }
    }
    return CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR DynamicEndpointProvider::RemoveDeviceType(DeviceTypeId deviceTypeId)
{
    for (auto it = mDeviceTypes.begin(); it != mDeviceTypes.end(); ++it)
    {
        if (it->deviceTypeId == deviceTypeId)
        {
            mDeviceTypes.erase(it);
            return CHIP_NO_ERROR;
        }
    }
    return CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR DynamicEndpointProvider::SemanticTags(ReadOnlyBufferBuilder<SemanticTag> & out)
{
    ReturnErrorOnFailure(out.EnsureAppendCapacity(mSemanticTags.size()));
    for (const auto & tag : mSemanticTags)
    {
        ReturnErrorOnFailure(out.Append(tag));
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR DynamicEndpointProvider::DeviceTypes(ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out)
{
    ReturnErrorOnFailure(out.EnsureAppendCapacity(mDeviceTypes.size()));
    for (const auto & dt : mDeviceTypes)
    {
        ReturnErrorOnFailure(out.Append(dt));
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR DynamicEndpointProvider::ClientClusters(ReadOnlyBufferBuilder<ClusterId> & out)
{
    ReturnErrorOnFailure(out.EnsureAppendCapacity(mClientClusters.size()));
    for (const auto & id : mClientClusters)
    {
        ReturnErrorOnFailure(out.Append(id));
    }
    return CHIP_NO_ERROR;
}

ServerClusterInterface * DynamicEndpointProvider::GetServerCluster(ClusterId clusterId)
{
    for (auto * serverCluster : mServerClusters)
    {
        // Ensure GetPaths() is not empty before accessing front().
        if (!serverCluster->GetPaths().empty() && serverCluster->GetPaths().front().mClusterId == clusterId)
        {
            return serverCluster;
        }
    }
    return nullptr;
}

CHIP_ERROR DynamicEndpointProvider::ServerClusterInterfaces(ReadOnlyBufferBuilder<ServerClusterInterface *> & out) const
{
    ReturnErrorOnFailure(out.EnsureAppendCapacity(mServerClusters.size()));
    for (ServerClusterInterface * cluster : mServerClusters)
    {
        ReturnErrorOnFailure(out.Append(cluster));
    }
    return CHIP_NO_ERROR;
}

} // namespace app
} // namespace chip
