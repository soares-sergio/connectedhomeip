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

#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/OptionalAttributeSet.h>
#include <clusters/Identify/ClusterId.h>
#include <clusters/Identify/Metadata.h>

namespace chip {
namespace app {
namespace Clusters {

class IsDeviceIdentifying {
public:
    virtual ~IsDeviceIdentifying() = default;
    virtual bool IsIdentifying() const = 0;
};

class IdentifyCluster : public DefaultServerCluster, public IsDeviceIdentifying
{
public:
    ~IdentifyCluster() override = default;
    IdentifyCluster(EndpointId endpointId) : DefaultServerCluster({ endpointId, Identify::Id }) {}

    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                AttributeValueEncoder & encoder) override;

    DataModel::ActionReturnStatus WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                 AttributeValueDecoder & decoder) override;

    CHIP_ERROR Attributes(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<DataModel::AttributeEntry> & builder) override;

    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath & path,
                                ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder) override;

    std::optional<DataModel::ActionReturnStatus> InvokeCommand(const DataModel::InvokeRequest & request,
                                                               chip::TLV::TLVReader & input_arguments,
                                                               CommandHandler * handler) override;

    //TODO: Should add a proper implementation of this. This is needed as part of the Groups
    // cluster, the old ember-based implementation checked if a device is identifying based on 
    // the identifyTime. The Groups cluster now takes a variable of IsDeviceIdentifying in its constructor
    // that should handle this properly.
    bool IsIdentifying() const override {
        return false;
    }
};

} // namespace Clusters
} // namespace app
} // namespace chip
