#pragma once

#include <app-common/zap-generated/cluster-objects.h>
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

    CHIP_ERROR RemoveClientCluster(ClusterId clusterId);
    CHIP_ERROR RemoveServerCluster(ClusterId clusterId);
    CHIP_ERROR RemoveSemanticTag(const SemanticTag & semanticTag);
    CHIP_ERROR RemoveDeviceType(DeviceTypeId deviceTypeId);

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
