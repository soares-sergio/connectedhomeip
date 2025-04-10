/*
 *    Copyright (c) 2025 Project CHIP Authors
 *    All rights reserved.
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
#include <app/data-model-provider/MetadataTypes.h>
#include <app/server-cluster/ServerClusterInterface.h>
#include <lib/core/DataModelTypes.h>
#include <lib/support/ReadOnlyBuffer.h>

namespace chip {
namespace app {

using SemanticTag = chip::app::Clusters::Descriptor::Structs::SemanticTagStruct::Type;

/**
 * @brief Defines the interface for an object that can provide information about a Matter endpoint.
 *
 * This interface is used by the CodeDrivenDataModelProvider to discover the structure and capabilities
 * of an endpoint, including its device types, client clusters, server clusters, and semantic tags.
 *
 * Implementations of this interface are responsible for providing instances of ServerClusterInterface
 * for each server cluster they expose.
 *
 * Lifecycle Management:
 * Instances of EndpointProviderInterface are typically registered with a CodeDrivenDataModelProvider.
 * The CodeDrivenDataModelProvider stores pointers to these instances but does NOT take ownership.
 * It is crucial that the lifetime of any EndpointProviderInterface instance outlives the
 * CodeDrivenDataModelProvider it is registered with.
 */
class EndpointProviderInterface
{
public:
    virtual ~EndpointProviderInterface() = default;

    virtual const DataModel::EndpointEntry & GetEndpointEntry() const                                       = 0;
    virtual CHIP_ERROR SemanticTags(ReadOnlyBufferBuilder<SemanticTag> & out)                               = 0;
    virtual CHIP_ERROR DeviceTypes(ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out)                 = 0;
    virtual CHIP_ERROR ClientClusters(ReadOnlyBufferBuilder<ClusterId> & out)                               = 0;
    virtual ServerClusterInterface * GetServerCluster(ClusterId clusterId)                                  = 0;
    virtual CHIP_ERROR ServerClusterInterfaces(ReadOnlyBufferBuilder<ServerClusterInterface *> & out) const = 0;
};

} // namespace app
} // namespace chip
