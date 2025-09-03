#include <AppDataModel.h>
#include <app/data-model-provider/MetadataTypes.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/testing/TestEventGenerator.h>
#include <app/server-cluster/testing/TestProviderChangeListener.h>
#include <app/server-cluster/testing/TestServerClusterContext.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/endpoint/SpanEndpoint.h>
#include <lib/support/ReadOnlyBuffer.h>
#include <pw_unit_test/framework.h>

using chip::BitFlags;
using chip::CharSpan;
using chip::EndpointId;
using chip::EventNumber;
using chip::ReadOnlyBufferBuilder;
using chip::app::CodeDrivenDataModelProvider;
using chip::app::ConcreteClusterPath;
using chip::app::ServerClusterContext;
using chip::app::ServerClusterInterface;
using chip::app::DataModel::ActionContext;
using chip::app::DataModel::EndpointCompositionPattern;
using chip::app::DataModel::EndpointEntry;
using chip::app::DataModel::EventsGenerator;
using chip::app::DataModel::InteractionModelContext;
using chip::app::DataModel::ProviderChangeListener;
using chip::Messaging::ExchangeContext;

using namespace chip::app;
using namespace chip::Test;

class TestActionContext : public ActionContext
{
public:
    ExchangeContext * CurrentExchange() override { return nullptr; }
};

#include <app/persistence/DefaultAttributePersistenceProvider.h>
#include <platform/KvsPersistentStorageDelegate.h>

class TestAllDevicesApp : public ::testing::Test
{
public:
    static void SetUpTestSuite() { ASSERT_EQ(chip::Platform::MemoryInit(), CHIP_NO_ERROR); }
    static void TearDownTestSuite() { chip::Platform::MemoryShutdown(); }
};

#include <app-common/zap-generated/ids/Clusters.h>
#include <algorithm>
#include <vector>

namespace {

constexpr EndpointId kTestEndpointId = 1;
constexpr chip::DeviceTypeId kContactSensorDeviceType = 0x0015;
constexpr uint16_t kContactSensorDeviceTypeRevision = 2;

constexpr chip::DeviceTypeId kOccupancySensorDeviceType = 0x0107;
constexpr uint16_t kOccupancySensorDeviceTypeRevision = 4;

void VerifyServerClusters(CodeDrivenDataModelProvider & provider, EndpointId endpointId, std::vector<chip::ClusterId> expectedClusterIds)
{
    ReadOnlyBufferBuilder<DataModel::ServerClusterEntry> clustersBuilder;
    ASSERT_EQ(provider.ServerClusters(endpointId, clustersBuilder), CHIP_NO_ERROR);
    auto clusters = clustersBuilder.TakeBuffer();

    std::vector<chip::ClusterId> actualClusterIds;
    for (const auto & cluster : clusters)
    {
        actualClusterIds.push_back(cluster.clusterId);
    }
    std::sort(actualClusterIds.begin(), actualClusterIds.end());
    std::sort(expectedClusterIds.begin(), expectedClusterIds.end());

    ASSERT_EQ(actualClusterIds.size(), expectedClusterIds.size());
    for (size_t i = 0; i < actualClusterIds.size(); ++i)
    {
        ASSERT_EQ(actualClusterIds[i], expectedClusterIds[i]);
    }
}
} // namespace

TEST_F(TestAllDevicesApp, RegisterNewDevice_ContactSensor)
{
    chip::KvsPersistentStorageDelegate storage;
    chip::app::DefaultAttributePersistenceProvider attributeProvider;
    CodeDrivenDataModelProvider provider(storage, attributeProvider);
    std::unique_ptr<Device> device = RegisterNewDevice(AppDeviceType::kContactSensor, provider, kTestEndpointId);
    ASSERT_TRUE(device);

    // Check device types
    ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> deviceTypesBuilder;
    ASSERT_EQ(provider.DeviceTypes(kTestEndpointId, deviceTypesBuilder), CHIP_NO_ERROR);
    auto deviceTypes = deviceTypesBuilder.TakeBuffer();
    ASSERT_EQ(deviceTypes.size(), 2u);
    ASSERT_EQ(deviceTypes[1].deviceTypeId, kContactSensorDeviceType);
    ASSERT_EQ(deviceTypes[1].deviceTypeRevision, kContactSensorDeviceTypeRevision);

    // Check server clusters
    VerifyServerClusters(provider, kTestEndpointId, {
        chip::app::Clusters::Descriptor::Id,
        chip::app::Clusters::Identify::Id,
        chip::app::Clusters::BooleanState::Id
    });
}

TEST_F(TestAllDevicesApp, RegisterNewDevice_OccupancySensor)
{
    chip::KvsPersistentStorageDelegate storage;
    chip::app::DefaultAttributePersistenceProvider attributeProvider;
    CodeDrivenDataModelProvider provider(storage, attributeProvider);
    std::unique_ptr<Device> device = RegisterNewDevice(AppDeviceType::kOccupancySensor, provider, kTestEndpointId);
    ASSERT_TRUE(device);

    // Check device types
    ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> deviceTypesBuilder;
    ASSERT_EQ(provider.DeviceTypes(kTestEndpointId, deviceTypesBuilder), CHIP_NO_ERROR);
    auto deviceTypes = deviceTypesBuilder.TakeBuffer();
    ASSERT_EQ(deviceTypes.size(), 2u);
    ASSERT_EQ(deviceTypes[1].deviceTypeId, kOccupancySensorDeviceType);
    ASSERT_EQ(deviceTypes[1].deviceTypeRevision, kOccupancySensorDeviceTypeRevision);

    // Check server clusters
    VerifyServerClusters(provider, kTestEndpointId, {
        chip::app::Clusters::Descriptor::Id,
        chip::app::Clusters::Identify::Id,
        chip::app::Clusters::OccupancySensing::Id
    });
}
