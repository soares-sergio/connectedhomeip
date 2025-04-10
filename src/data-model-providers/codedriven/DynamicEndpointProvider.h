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
#pragma once

#include <app/data-model-provider/MetadataTypes.h>
#include <app/server-cluster/ServerClusterInterface.h>
#include <data-model-providers/codedriven/EndpointProviderInterface.h>
#include <lib/core/DataModelTypes.h>
#include <lib/support/ReadOnlyBuffer.h>
#include <list>

namespace chip {
namespace app {

class DynamicEndpointProvider : public EndpointProviderInterface
{
public:
    DynamicEndpointProvider(EndpointId id, DataModel::EndpointCompositionPattern composition,
                            EndpointId parentId = kInvalidEndpointId);

    CHIP_ERROR SetEndpointEntry(DataModel::EndpointEntry entry)
    {
        mEndpointEntry = entry;
        return CHIP_NO_ERROR;
    }
    const DataModel::EndpointEntry & GetEndpointEntry() const override { return mEndpointEntry; }

    // Endpoint construction methods
    CHIP_ERROR AddServerCluster(ServerClusterInterface & cluster);
    CHIP_ERROR AddClientCluster(ClusterId clusterId);
    CHIP_ERROR AddSemanticTag(const SemanticTag & semanticTag);
    CHIP_ERROR AddDeviceType(const DataModel::DeviceTypeEntry & deviceType);

    // Iteration methods
    CHIP_ERROR SemanticTags(ReadOnlyBufferBuilder<SemanticTag> & out) override;
    CHIP_ERROR DeviceTypes(ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out) override;
    CHIP_ERROR ClientClusters(ReadOnlyBufferBuilder<ClusterId> & out) override;

    // Getter for ServerClusterInterface pointers
    ServerClusterInterface * GetServerCluster(ClusterId clusterId) override;
    CHIP_ERROR ServerClusterInterfaces(ReadOnlyBufferBuilder<ServerClusterInterface *> & out) const override;

private:
    DataModel::EndpointEntry mEndpointEntry;
    std::list<DataModel::DeviceTypeEntry> mDeviceTypes;
    std::list<SemanticTag> mSemanticTags;
    std::list<ClusterId> mClientClusters;
    std::list<ServerClusterInterface *> mServerClusters;
};

} // namespace app
} // namespace chip
