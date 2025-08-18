/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
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

#include <TracingCommandLineArgument.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/clusters/administrator-commissioning-server/AdministratorCommissioningCluster.h>
#include <app/clusters/basic-information/BasicInformationCluster.h>
#include <app/clusters/general-diagnostics-server/general-diagnostics-cluster.h>
#include <app/clusters/group-key-mgmt-server/group-key-mgmt-cluster.h>
#include <app/clusters/network-commissioning/network-commissioning.h>
#include <app/clusters/software-diagnostics-server/software-diagnostics-cluster.h>
#include <app/clusters/wifi-network-diagnostics-server/wifi-network-diagnostics-cluster.h>
#include <app/persistence/DefaultAttributePersistenceProvider.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <app/server/Dnssd.h>
#include <app/server/Server.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/endpoint/SpanEndpoint.h>
#include <platform/CommissionableDataProvider.h>
#include <platform/Linux/NetworkCommissioningDriver.h>
#include <platform/PlatformManager.h>
#include <setup_payload/OnboardingCodesUtil.h>
#include <system/SystemLayer.h>

#include <DeviceInfoProviderImpl.h>
#include <LinuxCommissionableDataProvider.h>
#include <Rpc.h>
#include <credentials/examples/DeviceAttestationCredsExample.h>

// TODO: this should go away
#include <data-model-providers/codegen/Instance.h>
#include <server-cluster-shim/ServerClusterShim.h>

using namespace chip;
using namespace chip::app;
using namespace chip::Platform;
using namespace chip::DeviceLayer;
using namespace chip::app::Clusters;

namespace {

DataModel::DeviceTypeEntry deviceTypesEp0[] = {
    { 0x0016, 3 }, // ma_rootdevice, version 3
    { 0x0012, 1 }, // ma_otarequestor, version 1
};

DataModel::DeviceTypeEntry deviceTypesEp1[] = {
    { 0x0015, 2 }, // ma_contactsensor, version 2
};

ClusterId clientClustersEp0[] = { OtaSoftwareUpdateProvider::Id };

SpanEndpoint endpoint0 = SpanEndpoint::Builder()
                             .SetClientClusters(chip::Span<const ClusterId>(clientClustersEp0))
                             .SetDeviceTypes(Span<const DataModel::DeviceTypeEntry>(deviceTypesEp0))
                             .Build();
SpanEndpoint endpoint1 = SpanEndpoint::Builder().SetDeviceTypes(Span<const DataModel::DeviceTypeEntry>(deviceTypesEp1)).Build();

EndpointInterfaceRegistration endpointRegistration0(endpoint0,
                                                    { .id                 = 0,
                                                      .parentId           = kInvalidEndpointId,
                                                      .compositionPattern = DataModel::EndpointCompositionPattern::kFullFamily });

EndpointInterfaceRegistration endpointRegistration1(endpoint1,
                                                    { .id                 = 1,
                                                      .parentId           = kInvalidEndpointId,
                                                      .compositionPattern = DataModel::EndpointCompositionPattern::kFullFamily });

ServerClusterShim serverClusterShimEp0({
    // Endpoint 0
    { 0, Descriptor::Id },
    { 0, AccessControl::Id },
    { 0, OtaSoftwareUpdateRequestor::Id },
    { 0, chip::app::Clusters::NetworkCommissioning::Id }, // Spelled out to avoid ambigous namespace error.
    { 0, GeneralCommissioning::Id },
    { 0, DiagnosticLogs::Id },
    { 0, ThreadNetworkDiagnostics::Id },
    { 0, EthernetNetworkDiagnostics::Id },
    { 0, OperationalCredentials::Id },
    { 0, UserLabel::Id },
});

ServerClusterShim serverClusterShimEp1({ // Endpoint 1
                                         { 1, Identify::Id },
                                         { 1, Descriptor::Id },
                                         { 1, BooleanState::Id } });

ServerClusterRegistration serverClusterShimRegistrationEp0(serverClusterShimEp0);
ServerClusterRegistration serverClusterShimRegistrationEp1(serverClusterShimEp1);

// TODO: add a Span of these registrations

DeviceLayer::NetworkCommissioning::LinuxWiFiDriver sWiFiDriver;
Optional<Clusters::NetworkCommissioning::Instance> sWiFiNetworkCommissioningInstance;
DeviceInfoProviderImpl gExampleDeviceInfoProvider;

// To hold SPAKE2+ verifier, discriminator, passcode
LinuxCommissionableDataProvider gCommissionableDataProvider;

void StopSignalHandler(int /* signal */)
{
    Server::GetInstance().GenerateShutDownEvent();
    SystemLayer().ScheduleLambda([]() { PlatformMgr().StopEventLoopTask(); });
}

void EnableWiFiNetworkCommissioning(EndpointId endpoint)
{
    sWiFiNetworkCommissioningInstance.Emplace(endpoint, &sWiFiDriver);
    sWiFiNetworkCommissioningInstance.Value().Init();
}

void InitNetworkCommissioning()
{
    // TODO: we may need to figure this out
    sWiFiDriver.Set5gSupport(true);
    EnableWiFiNetworkCommissioning(kRootEndpointId);
}

[[maybe_unused]] chip::app::DataModel::Provider * PopulateCodegenDataModelProvider(PersistentStorageDelegate * delegate)
{
    chip::app::CodegenDataModelProvider & dataModelProvider = CodegenDataModelProvider::Instance();
    dataModelProvider.SetPersistentStorageDelegate(delegate);

    CHIP_ERROR err = dataModelProvider.Registry().Register(serverClusterShimRegistrationEp0);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Cannot register ServerClusterShim for EP0: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    err = dataModelProvider.Registry().Register(serverClusterShimRegistrationEp1);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Cannot register ServerClusterShim for EP1: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    return &dataModelProvider;
}

[[maybe_unused]] chip::app::DataModel::Provider * PopulateCodeDrivenDataModelProvider(PersistentStorageDelegate * delegate)
{
    ServerClusterShim::InitEmberShims();

    static chip::app::DefaultAttributePersistenceProvider attributePersistenceProvider;
    static chip::app::CodeDrivenDataModelProvider dataModelProvider =
        chip::app::CodeDrivenDataModelProvider(*delegate, attributePersistenceProvider);

    // Add Cluster registrations
    CHIP_ERROR err = dataModelProvider.AddCluster(serverClusterShimRegistrationEp0);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Cannot register ServerClusterShim for EP0: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    // register real code driven clusters
    static AdministratorCommissioningWithBasicCommissioningWindowCluster clusterAdminCommissioning(0, {});
    static ServerClusterRegistration serverClusterAdminCommissioning(clusterAdminCommissioning);
    VerifyOrDie(dataModelProvider.AddCluster(serverClusterAdminCommissioning) == CHIP_NO_ERROR);

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

    VerifyOrDie(dataModelProvider.AddCluster(basicInfo) == CHIP_NO_ERROR);

    static GeneralDiagnosticsCluster clusterGeneralDiagnostics({});
    static ServerClusterRegistration generalDiagnostics(clusterGeneralDiagnostics);
    VerifyOrDie(dataModelProvider.AddCluster(generalDiagnostics) == CHIP_NO_ERROR);

    static GroupKeyManagementCluster clusterGroupKeyManagement;
    static ServerClusterRegistration groupKeyManagement(clusterGroupKeyManagement);
    VerifyOrDie(dataModelProvider.AddCluster(groupKeyManagement) == CHIP_NO_ERROR);

    static SoftwareDiagnosticsServerCluster clusterSoftwareDiagnostics({});
    static ServerClusterRegistration softwareDiagnostics(clusterSoftwareDiagnostics);
    VerifyOrDie(dataModelProvider.AddCluster(softwareDiagnostics) == CHIP_NO_ERROR);

    static WiFiDiagnosticsServerCluster clusterWifiDiagnostics(0, DeviceLayer::GetDiagnosticDataProvider(), {}, {});
    static ServerClusterRegistration wifiDiagnostics(clusterWifiDiagnostics);
    VerifyOrDie(dataModelProvider.AddCluster(wifiDiagnostics) == CHIP_NO_ERROR);


    err = dataModelProvider.AddCluster(serverClusterShimRegistrationEp1);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Cannot register ServerClusterShim for EP1: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    // Add Endpoint Registrations
    err = dataModelProvider.AddEndpoint(endpointRegistration0);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Cannot register Endpoint 0: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    err = dataModelProvider.AddEndpoint(endpointRegistration1);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Cannot register Endpoint 1: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    return &dataModelProvider;
}

void StartApplication()
{
    static chip::CommonCaseDeviceServerInitParams initParams;
    VerifyOrDie(initParams.InitializeStaticResourcesBeforeServerInit() == CHIP_NO_ERROR);

    // FIXME: update DMP here!!!
    // initParams.dataModelProvider = PopulateCodegenDataModelProvider(initParams.persistentStorageDelegate);
    initParams.dataModelProvider             = PopulateCodeDrivenDataModelProvider(initParams.persistentStorageDelegate);
    initParams.operationalServicePort        = CHIP_PORT;
    initParams.userDirectedCommissioningPort = CHIP_UDC_PORT;
    initParams.interfaceId                   = Inet::InterfaceId::Null();

    chip::CommandLineApp::TracingSetup tracing_setup;
    tracing_setup.EnableTracingFor("json:log");

    // Init ZCL Data Model and CHIP App Server
    CHIP_ERROR err = Server::GetInstance().Init(initParams);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Server init failed: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    // Now that the server has started and we are done with our startup logging,
    // log our discovery/onboarding information again so it's not lost in the
    // noise.
    ConfigurationMgr().LogDeviceConfig();

    chip::PayloadContents payload;

    payload.version = 0;
    payload.rendezvousInformation.SetValue(RendezvousInformationFlag::kBLE);

    if (GetCommissionableDataProvider()->GetSetupPasscode(payload.setUpPINCode) != CHIP_NO_ERROR)
    {
        payload.setUpPINCode = CHIP_DEVICE_CONFIG_USE_TEST_SETUP_PIN_CODE;
    }

    uint16_t discriminator = 0;
    VerifyOrDie(GetCommissionableDataProvider()->GetSetupDiscriminator(discriminator) == CHIP_NO_ERROR);
    payload.discriminator.SetLongValue(discriminator);

    VerifyOrDie(chip::DeviceLayer::GetDeviceInstanceInfoProvider()->GetVendorId(payload.vendorID) == CHIP_NO_ERROR);
    VerifyOrDie(chip::DeviceLayer::GetDeviceInstanceInfoProvider()->GetProductId(payload.productID) == CHIP_NO_ERROR);
    PrintOnboardingCodes(payload);

    SetDeviceAttestationCredentialsProvider(Credentials::Examples::GetExampleDACProvider());

    InitNetworkCommissioning();

    struct sigaction sa = {};
    sa.sa_handler       = StopSignalHandler;
    sa.sa_flags         = SA_RESETHAND;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);

    DeviceLayer::PlatformMgr().RunEventLoop();
    Server::GetInstance().Shutdown();
    DeviceLayer::PlatformMgr().Shutdown();
    tracing_setup.StopTracing();
}

void EventHandler(const DeviceLayer::ChipDeviceEvent * event, intptr_t arg)
{
    (void) arg;
    if (event->Type == DeviceLayer::DeviceEventType::kCHIPoBLEConnectionEstablished)
    {
        ChipLogProgress(DeviceLayer, "Receive kCHIPoBLEConnectionEstablished");
    }
    else if ((event->Type == chip::DeviceLayer::DeviceEventType::kInternetConnectivityChange))
    {
        // Restart the server on connectivity change
        DnssdServer::Instance().StartServer();
    }
}

CHIP_ERROR InitCommissionableDataProvider(LinuxCommissionableDataProvider & provider)
{
    const auto discriminator             = static_cast<uint16_t>(CHIP_DEVICE_CONFIG_USE_TEST_SETUP_DISCRIMINATOR);
    const auto setupPasscode             = MakeOptional(static_cast<uint32_t>(CHIP_DEVICE_CONFIG_USE_TEST_SETUP_PIN_CODE));
    const uint32_t spake2pIterationCount = Crypto::kSpake2p_Min_PBKDF_Iterations;

    Optional<std::vector<uint8_t>> serializedSpake2pVerifier = NullOptional;
    Optional<std::vector<uint8_t>> spake2pSalt               = NullOptional;

    return provider.Init(          //
        serializedSpake2pVerifier, //
        spake2pSalt,               //
        spake2pIterationCount,     //
        setupPasscode,             //
        discriminator              //
    );
}

CHIP_ERROR Initialize()
{
    ChipLogProgress(AppServer, "Initializing...");
    ReturnErrorOnFailure(Platform::MemoryInit());
    ReturnErrorOnFailure(DeviceLayer::PersistedStorage::KeyValueStoreMgrImpl().Init(CHIP_CONFIG_KVS_PATH));
    ReturnErrorOnFailure(DeviceLayer::PlatformMgr().InitChipStack());

    ReturnErrorOnFailure(InitCommissionableDataProvider(gCommissionableDataProvider));
    DeviceLayer::SetCommissionableDataProvider(&gCommissionableDataProvider);
    DeviceLayer::SetDeviceInfoProvider(&gExampleDeviceInfoProvider);
    ConfigurationMgr().LogDeviceConfig();
    rpc::Init(33000);

    ReturnErrorOnFailure(DeviceLayer::PlatformMgrImpl().AddEventHandler(EventHandler, 0));
    ReturnErrorOnFailure(DeviceLayer::ConnectivityMgr().SetBLEDeviceName(nullptr));
    ReturnErrorOnFailure(DeviceLayer::Internal::BLEMgrImpl().ConfigureBle(0, false));
    ReturnErrorOnFailure(DeviceLayer::ConnectivityMgr().SetBLEAdvertisingEnabled(true));

    return CHIP_NO_ERROR;
}

} // namespace

void ApplicationShutdown() {}

int main(int argc, char * argv[])
{
    ChipLogProgress(AppServer, "Initializing");

    if (CHIP_ERROR err = Initialize(); err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Initialize() failed: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    ChipLogProgress(AppServer, "Hello from all-devices-app!");
    StartApplication();

    return 0;
}
