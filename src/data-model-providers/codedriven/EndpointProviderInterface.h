#pragma once

#include <app-common/zap-generated/cluster-objects.h>
#include <app/data-model-provider/MetadataTypes.h>
#include <app/server-cluster/ServerClusterInterface.h>
#include <lib/core/DataModelTypes.h>
#include <lib/support/ReadOnlyBuffer.h>

namespace chip {
namespace app {

using SemanticTag = chip::app::Clusters::Descriptor::Structs::SemanticTagStruct::Type;

class EndpointProviderInterface
{
public:
    virtual ~EndpointProviderInterface() = default;

    virtual const DataModel::EndpointEntry & GetEndpointEntry() const                                       = 0;
    virtual CHIP_ERROR SemanticTags(ReadOnlyBufferBuilder<SemanticTag> & out)                               = 0;
    virtual CHIP_ERROR DeviceTypes(ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out)                 = 0;
    virtual CHIP_ERROR ClientClusters(ReadOnlyBufferBuilder<ClusterId> & out)                               = 0;
    virtual ServerClusterInterface * GetServerCluster(ClusterId clusterId)                                  = 0;
    virtual CHIP_ERROR ServerClusterInterfaces(ReadOnlyBufferBuilder<ServerClusterInterface *> & out) const = 0;
};

} // namespace app
} // namespace chip
