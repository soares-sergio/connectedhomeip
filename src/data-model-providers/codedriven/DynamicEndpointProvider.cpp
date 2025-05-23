/*
 *    Copyright (c) 2025 Project CHIP Authors
 *    All rights reserved.
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
    if (cluster.GetPaths().empty())
    {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    mServerClusters.push_back(&cluster);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DynamicEndpointProvider::AddClientCluster(ClusterId clusterId)
{
    mClientClusters.push_back(clusterId);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DynamicEndpointProvider::AddSemanticTag(const SemanticTag & semanticTag)
{
    mSemanticTags.push_back(semanticTag);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DynamicEndpointProvider::AddDeviceType(const DataModel::DeviceTypeEntry & deviceType)
{
    mDeviceTypes.push_back(deviceType);
    return CHIP_NO_ERROR;
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
