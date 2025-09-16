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

#include <app/clusters/administrator-commissioning-server/AdministratorCommissioningCluster.h>
#include <app/clusters/basic-information/BasicInformationCluster.h>
#include <app/clusters/descriptor/descriptor-cluster.h>
#include <app/clusters/general-commissioning-server/general-commissioning-cluster.h>
#include <app/clusters/general-diagnostics-server/general-diagnostics-cluster.h>
#include <app/clusters/group-key-mgmt-server/group-key-mgmt-cluster.h>
#include <app/clusters/network-commissioning/network-commissioning.h>
#include <app/clusters/software-diagnostics-server/software-diagnostics-cluster.h>
#include <app/clusters/wifi-network-diagnostics-server/wifi-network-diagnostics-cluster.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>

// Code driven clusters made for this app, these are clusters that
// need some additional work before landing upstream
#include <cluster-impl/access-control-cluster.h>
#include <cluster-impl/identify-cluster.h>
#include <cluster-impl/operational-credentials-cluster.h>

namespace chip::app {

/// Represents a root endpoint
class RootEndpoint : public EndpointInterface
{
public:
    ~RootEndpoint() override = default;

    /// Adds all relevant clusters on the given provider.
    ///
    /// and returns the necessary registration for it
    virtual CHIP_ERROR Register(CodeDrivenDataModelProvider & dataModelProvider);

    // Endpoint interface implementation
    CHIP_ERROR SemanticTags(ReadOnlyBufferBuilder<SemanticTag> & out) const override;
    CHIP_ERROR DeviceTypes(ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out) const override;
    CHIP_ERROR ClientClusters(ReadOnlyBufferBuilder<ClusterId> & out) const override;

protected:
    // Most implementations require network commissioning, so only subclasses have access to this.
    RootEndpoint();

private:
    EndpointInterfaceRegistration mRegistration;

    RegisteredServerCluster<Clusters::GeneralCommissioningCluster> mGeneralCommissioningCluster;
    RegisteredServerCluster<Clusters::AdministratorCommissioningWithBasicCommissioningWindowCluster> mAdminCommissioningCluster;
    RegisteredServerCluster<Clusters::GeneralDiagnosticsCluster> mGeneralDiagnosticsCluster;
    RegisteredServerCluster<Clusters::GroupKeyManagementCluster> mGroupKeyManagementCluster;
    RegisteredServerCluster<Clusters::SoftwareDiagnosticsServerCluster> mSoftwareDiagnosticsCluster;
    RegisteredServerCluster<Clusters::DescriptorCluster> mDescriptorCluster;
    RegisteredServerCluster<Clusters::AccessControlCluster> mAccessControlCluster;
    RegisteredServerCluster<Clusters::OperationalCredentialsCluster> mOperationalCredentialsCluster;

    // This uses a singleton ... not great
    ServerClusterRegistration mBasicInformationClusterRegistration;
};

/// A root endpoint that supports wifi network commissioning
class WiFiRootEndpoint : public RootEndpoint
{
public:
    WiFiRootEndpoint(DeviceLayer::NetworkCommissioning::WiFiDriver & wifiDriver);
    ~WiFiRootEndpoint() override = default;

    CHIP_ERROR Register(CodeDrivenDataModelProvider & dataModelProvider) override;

private:
    RegisteredServerCluster<Clusters::NetworkCommissioningCluster> mNetworkCommissioningCluster;
    RegisteredServerCluster<Clusters::WiFiDiagnosticsServerCluster> mWifiDiagnosticsCluster;
};

} // namespace chip::app
