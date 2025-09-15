/**
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

#include <app/clusters/boolean-state-server/boolean-state-cluster.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>

// Code driven clusters made for this app, these are clusters that
// need some additional work before landing upstream
#include <cluster-impl/identify-cluster.h>

namespace chip::app {

// Represents a contact sensor node
class ContactSensor : public EndpointInterface
{
public:
    ContactSensor(EndpointId parentEndpointId, EndpointId endpointId);

    CHIP_ERROR Register(CodeDrivenDataModelProvider & dataModelProvider);

    // Endpoint interface
    CHIP_ERROR SemanticTags(ReadOnlyBufferBuilder<SemanticTag> & out) const override;
    CHIP_ERROR DeviceTypes(ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out) const override;
    CHIP_ERROR ClientClusters(ReadOnlyBufferBuilder<ClusterId> & out) const override;

private:
    EndpointInterfaceRegistration mRegistration;

    RegisteredServerCluster<Clusters::BooleanStateCluster> mBooleanStateCluster;
    RegisteredServerCluster<Clusters::IdentifyCluster> mIdentifyCluster;
};

} // namespace chip::app
