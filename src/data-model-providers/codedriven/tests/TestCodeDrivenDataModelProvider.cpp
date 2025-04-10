#include <pw_unit_test/framework.h>

#include <app/data-model-provider/MetadataTypes.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/ServerClusterContext.h>
#include <app/server-cluster/ServerClusterInterface.h>
#include <app/server-cluster/testing/TestServerClusterContext.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/DynamicEndpointProvider.h>
#include <lib/support/ReadOnlyBuffer.h>

#include <memory> // For std::unique_ptr
#include <vector> // For std::vector

using chip::BitFlags;
using chip::CharSpan;
using chip::ClusterId;
using chip::CommandId;
using chip::DataVersion;
using chip::EndpointId;
using chip::EventNumber;
using chip::ReadOnlyBufferBuilder;
using chip::app::AttributeValueDecoder;
using chip::app::AttributeValueEncoder;
using chip::app::CodeDrivenDataModelProvider;
using chip::app::CommandHandler;
using chip::app::ConcreteClusterPath;
using chip::app::DefaultServerCluster;
using chip::app::DynamicEndpointProvider;
using chip::app::ServerClusterContext;
using chip::app::ServerClusterInterface;
using chip::app::DataModel::ActionContext;
using chip::app::DataModel::EventsGenerator;
using chip::app::DataModel::InteractionModelContext;
using chip::app::DataModel::Nullable;
using chip::app::DataModel::ProviderChangeListener;
using chip::Messaging::ExchangeContext;
using SemanticTag = chip::app::Clusters::Descriptor::Structs::SemanticTagStruct::Type;
using chip::app::DataModel::AcceptedCommandEntry;
using chip::app::DataModel::ActionReturnStatus;
using chip::app::DataModel::ClusterQualityFlags;
using chip::app::DataModel::DeviceTypeEntry;
using chip::app::DataModel::EndpointCompositionPattern;
using chip::app::DataModel::EndpointEntry;
using chip::app::DataModel::InvokeRequest;
using chip::app::DataModel::ReadAttributeRequest;
using chip::app::DataModel::ServerClusterEntry;
using chip::app::DataModel::WriteAttributeRequest;
using chip::TLV::TLVReader;

using namespace chip::app;

class TestProviderChangeListener : public ProviderChangeListener
{
public:
    void MarkDirty(const AttributePathParams & path) override { mDirtyList.push_back(path); }
    std::vector<AttributePathParams> mDirtyList;
};

class TestEventGenerator : public EventsGenerator
{
    CHIP_ERROR GenerateEvent(EventLoggingDelegate * eventPayloadWriter, const EventOptions & options,
                             EventNumber & generatedEventNumber) override
    {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }
};

class TestActionContext : public ActionContext
{
public:
    ExchangeContext * CurrentExchange() override { return nullptr; }
};

class TestCodeDrivenDataModelProvider : public ::testing::Test
{
public:
    static void SetUpTestSuite() { ASSERT_EQ(chip::Platform::MemoryInit(), CHIP_NO_ERROR); }
    static void TearDownTestSuite() { chip::Platform::MemoryShutdown(); }

protected:
    TestProviderChangeListener mChangeListener;
    TestEventGenerator mEventGenerator;
    TestActionContext mActionContext;
    InteractionModelContext mContext{
        .eventsGenerator         = &mEventGenerator,
        .dataModelChangeListener = &mChangeListener,
        .actionContext           = &mActionContext,
    };
    CodeDrivenDataModelProvider mProvider;
    std::vector<std::unique_ptr<DynamicEndpointProvider>> mEndpointStorage; // To keep providers alive
    chip::Test::TestServerClusterContext mServerClusterTestContext;

    TestCodeDrivenDataModelProvider()
    {
        EXPECT_EQ(mProvider.Startup(mContext), CHIP_NO_ERROR);
        mProvider.SetPersistentStorageDelegate(&mServerClusterTestContext.StorageDelegate());
    }

    ~TestCodeDrivenDataModelProvider() override
    {
        mProvider.Shutdown();
        mEndpointStorage.clear();
    }
};

// MockServerCluster implementation
class MockServerCluster : public DefaultServerCluster
{
public:
    MockServerCluster(ConcreteClusterPath path, DataVersion dataVersion, BitFlags<ClusterQualityFlags> flags) :
        DefaultServerCluster(path), mPath(path), mDataVersion(dataVersion), mFlags(flags)
    {}
    ~MockServerCluster() override = default;

    chip::Span<const ConcreteClusterPath> GetPaths() const override { return chip::Span<const ConcreteClusterPath>({ &mPath, 1 }); }
    DataVersion GetDataVersion(const ConcreteClusterPath &) const override { return mDataVersion; }
    BitFlags<ClusterQualityFlags> GetClusterFlags(const ConcreteClusterPath &) const override { return mFlags; }

    // Implement the pure virtual functions from DefaultServerCluster
    ActionReturnStatus ReadAttribute(const ReadAttributeRequest & request, AttributeValueEncoder & encoder) override
    {
        return ActionReturnStatus(CHIP_ERROR_NOT_IMPLEMENTED);
    }
    ActionReturnStatus WriteAttribute(const WriteAttributeRequest & request, AttributeValueDecoder & decoder) override
    {
        return ActionReturnStatus(CHIP_ERROR_NOT_IMPLEMENTED);
    }
    std::optional<ActionReturnStatus> InvokeCommand(const InvokeRequest & request, TLVReader & input_arguments,
                                                    CommandHandler * handler) override
    {
        return std::nullopt;
    }
    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<AcceptedCommandEntry> & builder) override
    {
        return CHIP_NO_ERROR;
    }
    CHIP_ERROR GeneratedCommands(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<CommandId> & builder) override
    {
        return CHIP_NO_ERROR;
    }

private:
    ConcreteClusterPath mPath;
    DataVersion mDataVersion;
    BitFlags<ClusterQualityFlags> mFlags;
};

namespace {

constexpr EndpointEntry endpointEntry1 = { .id                 = 1,
                                           .parentId           = chip::kInvalidEndpointId,
                                           .compositionPattern = EndpointCompositionPattern::kFullFamily };

constexpr EndpointEntry endpointEntry2 = { .id                 = 2,
                                           .parentId           = chip::kInvalidEndpointId,
                                           .compositionPattern = EndpointCompositionPattern::kTree };

constexpr EndpointEntry endpointEntry3 = { .id                 = 3,
                                           .parentId           = chip::kInvalidEndpointId,
                                           .compositionPattern = EndpointCompositionPattern::kFullFamily };

DeviceTypeEntry deviceType1 = { .deviceTypeId = 1, .deviceTypeRevision = 1 };
DeviceTypeEntry deviceType2 = { .deviceTypeId = 2, .deviceTypeRevision = 2 };
DeviceTypeEntry deviceType3 = { .deviceTypeId = 3, .deviceTypeRevision = 3 };

SemanticTag semanticTag1 = { .mfgCode     = chip::VendorId::Google,
                             .namespaceID = 1,
                             .tag         = 1,
                             .label       = chip::Optional<chip::app::DataModel::Nullable<chip::CharSpan>>(
                                 { chip::app::DataModel::MakeNullable(chip::CharSpan("label1", 6)) }) };
SemanticTag semanticTag2 = { .mfgCode     = chip::VendorId::Google,
                             .namespaceID = 2,
                             .tag         = 2,
                             .label       = chip::Optional<chip::app::DataModel::Nullable<chip::CharSpan>>(
                                 { chip::app::DataModel::MakeNullable(chip::CharSpan("label1", 6)) }) };

MockServerCluster mockServerCluster1(ConcreteClusterPath(1, 10), 1, BitFlags<ClusterQualityFlags>());
MockServerCluster mockServerCluster2(ConcreteClusterPath(1, 20), 2,
                                     BitFlags<ClusterQualityFlags>().Set(ClusterQualityFlags::kDiagnosticsData));
MockServerCluster mockServerCluster3(ConcreteClusterPath(2, 30), 3, BitFlags<ClusterQualityFlags>());

constexpr EndpointId clientClusterId1 = 1;
constexpr EndpointId clientClusterId2 = 2;
constexpr EndpointId clientClusterId3 = 3;

} // namespace

TEST_F(TestCodeDrivenDataModelProvider, IterateOverEndpoints)
{
    auto ep1Provider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    auto ep2Provider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry2.id, endpointEntry2.compositionPattern, endpointEntry2.parentId);
    auto ep3Provider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry3.id, endpointEntry3.compositionPattern, endpointEntry3.parentId);

    ASSERT_EQ(mProvider.AddEndpoint(*ep1Provider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(ep1Provider));
    ASSERT_EQ(mProvider.AddEndpoint(*ep2Provider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(ep2Provider));
    ASSERT_EQ(mProvider.AddEndpoint(*ep3Provider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(ep3Provider));

    ReadOnlyBufferBuilder<DataModel::EndpointEntry> endpointsBuilder;

    ASSERT_EQ(mProvider.Endpoints(endpointsBuilder), CHIP_NO_ERROR);

    auto endpoints = endpointsBuilder.TakeBuffer();

    ASSERT_EQ(endpoints.size(), 3u);

    EXPECT_EQ(endpoints[0], endpointEntry1);
    EXPECT_EQ(endpoints[1], endpointEntry2);
    EXPECT_EQ(endpoints[2], endpointEntry3);
}

TEST_F(TestCodeDrivenDataModelProvider, IterateOverServerClusters)
{
    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    ASSERT_EQ(epProvider->AddServerCluster(mockServerCluster1), CHIP_NO_ERROR);
    ASSERT_EQ(epProvider->AddServerCluster(mockServerCluster2), CHIP_NO_ERROR);

    ASSERT_EQ(mProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(epProvider));

    ReadOnlyBufferBuilder<ServerClusterEntry> builder;
    ASSERT_EQ(mProvider.ServerClusters(endpointEntry1.id, builder), CHIP_NO_ERROR);

    auto serverClusters = builder.TakeBuffer();
    ASSERT_EQ(serverClusters.size(), 2u);

    EXPECT_EQ(serverClusters[0].clusterId, mockServerCluster1.GetPaths()[0].mClusterId);
    EXPECT_EQ(serverClusters[0].dataVersion, mockServerCluster1.GetDataVersion({}));
    EXPECT_EQ(serverClusters[0].flags, mockServerCluster1.GetClusterFlags({}));

    EXPECT_EQ(serverClusters[1].clusterId, mockServerCluster2.GetPaths()[0].mClusterId);
    EXPECT_EQ(serverClusters[1].dataVersion, mockServerCluster2.GetDataVersion({}));
    EXPECT_EQ(serverClusters[1].flags, mockServerCluster2.GetClusterFlags({}));
}

TEST_F(TestCodeDrivenDataModelProvider, IterateOverClientClusters)
{
    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    ASSERT_EQ(epProvider->AddClientCluster(clientClusterId1), CHIP_NO_ERROR);
    ASSERT_EQ(epProvider->AddClientCluster(clientClusterId2), CHIP_NO_ERROR);

    ASSERT_EQ(mProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(epProvider));

    ReadOnlyBufferBuilder<ClusterId> builder;
    ASSERT_EQ(mProvider.ClientClusters(endpointEntry1.id, builder), CHIP_NO_ERROR);

    auto clientClusters = builder.TakeBuffer();
    ASSERT_EQ(clientClusters.size(), 2u);
    EXPECT_EQ(clientClusters[0], clientClusterId1);
    EXPECT_EQ(clientClusters[1], clientClusterId2);
}

TEST_F(TestCodeDrivenDataModelProvider, IterateOverTags)
{
    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    ASSERT_EQ(epProvider->AddSemanticTag(semanticTag1), CHIP_NO_ERROR);
    ASSERT_EQ(epProvider->AddSemanticTag(semanticTag2), CHIP_NO_ERROR);

    ASSERT_EQ(mProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(epProvider));

    ReadOnlyBufferBuilder<SemanticTag> builder;
    ASSERT_EQ(mProvider.SemanticTags(endpointEntry1.id, builder), CHIP_NO_ERROR);

    auto tags = builder.TakeBuffer();
    ASSERT_EQ(tags.size(), 2u);
    // SemanticTag doesn't have operator==, compare members
    EXPECT_EQ(tags[0].mfgCode, semanticTag1.mfgCode);
    EXPECT_EQ(tags[0].namespaceID, semanticTag1.namespaceID);
    EXPECT_EQ(tags[0].tag, semanticTag1.tag);
    EXPECT_EQ(tags[1].mfgCode, semanticTag2.mfgCode);
    EXPECT_EQ(tags[1].namespaceID, semanticTag2.namespaceID);
    EXPECT_EQ(tags[1].tag, semanticTag2.tag);
}

TEST_F(TestCodeDrivenDataModelProvider, IterateOverDeviceTypes)
{
    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    DeviceTypeEntry deviceType;
    for (unsigned int i = 0; i < DynamicEndpointProviderConfig::kMaxDeviceTypes; i++)
    {
        deviceType.deviceTypeId = i;
        ASSERT_EQ(epProvider->AddDeviceType(deviceType), CHIP_NO_ERROR);
    }
    deviceType.deviceTypeId++;

    ASSERT_EQ(mProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(epProvider));

    ReadOnlyBufferBuilder<DeviceTypeEntry> builder;
    ASSERT_EQ(mProvider.DeviceTypes(endpointEntry1.id, builder), CHIP_NO_ERROR);

    auto deviceTypes = builder.TakeBuffer();
    ASSERT_EQ(deviceTypes.size(), DynamicEndpointProviderConfig::kMaxDeviceTypes);
    for (unsigned int i = 0; i < DynamicEndpointProviderConfig::kMaxDeviceTypes; i++)
    {
        EXPECT_EQ(deviceTypes[i].deviceTypeId, i);
    }
    // Add one more over capacity to the DynamicEndpointProvider directly
    ASSERT_EQ(mEndpointStorage.front()->AddDeviceType(deviceType), CHIP_ERROR_NO_MEMORY);
}

TEST_F(TestCodeDrivenDataModelProvider, AddSemanticTagsOverCapacity)
{
    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    SemanticTag semanticTag;
    for (uint8_t i = 0; i < DynamicEndpointProviderConfig::kMaxSemanticTags; i++)
    {
        semanticTag.tag = i;
        ASSERT_EQ(epProvider->AddSemanticTag(semanticTag), CHIP_NO_ERROR);
    }
    semanticTag.tag++;
    ASSERT_EQ(epProvider->AddSemanticTag(semanticTag), CHIP_ERROR_NO_MEMORY);
}

TEST_F(TestCodeDrivenDataModelProvider, AddClientClustersOverCapacity)
{
    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    ClusterId clusterId;
    for (unsigned int i = 0; i < DynamicEndpointProviderConfig::kMaxClientClusters; i++)
    {
        clusterId = i;
        ASSERT_EQ(epProvider->AddClientCluster(clusterId), CHIP_NO_ERROR);
    }
    clusterId++;
    ASSERT_EQ(epProvider->AddClientCluster(clusterId), CHIP_ERROR_NO_MEMORY);
}

TEST_F(TestCodeDrivenDataModelProvider, AddServerClustersOverCapacity)
{
    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    MockServerCluster mockServerCluster(ConcreteClusterPath(1, 10), 1, BitFlags<ClusterQualityFlags>());
    for (unsigned int i = 0; i < DynamicEndpointProviderConfig::kMaxServerClusters; i++)
    {
        // Startup is handled by CodeDrivenDataModelProvider when endpoint is added or provider starts
        ASSERT_EQ(epProvider->AddServerCluster(mockServerCluster), CHIP_NO_ERROR);
    }
    ASSERT_EQ(epProvider->AddServerCluster(mockServerCluster), CHIP_ERROR_NO_MEMORY);
}

TEST_F(TestCodeDrivenDataModelProvider, AddAndRemoveEndpoints)
{
    // Add 3 endpoints
    auto ep1Provider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    auto ep2Provider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry2.id, endpointEntry2.compositionPattern, endpointEntry2.parentId);
    auto ep3Provider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry3.id, endpointEntry3.compositionPattern, endpointEntry3.parentId);

    EXPECT_EQ(mProvider.AddEndpoint(*ep1Provider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(ep1Provider));
    EXPECT_EQ(mProvider.AddEndpoint(*ep2Provider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(ep2Provider)); // Will be kept
    EXPECT_EQ(mProvider.AddEndpoint(*ep3Provider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(ep3Provider));

    // Remove endpoints 1 and 3
    EXPECT_EQ(mProvider.RemoveEndpoint(endpointEntry1.id), CHIP_NO_ERROR);
    EXPECT_EQ(mProvider.RemoveEndpoint(endpointEntry3.id), CHIP_NO_ERROR);

    // Remove from storage too to reflect they are gone
    mEndpointStorage.erase(std::remove_if(mEndpointStorage.begin(), mEndpointStorage.end(),
                                          [&](const auto & ep_ptr) {
                                              return ep_ptr->GetEndpointEntry().id == endpointEntry1.id ||
                                                  ep_ptr->GetEndpointEntry().id == endpointEntry3.id;
                                          }),
                           mEndpointStorage.end());

    // Assert that the remaining endpoints are as expected
    ReadOnlyBufferBuilder<EndpointEntry> out;
    EXPECT_EQ(mProvider.Endpoints(out), CHIP_NO_ERROR);
    EXPECT_EQ(out.Size(), 1U);

    auto endpoints = out.TakeBuffer();
    EXPECT_EQ(endpoints[0].id, endpointEntry2.id);
}

TEST_F(TestCodeDrivenDataModelProvider, AddAndRemoveSemanticTags)
{
    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    // Add semantic tags
    EXPECT_EQ(epProvider->AddSemanticTag(semanticTag1), CHIP_NO_ERROR);
    EXPECT_EQ(epProvider->AddSemanticTag(semanticTag2), CHIP_NO_ERROR);

    EXPECT_EQ(mProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(epProvider)); // epProvider is now moved, access via mEndpointStorage.back()

    // Remove semantic tag 1
    EXPECT_EQ(mEndpointStorage.back()->RemoveSemanticTag(semanticTag1), CHIP_NO_ERROR);

    // Assert that the remaining semantic tags are as expected
    ReadOnlyBufferBuilder<SemanticTag> out;
    EXPECT_EQ(mProvider.SemanticTags(endpointEntry1.id, out), CHIP_NO_ERROR);
    EXPECT_EQ(out.Size(), 1U);

    auto semanticTags = out.TakeBuffer();
    EXPECT_EQ(semanticTags[0].namespaceID, semanticTag2.namespaceID);
    EXPECT_EQ(semanticTags[0].tag, semanticTag2.tag);
}
TEST_F(TestCodeDrivenDataModelProvider, AddAndRemoveDeviceTypes)
{
    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    // Add device types
    EXPECT_EQ(epProvider->AddDeviceType(deviceType1), CHIP_NO_ERROR);
    EXPECT_EQ(epProvider->AddDeviceType(deviceType2), CHIP_NO_ERROR);
    EXPECT_EQ(epProvider->AddDeviceType(deviceType3), CHIP_NO_ERROR);

    EXPECT_EQ(mProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(epProvider));

    // Remove device type 1
    EXPECT_EQ(mEndpointStorage.back()->RemoveDeviceType(deviceType1.deviceTypeId), CHIP_NO_ERROR);

    // Assert that the remaining device types are as expected
    ReadOnlyBufferBuilder<DeviceTypeEntry> out;
    EXPECT_EQ(mProvider.DeviceTypes(endpointEntry1.id, out), CHIP_NO_ERROR);
    EXPECT_EQ(out.Size(), 2U);

    auto deviceTypes = out.TakeBuffer();
    EXPECT_EQ(deviceTypes[0].deviceTypeId, deviceType2.deviceTypeId);
    EXPECT_EQ(deviceTypes[0].deviceTypeRevision, deviceType2.deviceTypeRevision);
    EXPECT_EQ(deviceTypes[1].deviceTypeId, deviceType3.deviceTypeId);
    EXPECT_EQ(deviceTypes[1].deviceTypeRevision, deviceType3.deviceTypeRevision);
}

TEST_F(TestCodeDrivenDataModelProvider, AddAndRemoveClientClusters)
{
    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    // Add client clusters
    EXPECT_EQ(epProvider->AddClientCluster(clientClusterId1), CHIP_NO_ERROR);
    EXPECT_EQ(epProvider->AddClientCluster(clientClusterId2), CHIP_NO_ERROR);
    EXPECT_EQ(epProvider->AddClientCluster(clientClusterId3), CHIP_NO_ERROR);

    EXPECT_EQ(mProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(epProvider));

    // Remove client cluster 1
    EXPECT_EQ(mEndpointStorage.back()->RemoveClientCluster(clientClusterId1), CHIP_NO_ERROR);

    // Assert that the remaining client clusters are as expected
    ReadOnlyBufferBuilder<ClusterId> out;
    EXPECT_EQ(mProvider.ClientClusters(endpointEntry1.id, out), CHIP_NO_ERROR);
    EXPECT_EQ(out.Size(), 2U);

    auto clientClusters = out.TakeBuffer();
    EXPECT_EQ(clientClusters[0], clientClusterId2);
    EXPECT_EQ(clientClusters[1], clientClusterId3);
}

TEST_F(TestCodeDrivenDataModelProvider, AddAndRemoveServerClusters)
{
    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    // Add server clusters
    EXPECT_EQ(epProvider->AddServerCluster(mockServerCluster1), CHIP_NO_ERROR);
    EXPECT_EQ(epProvider->AddServerCluster(mockServerCluster2), CHIP_NO_ERROR);
    EXPECT_EQ(epProvider->AddServerCluster(mockServerCluster3), CHIP_NO_ERROR);

    EXPECT_EQ(mProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(epProvider));

    // Remove server cluster 1
    EXPECT_EQ(mEndpointStorage.back()->RemoveServerCluster(mockServerCluster1.GetPaths()[0].mClusterId), CHIP_NO_ERROR);

    // Assert that the remaining server clusters are as expected
    ReadOnlyBufferBuilder<ServerClusterEntry> out;
    EXPECT_EQ(mProvider.ServerClusters(endpointEntry1.id, out), CHIP_NO_ERROR);
    EXPECT_EQ(out.Size(), 2U);

    auto serverClusters = out.TakeBuffer();
    EXPECT_EQ(serverClusters[0].clusterId, mockServerCluster2.GetPaths()[0].mClusterId);
    EXPECT_EQ(serverClusters[1].clusterId, mockServerCluster3.GetPaths()[0].mClusterId);
}
