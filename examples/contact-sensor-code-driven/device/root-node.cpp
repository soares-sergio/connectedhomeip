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

#include "root-node.h"

using namespace chip::app::Clusters;

namespace chip::app {

RootEndpoint::RootEndpoint(NetworkCommissioningCluster & networkCommissioningCluster) :
    mRegistration(*this,
                  {
                      .id                 = kRootEndpointId,
                      .parentId           = kInvalidEndpointId,
                      .compositionPattern = DataModel::EndpointCompositionPattern::kFullFamily,
                  }),
    mNetworkCommissioningClusterRegistration(networkCommissioningCluster)
{}

CHIP_ERROR RootEndpoint::SemanticTags(ReadOnlyBufferBuilder<SemanticTag> & out) const
{
    return CHIP_NO_ERROR;
}

CHIP_ERROR RootEndpoint::DeviceTypes(ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> & out) const
{
    constexpr uint16_t kRootDeviceType                            = 0x0016;
    constexpr uint16_t kRootDeviceVersion                         = 3;
    constexpr DataModel::DeviceTypeEntry kRootDeviceTypeEntries[] = { { .deviceTypeId       = kRootDeviceType,
                                                                        .deviceTypeRevision = kRootDeviceVersion } };
    return out.ReferenceExisting(kRootDeviceTypeEntries);
}

CHIP_ERROR RootEndpoint::ClientClusters(ReadOnlyBufferBuilder<ClusterId> & out) const
{
    return CHIP_NO_ERROR;
}

CHIP_ERROR RootEndpoint::Register(CodeDrivenDataModelProvider & dataModelProvider)
{
    static GeneralCommissioningCluster clusterGeneralCommissioning({}, {});
    static ServerClusterRegistration serverClusterGeneralCommissioning(clusterGeneralCommissioning);
    ReturnErrorOnFailure(dataModelProvider.AddCluster(serverClusterGeneralCommissioning));

    static AdministratorCommissioningWithBasicCommissioningWindowCluster clusterAdminCommissioning(0, {});
    static ServerClusterRegistration serverClusterAdminCommissioning(clusterAdminCommissioning);
    ReturnErrorOnFailure(dataModelProvider.AddCluster(serverClusterAdminCommissioning));

    static ServerClusterRegistration basicInfo(BasicInformationCluster::Instance());
    BasicInformationCluster()
        .Instance()
        .OptionalAttributes()
        .Set<BasicInformation::Attributes::ManufacturingDate::Id>()
        .Set<BasicInformation::Attributes::PartNumber::Id>()
        .Set<BasicInformation::Attributes::ProductURL::Id>()
        .Set<BasicInformation::Attributes::ProductLabel::Id>()
        .Set<BasicInformation::Attributes::SerialNumber::Id>()
        .Set<BasicInformation::Attributes::LocalConfigDisabled::Id>()
        .Set<BasicInformation::Attributes::Reachable::Id>()
        .Set<BasicInformation::Attributes::ProductAppearance::Id>();

    ReturnErrorOnFailure(dataModelProvider.AddCluster(basicInfo));

    static GeneralDiagnosticsCluster clusterGeneralDiagnostics({});
    static ServerClusterRegistration generalDiagnostics(clusterGeneralDiagnostics);
    ReturnErrorOnFailure(dataModelProvider.AddCluster(generalDiagnostics));

    static GroupKeyManagementCluster clusterGroupKeyManagement;
    static ServerClusterRegistration groupKeyManagement(clusterGroupKeyManagement);
    ReturnErrorOnFailure(dataModelProvider.AddCluster(groupKeyManagement));

    static SoftwareDiagnosticsServerCluster clusterSoftwareDiagnostics({});
    static ServerClusterRegistration softwareDiagnostics(clusterSoftwareDiagnostics);
    ReturnErrorOnFailure(dataModelProvider.AddCluster(softwareDiagnostics));

    static WiFiDiagnosticsServerCluster clusterWifiDiagnostics(0, DeviceLayer::GetDiagnosticDataProvider(), {}, {});
    static ServerClusterRegistration wifiDiagnostics(clusterWifiDiagnostics);
    ReturnErrorOnFailure(dataModelProvider.AddCluster(wifiDiagnostics));

    static DescriptorCluster clusterDescriptor(0, BitFlags<Descriptor::Feature>(0));
    static ServerClusterRegistration descriptor(clusterDescriptor);
    ReturnErrorOnFailure(dataModelProvider.AddCluster(descriptor));

    static AccessControlCluster clusterAccessControl({});
    static ServerClusterRegistration accessControl(clusterAccessControl);
    ReturnErrorOnFailure(dataModelProvider.AddCluster(accessControl));

    static OperationalCredentialsCluster clusterOperationalCredenitals(0);
    static ServerClusterRegistration operationalCredentials(clusterOperationalCredenitals);
    ReturnErrorOnFailure(dataModelProvider.AddCluster(operationalCredentials));

    ReturnErrorOnFailure(dataModelProvider.AddCluster(mNetworkCommissioningClusterRegistration));

    return dataModelProvider.AddEndpoint(mRegistration);
}

} // namespace chip::app
