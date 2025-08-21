#include "descriptor-cluster.h"

#include <app/server-cluster/AttributeListBuilder.h>
#include <clusters/Descriptor/ClusterId.h>
#include <clusters/Descriptor/Metadata.h>
#include <lib/core/DataModelTypes.h>

using namespace chip::app::Clusters::Descriptor;
using namespace chip::app::Clusters::Descriptor::Attributes;
using chip::Protocols::InteractionModel::Status;

namespace chip::app::Clusters {

namespace {
/// Figures out if `childId` is a descendant of `parentId` given some specific endpoint entries
bool IsDescendantOf(const DataModel::EndpointEntry * __restrict__ childEndpoint, const EndpointId parentId,
                    Span<const DataModel::EndpointEntry> allEndpoints)
{
    // NOTE: this is not very efficient as we loop through all endpoints for each parent search
    //       however endpoint depth should not be as large.
    while (true)
    {
        VerifyOrReturnValue(childEndpoint != nullptr, false);
        VerifyOrReturnValue(childEndpoint->parentId != parentId, true);

        // Parent endpoint id 0 is never here: EndpointEntry::parentId uses
        // kInvalidEndpointId to reference no explicit endpoint. See `EndpointEntry`
        // comments.
        VerifyOrReturnValue(childEndpoint->parentId != kInvalidEndpointId, false);

        const auto lookupId = childEndpoint->parentId;
        childEndpoint       = nullptr; // we will look it up again

        // find the requested value in the array to get its parent
        for (const auto & ep : allEndpoints)
        {
            if (ep.id == lookupId)
            {
                childEndpoint = &ep;
                break;
            }
        }
    }
}
}

CHIP_ERROR DescriptorCluster::Attributes(const ConcreteClusterPath & path,
                                         ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    AttributeListBuilder listBuilder(builder);
    return listBuilder.Append(Span(Descriptor::Attributes::kMandatoryMetadata), {});
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
        ReturnErrorOnFailure(mContext->provider.ServerClusters(mPath.mEndpointId, builder));
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
        ReturnErrorOnFailure(mContext->provider.ClientClusters(mPath.mEndpointId, builder));
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
        // return ReadPartsAttribute(request.path.mEndpointId, encoder);
    default:
        return Status::UnsupportedAttribute;
    }
}

CHIP_ERROR DescriptorCluster::ReadPartsAttribute(EndpointId endpoint, AttributeValueEncoder & aEncoder)
{
    ReadOnlyBufferBuilder<DataModel::EndpointEntry> endpointsList;
    ReturnErrorOnFailure(mContext->provider.Endpoints(endpointsList));
    auto endpoints = endpointsList.TakeBuffer();
    if (endpoint == 0x00)
    {
        return aEncoder.EncodeList([&endpoints](const auto & encoder) -> CHIP_ERROR {
            for (const auto & ep : endpoints)
            {
                if (ep.id == 0)
                {
                    continue;
                }
                ReturnErrorOnFailure(encoder.Encode(ep.id));
            }
            return CHIP_NO_ERROR;
        });
    }

    // find the given endpoint
    unsigned idx = 0;
    while (idx < endpoints.size())
    {
        if (endpoints[idx].id == endpoint)
        {
            break;
        }
        idx++;
    }
    if (idx >= endpoints.size())
    {
        // not found
        return CHIP_ERROR_NOT_FOUND;
    }

    auto & endpointInfo = endpoints[idx];

    switch (endpointInfo.compositionPattern)
    {
    case DataModel::EndpointCompositionPattern::kFullFamily:
        // encodes ALL endpoints that have the specified endpoint as a descendant.
        return aEncoder.EncodeList([&endpoints, endpoint](const auto & encoder) -> CHIP_ERROR {
            for (const auto & ep : endpoints)
            {
                if (IsDescendantOf(&ep, endpoint, endpoints))
                {
                    ReturnErrorOnFailure(encoder.Encode(ep.id));
                }
            }
            return CHIP_NO_ERROR;
        });

    case DataModel::EndpointCompositionPattern::kTree:
        return aEncoder.EncodeList([&endpoints, endpoint](const auto & encoder) -> CHIP_ERROR {
            for (const auto & ep : endpoints)
            {
                if (ep.parentId != endpoint)
                {
                    continue;
                }
                ReturnErrorOnFailure(encoder.Encode(ep.id));
            }
            return CHIP_NO_ERROR;
        });
    }
    // not actually reachable and compiler will validate we
    // handle all switch cases above
    return CHIP_NO_ERROR;
}

} // namespace chip::app::Clusters
