#include "descriptor-cluster.h"

#include <app/server-cluster/AttributeListBuilder.h>
#include <clusters/Descriptor/ClusterId.h>
#include <clusters/Descriptor/Metadata.h>

using namespace chip::app::Clusters::Descriptor;
using namespace chip::app::Clusters::Descriptor::Attributes;
using chip::Protocols::InteractionModel::Status;

namespace chip::app::Clusters {
namespace {

constexpr DataModel::AttributeEntry kMandatoryAttributes[] = {
    DeviceTypeList::kMetadataEntry,
    ServerList::kMetadataEntry,
    ClientList::kMetadataEntry,
    PartsList::kMetadataEntry,
};

} // namespace

CHIP_ERROR DescriptorCluster::Attributes(const ConcreteClusterPath & path,
                                         ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    AttributeListBuilder listBuilder(builder);
    return listBuilder.Append(Span(kMandatoryAttributes), {});
}
DataModel::ActionReturnStatus DescriptorCluster::ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                               AttributeValueEncoder & encoder)
{
    switch (request.path.mAttributeId)
    {
    case FeatureMap::Id:
        return encoder.Encode(BitFlags<Descriptor::Feature>{ 0 }); // TODO: more features support
    case ClusterRevision::Id:
        return encoder.Encode(Descriptor::kRevision);
    case DeviceTypeList::Id:
        return encoder.EncodeList([this](const auto & itemEncoder) -> CHIP_ERROR {
            for (const auto & entry : mDeviceTypes)
            {
                ReturnErrorOnFailure(itemEncoder.Encode(entry));
            }
            return CHIP_NO_ERROR;
        });
    case ServerList::Id: {
        ReadOnlyBufferBuilder<DataModel::ServerClusterEntry> builder;
        ReturnErrorOnFailure(mContext->provider->ServerClusters(mPath.mEndpointId, builder));
        ReadOnlyBuffer<DataModel::ServerClusterEntry> buffer = builder.TakeBuffer();
        return encoder.EncodeList([&buffer](const auto & itemEncoder) -> CHIP_ERROR {
            for (const auto & entry : buffer)
            {
                ReturnErrorOnFailure(itemEncoder.Encode(entry.clusterId));
            }
            return CHIP_NO_ERROR;
        });
    }
    case ClientList::Id: {
        ReadOnlyBufferBuilder<ClusterId> builder;
        ReturnErrorOnFailure(mContext->provider->ClientClusters(mPath.mEndpointId, builder));
        ReadOnlyBuffer<ClusterId> buffer = builder.TakeBuffer();
        return encoder.EncodeList([&buffer](const auto & itemEncoder) -> CHIP_ERROR {
            for (const auto & clusterId : buffer)
            {
                ReturnErrorOnFailure(itemEncoder.Encode(clusterId));
            }
            return CHIP_NO_ERROR;
        });
    }
    case PartsList::Id:
        return encoder.EncodeEmptyList();
    default:
        return Status::UnsupportedAttribute;
    }
}

} // namespace chip::app::Clusters
