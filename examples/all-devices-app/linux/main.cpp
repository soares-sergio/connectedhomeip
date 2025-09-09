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

#include <AppDataModel.h>
#include <TracingCommandLineArgument.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/clusters/administrator-commissioning-server/AdministratorCommissioningCluster.h>
#include <app/clusters/basic-information/BasicInformationCluster.h>
#include <app/clusters/general-commissioning-server/general-commissioning-cluster.h>
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

// Code driven clusters made for this app, these are clusters that
// need some additional work before landing upstream
#include <cluster-impl/access-control-cluster.h>
#include <cluster-impl/descriptor-cluster.h>
#include <cluster-impl/identify-cluster.h>
#include <cluster-impl/operational-credentials-cluster.h>

#if defined(CHIP_IMGUI_ENABLED) && CHIP_IMGUI_ENABLED
#include <imgui_ui/ui.h>
#include <imgui_ui/windows/connectivity.h>
#include <imgui_ui/windows/qrcode.h>
#endif

#include <AppMain.h>
#include <map>
#include <string>

using namespace chip;
using namespace chip::app;
using namespace chip::Platform;
using namespace chip::DeviceLayer;
using namespace chip::app::Clusters;
using namespace chip::ArgParser;

namespace {
AppMainLoopImplementation * gMainLoopImplementation = nullptr;

DataModel::DeviceTypeEntry deviceTypesEp0[] = {
    { 0x0016, 3 }, // ma_rootdevice, version 3
};

SpanEndpoint endpoint0 = SpanEndpoint::Builder().SetDeviceTypes(Span<const DataModel::DeviceTypeEntry>(deviceTypesEp0)).Build();

EndpointInterfaceRegistration endpointRegistration0(endpoint0,
                                                    { .id                 = 0,
                                                      .parentId           = kInvalidEndpointId,
                                                      .compositionPattern = DataModel::EndpointCompositionPattern::kFullFamily });

DeviceLayer::NetworkCommissioning::LinuxWiFiDriver sWiFiDriver;
RegisteredServerCluster<NetworkCommissioningCluster> sWifiNetworkCommissioningCluster(kRootEndpointId, &sWiFiDriver);

DeviceInfoProviderImpl gExampleDeviceInfoProvider;

// To hold SPAKE2+ verifier, discriminator, passcode
LinuxCommissionableDataProvider gCommissionableDataProvider;

std::unique_ptr<DeviceManager> gDeviceManager;

// App custom argument handling
constexpr uint16_t kOptionDeviceType = 0xffd0;

constexpr const char * kBridgeApp               = "bridge";
constexpr const char * kContactSensorApp        = "contact-sensor";
constexpr const char * kOccupancySensorApp      = "occupancy-sensor";
constexpr const char * kLightApp                = "light";
constexpr const char * kPlugApp                 = "plug";
std::map<std::string, DeviceType> kValidApps = { { kBridgeApp, DeviceType::kAggregator },
                                                    { kContactSensorApp, DeviceType::kContactSensor },
                                                    { kOccupancySensorApp, DeviceType::kOccupancySensor },
                                                    { kLightApp, DeviceType::kOnOffLight },
                                                    { kPlugApp, DeviceType::kOnOffPlug } };

DeviceType deviceType   = DeviceType::kAggregator; // Using a bridge as default
const char * deviceName = kBridgeApp;

chip::ArgParser::OptionDef sAllDevicesAppOptionDefs[] = {
    { "device", chip::ArgParser::kArgumentRequired, kOptionDeviceType },
};

bool AllDevicesAppOptionHandler(const char * program, OptionSet * options, int identifier, const char * name, const char * value)
{
    switch (identifier)
    {
    case kOptionDeviceType:
        if (value == nullptr || kValidApps.find(value) == kValidApps.end())
        {
            ChipLogError(Support, "INTERNAL ERROR: Invalid device type: %s\n", value);
            return false;
        }
        ChipLogProgress(AppServer, "Using the device type of %s", value);
        deviceType = kValidApps[value];
        deviceName = value;
        return true;
    default:
        ChipLogError(Support, "%s: INTERNAL ERROR: Unhandled option: %s\n", program, name);
        return false;
    }

    return true;
}

chip::ArgParser::OptionSet sCmdLineOptions = { AllDevicesAppOptionHandler, // handler function
                                               sAllDevicesAppOptionDefs,   // array of option definitions
                                               "PROGRAM OPTIONS",          // help group
                                               "-d, --device <bridge|contact-sensor|occupancy-sensor|light>\n" };

void StopSignalHandler(int /* signal */)
{
    if (gMainLoopImplementation != nullptr)
    {
        gMainLoopImplementation->SignalSafeStopMainLoop();
    }
    else
    {
        Server::GetInstance().GenerateShutDownEvent();
        SystemLayer().ScheduleLambda([]() { PlatformMgr().StopEventLoopTask(); });
    }
}

[[maybe_unused]] chip::app::DataModel::Provider * PopulateCodeDrivenDataModelProvider(PersistentStorageDelegate * delegate)
{
    static chip::app::DefaultAttributePersistenceProvider attributePersistenceProvider;
    static chip::app::CodeDrivenDataModelProvider dataModelProvider =
        chip::app::CodeDrivenDataModelProvider(*delegate, attributePersistenceProvider);

    gDeviceManager = std::make_unique<DeviceManager>(1 /* start endpoint id */, dataModelProvider);

    // register real code driven clusters
    static GeneralCommissioningCluster clusterGeneralCommissioning({}, {});
    static ServerClusterRegistration serverClusterGeneralCommissioning(clusterGeneralCommissioning);
    VerifyOrDie(dataModelProvider.AddCluster(serverClusterGeneralCommissioning) == CHIP_NO_ERROR);

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

    static DescriptorCluster clusterDescriptor(0, BitFlags<Descriptor::Feature>(0));
    static ServerClusterRegistration descriptor(clusterDescriptor);
    VerifyOrDie(dataModelProvider.AddCluster(descriptor) == CHIP_NO_ERROR);

    static AccessControlCluster clusterAccessControl({});
    static ServerClusterRegistration accessControl(clusterAccessControl);
    VerifyOrDie(dataModelProvider.AddCluster(accessControl) == CHIP_NO_ERROR);

    static OperationalCredentialsCluster clusterOperationalCredenitals(0);
    static ServerClusterRegistration operationalCredentials(clusterOperationalCredenitals);
    VerifyOrDie(dataModelProvider.AddCluster(operationalCredentials) == CHIP_NO_ERROR);

    VerifyOrDie(dataModelProvider.AddCluster(sWifiNetworkCommissioningCluster.Registration()) == CHIP_NO_ERROR);

    // Add Endpoint 0 Registration
    // TODO: Create a Root Node Device type and move this inside RegisterNewDevice
    CHIP_ERROR err = dataModelProvider.AddEndpoint(endpointRegistration0);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Cannot register Endpoint 0: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    VerifyOrDie(RegisterNewDevice(deviceType, deviceName, kInvalidEndpointId, *gDeviceManager) == CHIP_NO_ERROR);

    rpc::Start(33000, *gDeviceManager);

    return &dataModelProvider;
}

void RunApplication(AppMainLoopImplementation * mainLoop = nullptr)
{
    gMainLoopImplementation = mainLoop;

    static chip::CommonCaseDeviceServerInitParams initParams;
    VerifyOrDie(initParams.InitializeStaticResourcesBeforeServerInit() == CHIP_NO_ERROR);

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

#if defined(CHIP_IMGUI_ENABLED) && CHIP_IMGUI_ENABLED
    // This is a hack .. but keeping it for now to get imgui qrcode working
    LinuxDeviceOptions::GetInstance().payload = payload;
#endif

    SetDeviceAttestationCredentialsProvider(Credentials::Examples::GetExampleDACProvider());

    sWiFiDriver.Set5gSupport(true);

    struct sigaction sa = {};
    sa.sa_handler       = StopSignalHandler;
    sa.sa_flags         = SA_RESETHAND;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);

    if (mainLoop != nullptr)
    {
        mainLoop->RunMainLoop();
    }
    else
    {
        DeviceLayer::PlatformMgr().RunEventLoop();
    }
    gMainLoopImplementation = nullptr;

    rpc::Stop();
    gDeviceManager.reset();
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
    auto discriminator                              = static_cast<uint16_t>(CHIP_DEVICE_CONFIG_USE_TEST_SETUP_DISCRIMINATOR);
    chip::Optional<uint16_t> discriminatorFromParam = LinuxDeviceOptions::GetInstance().discriminator;
    if (discriminatorFromParam.HasValue())
    {
        discriminator = discriminatorFromParam.Value();
    }

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

CHIP_ERROR Initialize(int argc, char * argv[])
{
    ChipLogProgress(AppServer, "Initializing...");
    ReturnErrorOnFailure(Platform::MemoryInit());
    ReturnErrorOnFailure(ParseArguments(argc, argv, &sCmdLineOptions));
    ReturnErrorOnFailure(DeviceLayer::PersistedStorage::KeyValueStoreMgrImpl().Init(CHIP_CONFIG_KVS_PATH));
    ReturnErrorOnFailure(DeviceLayer::PlatformMgr().InitChipStack());

    ReturnErrorOnFailure(InitCommissionableDataProvider(gCommissionableDataProvider));
    DeviceLayer::SetCommissionableDataProvider(&gCommissionableDataProvider);
    DeviceLayer::SetDeviceInfoProvider(&gExampleDeviceInfoProvider);
    ConfigurationMgr().LogDeviceConfig();

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

    if (CHIP_ERROR err = Initialize(argc, argv); err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Initialize() failed: %" CHIP_ERROR_FORMAT, err.Format());
        chipDie();
    }

    ChipLogProgress(AppServer, "Hello from all-devices-app!");
#if defined(CHIP_IMGUI_ENABLED) && CHIP_IMGUI_ENABLED
    example::Ui::ImguiUi ui;
    ui.AddWindow(std::make_unique<example::Ui::Windows::QRCode>());
    ui.AddWindow(std::make_unique<example::Ui::Windows::Connectivity>());
    RunApplication(&ui);
#else
    RunApplication();
#endif

    return 0;
}
