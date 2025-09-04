/**
 *
 *    Copyright (c) 2025 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include <app-common/zap-generated/cluster-objects.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/OptionalAttributeSet.h>
#include <clusters/Groups/ClusterId.h>
#include <clusters/Groups/Metadata.h>
#include <platform/DiagnosticDataProvider.h>
#include "identify-cluster.h"

namespace chip {
namespace app {
namespace Clusters {

class GroupsCluster : public DefaultServerCluster
{
public:
    //TODO: In CodegenIntegration, need to add code to check if the device is identifying, which can still be based on the old
    // non code driven implementation for now. This will need to be part of an implemenation for IsDeviceIdentifying
    GroupsCluster(EndpointId endpoint, BitFlags<Groups::Feature> featureFlags, const IsDeviceIdentifying & deviceIdentifying) : DefaultServerCluster({endpoint , Groups::Id}), 
                                                                  mFeatureFlags(featureFlags), isIdentifyingBool(deviceIdentifying.IsIdentifying()) {}

    std::optional<DataModel::ActionReturnStatus> InvokeCommand(const DataModel::InvokeRequest & request,
                                                               chip::TLV::TLVReader & input_arguments,
                                                               CommandHandler * handler) override;

    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                AttributeValueEncoder & encoder) override;

    CHIP_ERROR Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;

    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath & path,
                                ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder) override;

    CHIP_ERROR GeneratedCommands(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<CommandId> & builder) override;

private:
    BitFlags<Groups::Feature> mFeatureFlags;
    Groups::NameSupportBitmap mNameSupport = Groups::NameSupportBitmap::kGroupNames;
    bool isIdentifyingBool;

};

} // namespace Clusters
} // namespace app
} // namespace chip
