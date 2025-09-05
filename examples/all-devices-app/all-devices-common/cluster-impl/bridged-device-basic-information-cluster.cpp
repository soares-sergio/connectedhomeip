#include "bridged-device-basic-information-cluster.h"

#include <app/server-cluster/AttributeListBuilder.h>
#include <clusters/BridgedDeviceBasicInformation/ClusterId.h>
#include <clusters/BridgedDeviceBasicInformation/Metadata.h>

using namespace chip::app::Clusters::BridgedDeviceBasicInformation;
using namespace chip::app::Clusters::BridgedDeviceBasicInformation::Attributes;
using chip::Protocols::InteractionModel::Status;

namespace chip::app::Clusters {

CHIP_ERROR BridgedDeviceBasicInformationCluster::Attributes(const ConcreteClusterPath & path,
                                                            ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder)
{
    AttributeListBuilder listBuilder(builder);
    return listBuilder.Append(Span(BridgedDeviceBasicInformation::Attributes::kMandatoryMetadata), {});
}

DataModel::ActionReturnStatus BridgedDeviceBasicInformationCluster::ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                                                  AttributeValueEncoder & encoder)
{
    switch (request.path.mAttributeId)
    {
    case FeatureMap::Id:
        return encoder.Encode(BitFlags<BridgedDeviceBasicInformation::Feature>{ 0 }); // TODO: more features support
    case ClusterRevision::Id:
        return encoder.Encode(BridgedDeviceBasicInformation::kRevision);
    case Reachable::Id:
        return encoder.Encode(true); // for now claim always reachable
    case UniqueID::Id:
        return encoder.Encode(Span(mUniqueID.c_str(), mUniqueID.size())); // for now claim always reachable
    default:
        return Status::UnsupportedAttribute;
    }
}

} // namespace chip::app::Clusters
