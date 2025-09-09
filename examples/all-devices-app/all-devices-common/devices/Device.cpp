#include "Device.h"
#include <memory>

using namespace chip::app::Clusters;

namespace chip::app {

constexpr uint16_t kBridgedNodeDeviceTypeRevision = 3;

CHIP_ERROR Device::RegisterDescriptor(chip::EndpointId endpoint, CodeDrivenDataModelProvider & provider,
                                      const Clusters::DescriptorCluster::DeviceType & deviceType, EndpointId parentId)
{
    VerifyOrReturnError(mEndpointId == kInvalidEndpointId, CHIP_ERROR_INCORRECT_STATE);
    mDeviceType = deviceType;
    mEndpointId = endpoint;

    mDescriptorCluster.Create(endpoint, BitFlags<Descriptor::Feature>(0));
    ReturnErrorOnFailure(provider.AddCluster(mDescriptorCluster.Registration()));

    mEndpointRegistration.endpointEntry = DataModel::EndpointEntry{
        .id                 = endpoint, //
        .parentId           = parentId, //
        .compositionPattern = GetDeviceType() == DeviceType::kBridgedNodeDevice
            ? DataModel::EndpointCompositionPattern::kTree
            : DataModel::EndpointCompositionPattern::kFullFamily,
    };
    return CHIP_NO_ERROR;
}

CHIP_ERROR Device::UnRegisterBridgedNodeClusters(CodeDrivenDataModelProvider & provider)
{
    ReturnErrorOnFailure(provider.RemoveCluster(&mDescriptorCluster.Cluster()));

    mEndpointId = kInvalidEndpointId;

    return CHIP_NO_ERROR;
}

CHIP_ERROR Device::DeviceTypes(ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out) const
{
    ReturnErrorOnFailure(out.EnsureAppendCapacity(1));

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

#if CHIP_CONFIG_USE_ENDPOINT_UNIQUE_ID
MutableCharSpan Device::EndpointUniqueId() const {
    return {};
}
#endif

} // namespace chip::app
