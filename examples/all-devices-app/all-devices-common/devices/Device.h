#pragma once

#include <clusters/bridged-device-basic-information-cluster.h>
#include <clusters/descriptor-cluster.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>

#include <string>

namespace chip::app {

/// A list of supported devices, since some of our methods want to
/// know the device type.
enum class BridgedDeviceType
{
    kBridgedNodeDevice,
    kContactSensor,
    kOccupancySensor,
};

/// A device is a entity that is uniquely identified (unique id string)
/// and maintains some cluster functionality.
///
/// Current implementation assumes that a device is registered on a single
/// endpoint.
class Device : public EndpointInterface
{
public:
    Device(std::string id) : mUniqueId(std::move(id)) {}
    virtual ~Device() = default;

    const std::string & GetUniqueId() const { return mUniqueId; }
    EndpointId GetEndpointId() const { return mEndpointId; }

    virtual BridgedDeviceType GetDeviceType() const = 0;

    /// Register relevant clusters on the given endpoint
    virtual CHIP_ERROR Register(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider) = 0;

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
    CHIP_ERROR RegisterBridgedNodeClusters(chip::EndpointId endpoint, const Clusters::DescriptorCluster::DeviceType & deviceType,
                                           CodeDrivenDataModelProvider & provider);
    CHIP_ERROR RegisterDescriptor(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider, const Clusters::DescriptorCluster::DeviceType & deviceType);
    CHIP_ERROR UnRegisterBridgedNodeClusters(CodeDrivenDataModelProvider & provider);

    chip::EndpointId mEndpointId = kInvalidEndpointId;
    std::string mUniqueId;
    Clusters::DescriptorCluster::DeviceType mDeviceType;

    // Common clusters..
    LazyRegisteredServerCluster<Clusters::DescriptorCluster> mDescriptorCluster;
    // LazyRegisteredServerCluster<Clusters::BridgedDeviceBasicInformationCluster> mBridgedDeviceInfoCluster;
};

} // namespace chip::app
