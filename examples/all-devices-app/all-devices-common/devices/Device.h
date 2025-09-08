#pragma once

#include <app/util/basic-types.h>
#include <cluster-impl/bridged-device-basic-information-cluster.h>
#include <cluster-impl/descriptor-cluster.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/endpoint/EndpointInterfaceRegistry.h>

#include <string>

namespace chip::app {

/// A list of supported devices, since some of our methods want to
/// know the device type.
enum class DeviceType : DeviceTypeId
{
    kAggregator        = 0x000E,
    kBridgedNodeDevice = 0x0013,
    kRootNode          = 0x0016,
    kContactSensor     = 0x0015,
    kOccupancySensor   = 0x0107,
    kOnOffLight        = 0x0100,
    kOnOffPlug         = 0x010A,
};

/// A device is a entity that is uniquely identified (unique id string)
/// and maintains some cluster functionality.
///
/// Current implementation assumes that a device is registered on a single
/// endpoint.
class Device : public EndpointInterface
{
public:
    Device(std::string id) : mUniqueId(std::move(id)), mEndpointRegistration(*this, {}) {}
    virtual ~Device() = default;

    const std::string & GetUniqueId() const { return mUniqueId; }
    EndpointId GetEndpointId() const { return mEndpointId; }

    virtual DeviceType GetDeviceType() const = 0;

    /// Register relevant clusters on the given endpoint
    virtual CHIP_ERROR Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider,
                                EndpointId parentId = kInvalidEndpointId) = 0;

    /// Remove clusters from the given provider.
    ///
    /// Will only be called if register has succeeded before
    virtual void UnRegister(CodeDrivenDataModelProvider & provider) = 0;

    // Endpoint interface implementation
    CHIP_ERROR SemanticTags(ReadOnlyBufferBuilder<SemanticTag> & out) const override;
    CHIP_ERROR DeviceTypes(ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out) const override;
    CHIP_ERROR ClientClusters(ReadOnlyBufferBuilder<ClusterId> & out) const override;

protected:
    /// Internal registration functions for common device clusters
    /// Subclasses are expected to call these
    CHIP_ERROR RegisterDescriptor(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider,
                                  const Clusters::DescriptorCluster::DeviceType & deviceType, EndpointId parentId);
    CHIP_ERROR UnRegisterBridgedNodeClusters(CodeDrivenDataModelProvider & provider);

    chip::EndpointId mEndpointId = kInvalidEndpointId;
    std::string mUniqueId;
    Clusters::DescriptorCluster::DeviceType mDeviceType;
    EndpointInterfaceRegistration mEndpointRegistration;

    // Common clusters..
    LazyRegisteredServerCluster<Clusters::DescriptorCluster> mDescriptorCluster;
};

} // namespace chip::app
