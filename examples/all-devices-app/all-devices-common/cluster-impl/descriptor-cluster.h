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
    DescriptorCluster(EndpointId endpointId, std::initializer_list<DeviceType> deviceTypes) :
        DefaultServerCluster({ endpointId, Descriptor::Id }), mDeviceTypes(deviceTypes)
    {}

    DescriptorCluster(EndpointId endpointId, std::vector<DeviceType> deviceTypes) :
        DefaultServerCluster({ endpointId, Descriptor::Id }), mDeviceTypes(std::move(deviceTypes))
    {}

    CHIP_ERROR Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;
    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                AttributeValueEncoder & encoder) override;

private:
    // TODO: is a parts list needed instead of using the old part list with read attribute code?
    std::vector<DeviceType> mDeviceTypes;
    CHIP_ERROR ReadPartsAttribute(EndpointId endpoint, AttributeValueEncoder & aEncoder);
};

} // namespace chip::app::Clusters
