#pragma once

#include <app/server-cluster/DefaultServerCluster.h>
#include <clusters/Descriptor/ClusterId.h>
#include <clusters/Descriptor/Structs.h>

#include <initializer_list>
#include <vector>

namespace chip::app::Clusters {

/// Exposes mandatory attributes for the descriptor clusters (it does NOT implement
/// optional bits yet.
class DescriptorCluster : public DefaultServerCluster
{
public:
    using DeviceType = Descriptor::Structs::DeviceTypeStruct::Type;

    /// Creates a descriptor cluster for the given `endpointId`
    ///
    /// Associates the given device types to the descriptor (should be at least 1)
    DescriptorCluster(EndpointId endpointId, BitFlags<Descriptor::Feature> featureFlags) :
        DefaultServerCluster({ endpointId, Descriptor::Id }), mFeatureFlags(featureFlags)
    {}

    CHIP_ERROR Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;
    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                AttributeValueEncoder & encoder) override;

private:
    BitFlags<Descriptor::Feature> mFeatureFlags;
};

} // namespace chip::app::Clusters
