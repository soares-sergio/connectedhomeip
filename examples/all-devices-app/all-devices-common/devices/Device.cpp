#include "Device.h"
#include <memory>

using namespace chip::app::Clusters;

namespace chip::app {

constexpr DeviceTypeId kBridgedNodeDeviceType     = 0x0013;
constexpr uint16_t kBridgedNodeDeviceTypeRevision = 3;

CHIP_ERROR Device::RegisterBridgedNodeClusters(chip::EndpointId endpoint,
                                               const Clusters::DescriptorCluster::DeviceType & deviceType,
                                               CodeDrivenDataModelProvider & provider)
{
    VerifyOrReturnError(mEndpointId == kInvalidEndpointId, CHIP_ERROR_INCORRECT_STATE);

    mDeviceType = deviceType;

    /// std::initializer_list does not work well with std::forward, so use the
    /// vector constructor instead
    std::vector<DescriptorCluster::DeviceType> deviceTypes{ {
                                                                .deviceType = kBridgedNodeDeviceType,
                                                                .revision   = kBridgedNodeDeviceTypeRevision,
                                                            },
                                                            {
                                                                .deviceType = deviceType.deviceType,
                                                                .revision   = deviceType.revision,
                                                            } };

    mDescriptorCluster.Create(endpoint, deviceTypes);
    mBridgedDeviceInfoCluster.Create(endpoint, mUniqueId);

    ReturnErrorOnFailure(provider.AddCluster(mDescriptorCluster.Registration()));
    ReturnErrorOnFailure(provider.AddCluster(mBridgedDeviceInfoCluster.Registration()));
    mEndpointId = endpoint;

    return CHIP_NO_ERROR;
}

CHIP_ERROR Device::UnRegisterBridgedNodeClusters(CodeDrivenDataModelProvider & provider)
{
    ReturnErrorOnFailure(provider.RemoveCluster(&mDescriptorCluster.Cluster()));
    ReturnErrorOnFailure(provider.RemoveCluster(&mBridgedDeviceInfoCluster.Cluster()));

    mEndpointId = kInvalidEndpointId;

    return CHIP_NO_ERROR;
}

CHIP_ERROR Device::DeviceTypes(ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out) const
{
    ReturnErrorOnFailure(out.EnsureAppendCapacity(2));

    ReturnErrorOnFailure(out.Append(DataModel::DeviceTypeEntry{
        .deviceTypeId       = kBridgedNodeDeviceType,
        .deviceTypeRevision = kBridgedNodeDeviceTypeRevision,
    }));

    if (mDescriptorCluster.IsConstructed())
    {
        ReturnErrorOnFailure(out.Append(DataModel::DeviceTypeEntry{
            .deviceTypeId       = mDeviceType.deviceType,
            .deviceTypeRevision = static_cast<uint8_t>(mDeviceType.revision),
        }));
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR Device::SemanticTags(ReadOnlyBufferBuilder<SemanticTag> & out) const
{
    // no semantic tags
    return CHIP_NO_ERROR;
}

CHIP_ERROR Device::ClientClusters(ReadOnlyBufferBuilder<ClusterId> & out) const
{
    // no bindings
    return CHIP_NO_ERROR;
}

} // namespace chip::app
