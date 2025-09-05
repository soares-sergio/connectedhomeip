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
#include <clusters/OnOff/ClusterId.h>
#include <clusters/OnOff/Metadata.h>
#include <platform/DiagnosticDataProvider.h>

namespace chip {
namespace app {
namespace Clusters {

class OnOffCluster : public DefaultServerCluster
{
public:
    using OptionalAttributeSet =
        chip::app::OptionalAttributeSet<OnOff::Attributes::GlobalSceneControl::Id, OnOff::Attributes::OnTime::Id,
                                        OnOff::Attributes::OffWaitTime::Id, OnOff::Attributes::StartUpOnOff::Id>;

    OnOffCluster(EndpointId endpoint, OptionalAttributeSet optionalAttributeSet, BitFlags<OnOff::Feature> featureFlags) :
        DefaultServerCluster({ endpoint, OnOff::Id }), mFeatureFlags(featureFlags)
    {}

    std::optional<DataModel::ActionReturnStatus> InvokeCommand(const DataModel::InvokeRequest & request,
                                                               chip::TLV::TLVReader & input_arguments,
                                                               CommandHandler * handler) override;

    CHIP_ERROR Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;

    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                AttributeValueEncoder & encoder) override;

    DataModel::ActionReturnStatus WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                 AttributeValueDecoder & decoder) override;

    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath & path,
                                ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder) override;

    void SetOnOff(bool value);

private:
    OptionalAttributeSet mOptionalAttributeSet;
    BitFlags<OnOff::Feature> mFeatureFlags;

    // Attributes
    bool mOnOff                           = false;
    bool mGlobalSceneControl              = false;
    bool mOnTime                          = 0;
    bool mOffWaitTime                     = 0;
    OnOff::StartUpOnOffEnum mStartUpOnOff = OnOff::StartUpOnOffEnum::kOn;
};

} // namespace Clusters
} // namespace app
} // namespace chip
