#pragma once

#include <app/server-cluster/DefaultServerCluster.h>
#include <clusters/BridgedDeviceBasicInformation/ClusterId.h>

#include <string>
#include <utility>

namespace chip::app::Clusters {

class BridgedDeviceBasicInformationCluster : public DefaultServerCluster
{
public:
    BridgedDeviceBasicInformationCluster(EndpointId endpointId, std::string uniqueId) :
        DefaultServerCluster({ endpointId, BridgedDeviceBasicInformation::Id }), mUniqueID(std::move(uniqueId))
    {}

    CHIP_ERROR Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;
    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                AttributeValueEncoder & encoder) override;

private:
    std::string mUniqueID;
};

} // namespace chip::app::Clusters
