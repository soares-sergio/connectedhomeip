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

RootEndpoint::RootEndpoint() :
    mRegistration(*this,
                  {
                      .id                 = kRootEndpointId,
                      .parentId           = kInvalidEndpointId,
                      .compositionPattern = DataModel::EndpointCompositionPattern::kFullFamily,
                  }),
    mGeneralCommissioningCluster(BitFlags<GeneralCommissioning::Feature>(), GeneralCommissioningCluster::OptionalAttributes()),
    mAdminCommissioningCluster(kRootEndpointId, BitFlags<AdministratorCommissioning::Feature>()),
    mGeneralDiagnosticsCluster(GeneralDiagnosticsCluster::OptionalAttributeSet()), //
    mGroupKeyManagementCluster(),                                                  //
    mSoftwareDiagnosticsCluster(SoftwareDiagnosticsLogic::OptionalAttributeSet()),
    mDescriptorCluster(kRootEndpointId, BitFlags<Descriptor::Feature>(0)),
    mAccessControlCluster(AccessControlCluster::OptionalAttributeSet()), //
    mOperationalCredentialsCluster(kRootEndpointId),                     //
    mBasicInformationClusterRegistration(BasicInformationCluster::Instance())
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
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mGeneralCommissioningCluster.Registration()));
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mAdminCommissioningCluster.Registration()));

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
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mBasicInformationClusterRegistration));

    ReturnErrorOnFailure(dataModelProvider.AddCluster(mGeneralDiagnosticsCluster.Registration()));
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mGroupKeyManagementCluster.Registration()));
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mSoftwareDiagnosticsCluster.Registration()));
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mDescriptorCluster.Registration()));
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mAccessControlCluster.Registration()));
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mOperationalCredentialsCluster.Registration()));

    return dataModelProvider.AddEndpoint(mRegistration);
}

WiFiRootEndpoint::WiFiRootEndpoint(DeviceLayer::NetworkCommissioning::WiFiDriver & wifiDriver) :
    mNetworkCommissioningCluster(kRootEndpointId, &wifiDriver),
    mWifiDiagnosticsCluster(kRootEndpointId, DeviceLayer::GetDiagnosticDataProvider(),
                            WiFiDiagnosticsServerLogic::OptionalAttributeSet(), BitFlags<WiFiNetworkDiagnostics::Feature>())
{}

CHIP_ERROR
WiFiRootEndpoint::Register(CodeDrivenDataModelProvider & dataModelProvider)
{
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mNetworkCommissioningCluster.Registration()));
    ReturnErrorOnFailure(dataModelProvider.AddCluster(mWifiDiagnosticsCluster.Registration()));

    return RootEndpoint::Register(dataModelProvider);
}

} // namespace chip::app
