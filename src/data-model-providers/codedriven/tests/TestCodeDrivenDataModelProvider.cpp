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
#include <pw_unit_test/framework.h>

#include <app/data-model-provider/MetadataTypes.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/ServerClusterContext.h>
#include <app/server-cluster/ServerClusterInterface.h>
#include <app/server-cluster/testing/TestServerClusterContext.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/DynamicEndpointProvider.h>
#include <lib/support/ReadOnlyBuffer.h>

using namespace chip;
using namespace chip::app;

class TestProviderChangeListener : public DataModel::ProviderChangeListener
{
public:
    void MarkDirty(const AttributePathParams & path) override { mDirtyList.push_back(path); }
    std::vector<AttributePathParams> mDirtyList;
};

class TestEventGenerator : public DataModel::EventsGenerator
{
    CHIP_ERROR GenerateEvent(EventLoggingDelegate * eventPayloadWriter, const EventOptions & options,
                             EventNumber & generatedEventNumber) override
    {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }
};

class TestActionContext : public DataModel::ActionContext
{
public:
    Messaging::ExchangeContext * CurrentExchange() override { return nullptr; }
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
    DataModel::InteractionModelContext mContext{
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
    MockServerCluster(ConcreteClusterPath path, DataVersion dataVersion, BitFlags<DataModel::ClusterQualityFlags> flags) :
        DefaultServerCluster(path), mPath(path), mDataVersion(dataVersion), mFlags(flags)
    {}
    ~MockServerCluster() override = default; // NOLINT(modernize-use-override)

    chip::Span<const ConcreteClusterPath> GetPaths() const override { return chip::Span<const ConcreteClusterPath>({ &mPath, 1 }); }
    chip::DataVersion GetDataVersion(const ConcreteClusterPath &) const override { return mDataVersion; }
    BitFlags<DataModel::ClusterQualityFlags> GetClusterFlags(const ConcreteClusterPath &) const override { return mFlags; }

    // Implement the pure virtual functions from DefaultServerCluster
    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest & request,
                                                AttributeValueEncoder & encoder) override
    {
        return DataModel::ActionReturnStatus(CHIP_ERROR_NOT_IMPLEMENTED);
    }
    DataModel::ActionReturnStatus WriteAttribute(const DataModel::WriteAttributeRequest & request,
                                                 AttributeValueDecoder & decoder) override
    {
        return DataModel::ActionReturnStatus(CHIP_ERROR_NOT_IMPLEMENTED);
    }
    std::optional<DataModel::ActionReturnStatus> InvokeCommand(const DataModel::InvokeRequest & request,
                                                               TLV::TLVReader & input_arguments, CommandHandler * handler) override
    {
        return std::nullopt;
    }
    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath & path,
                                ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> & builder) override
    {
        return CHIP_NO_ERROR;
    }
    CHIP_ERROR GeneratedCommands(const ConcreteClusterPath & path, ReadOnlyBufferBuilder<chip::CommandId> & builder) override
    {
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR Startup(ServerClusterContext & context) override
    {
        startupCalled = true;
        return DefaultServerCluster::Startup(context);
    }

    void Shutdown() override
    {
        shutdownCalled = true;
        DefaultServerCluster::Shutdown();
    }

    bool startupCalled  = false;
    bool shutdownCalled = false;

private:
    ConcreteClusterPath mPath;
    DataVersion mDataVersion;
    BitFlags<DataModel::ClusterQualityFlags> mFlags;
};

namespace {

constexpr DataModel::EndpointEntry endpointEntry1 = { .id                 = 1,
                                                      .parentId           = kInvalidEndpointId,
                                                      .compositionPattern = DataModel::EndpointCompositionPattern::kFullFamily };

constexpr DataModel::EndpointEntry endpointEntry2 = { .id                 = 2,
                                                      .parentId           = kInvalidEndpointId,
                                                      .compositionPattern = DataModel::EndpointCompositionPattern::kTree };

constexpr DataModel::EndpointEntry endpointEntry3 = { .id                 = 3,
                                                      .parentId           = kInvalidEndpointId,
                                                      .compositionPattern = DataModel::EndpointCompositionPattern::kFullFamily };

// SemanticTag is already an alias for chip::app::Clusters::Descriptor::Structs::SemanticTagStruct::Type
// from EndpointProviderInterface.h, so we can use it directly.
SemanticTag semanticTag1 = { .mfgCode     = VendorId::Google,
                             .namespaceID = 1,
                             .tag         = 1,
                             .label       = chip::Optional<chip::app::DataModel::Nullable<chip::CharSpan>>(
                                 { chip::app::DataModel::MakeNullable(chip::CharSpan("label1", 6)) }) };
SemanticTag semanticTag2 = { .mfgCode     = VendorId::Google,
                             .namespaceID = 2,
                             .tag         = 2,
                             .label       = chip::Optional<chip::app::DataModel::Nullable<chip::CharSpan>>(
                                 { chip::app::DataModel::MakeNullable(chip::CharSpan("label1", 6)) }) };

MockServerCluster mockServerCluster1(ConcreteClusterPath(1, 10), 1, BitFlags<DataModel::ClusterQualityFlags>());
MockServerCluster
    mockServerCluster2(ConcreteClusterPath(1, 20), 2,
                       BitFlags<DataModel::ClusterQualityFlags>().Set(DataModel::ClusterQualityFlags::kDiagnosticsData));
MockServerCluster mockServerCluster3(ConcreteClusterPath(2, 30), 3, BitFlags<DataModel::ClusterQualityFlags>());

constexpr chip::EndpointId clientClusterId1 = 1;
constexpr chip::EndpointId clientClusterId2 = 2;

// Define kMax constants locally for testing purposes
constexpr unsigned int kTestMaxDeviceTypes = 5;

} // namespace

TEST_F(TestCodeDrivenDataModelProvider, ClusterStartupIsCalledWhenAddingToStartedProvider)
{
    // Use a local provider, started up, to test adding an endpoint to an already started provider.
    // This ensures its lifecycle (including Shutdown) is fully contained within this test,
    // happening before local stack objects like testCluster are destroyed.
    CodeDrivenDataModelProvider localProvider;
    localProvider.SetPersistentStorageDelegate(&mServerClusterTestContext.StorageDelegate());
    ASSERT_EQ(localProvider.Startup(mContext), CHIP_NO_ERROR); // Provider is now "already started"

    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    MockServerCluster testCluster(ConcreteClusterPath(endpointEntry1.id, 100), 1, BitFlags<DataModel::ClusterQualityFlags>());
    ASSERT_FALSE(testCluster.startupCalled);

    ASSERT_EQ(epProvider->AddServerCluster(testCluster), CHIP_NO_ERROR);
    ASSERT_EQ(localProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);

    // Keep epProvider alive for the scope of localProvider by storing it locally.
    std::vector<std::unique_ptr<DynamicEndpointProvider>> localEndpointStorage;
    localEndpointStorage.push_back(std::move(epProvider));

    EXPECT_TRUE(testCluster.startupCalled);

    localProvider.Shutdown(); // Explicitly shutdown the local provider before local objects (testCluster, epProvider in
                              // localEndpointStorage) are destroyed.
}

TEST_F(TestCodeDrivenDataModelProvider, ClusterStartupNotCalledWhenAddingToNonStartedProviderThenCalledOnProviderStartup)
{
    CodeDrivenDataModelProvider localProvider; // Not started yet
    localProvider.SetPersistentStorageDelegate(&mServerClusterTestContext.StorageDelegate());

    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    MockServerCluster testCluster(ConcreteClusterPath(endpointEntry1.id, 101), 1, BitFlags<DataModel::ClusterQualityFlags>());
    ASSERT_FALSE(testCluster.startupCalled);

    ASSERT_EQ(epProvider->AddServerCluster(testCluster), CHIP_NO_ERROR);
    ASSERT_EQ(localProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);
    // Keep epProvider alive for the scope of localProvider
    std::vector<std::unique_ptr<DynamicEndpointProvider>> localEndpointStorage;
    localEndpointStorage.push_back(std::move(epProvider));

    EXPECT_FALSE(testCluster.startupCalled); // Should not be called yet

    // Now startup the provider
    ASSERT_EQ(localProvider.Startup(mContext), CHIP_NO_ERROR);
    EXPECT_TRUE(testCluster.startupCalled); // Should be called now

    localProvider.Shutdown();
}

TEST_F(TestCodeDrivenDataModelProvider, ClusterShutdownIsCalledWhenRemovingFromStartedProvider)
{
    // Use a local provider to ensure its lifecycle is fully contained within this test,
    // avoiding potential issues with the fixture's provider shutdown timing relative to
    // local stack objects like testCluster.
    CodeDrivenDataModelProvider localProvider;
    localProvider.SetPersistentStorageDelegate(&mServerClusterTestContext.StorageDelegate());
    ASSERT_EQ(localProvider.Startup(mContext), CHIP_NO_ERROR);

    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    ASSERT_NE(epProvider, nullptr);

    MockServerCluster testCluster(ConcreteClusterPath(endpointEntry1.id, 102), 1, BitFlags<DataModel::ClusterQualityFlags>());
    ASSERT_EQ(epProvider->AddServerCluster(testCluster), CHIP_NO_ERROR);
    // Add the endpoint provider to the localProvider.
    // The localProvider will call Startup() on testCluster's clusters if it's already started.
    ASSERT_EQ(localProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);

    ASSERT_TRUE(testCluster.startupCalled);
    ASSERT_FALSE(testCluster.shutdownCalled);

    // Remove the endpoint from the localProvider. This should call Shutdown() on testCluster.
    ASSERT_EQ(localProvider.RemoveEndpoint(endpointEntry1.id), CHIP_NO_ERROR);
    EXPECT_TRUE(testCluster.shutdownCalled);

    localProvider
        .Shutdown(); // Explicitly shutdown the local provider before local objects (testCluster, epProvider) are destroyed.
}

TEST_F(TestCodeDrivenDataModelProvider, ClusterShutdownNotCalledWhenRemovingFromNonStartedProvider)
{
    CodeDrivenDataModelProvider localProvider; // Not started
    localProvider.SetPersistentStorageDelegate(&mServerClusterTestContext.StorageDelegate());

    auto epProvider =
        std::make_unique<DynamicEndpointProvider>(endpointEntry1.id, endpointEntry1.compositionPattern, endpointEntry1.parentId);
    MockServerCluster testCluster(ConcreteClusterPath(endpointEntry1.id, 103), 1, BitFlags<DataModel::ClusterQualityFlags>());
    ASSERT_EQ(epProvider->AddServerCluster(testCluster), CHIP_NO_ERROR);
    ASSERT_EQ(localProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);
    // Keep epProvider alive for the scope of localProvider
    std::vector<std::unique_ptr<DynamicEndpointProvider>> localEndpointStorage;
    localEndpointStorage.push_back(std::move(epProvider));

    ASSERT_FALSE(testCluster.startupCalled);
    ASSERT_FALSE(testCluster.shutdownCalled);

    ASSERT_EQ(localProvider.RemoveEndpoint(endpointEntry1.id), CHIP_NO_ERROR);
    EXPECT_FALSE(testCluster.shutdownCalled); // Should not be called as provider was never started

    // localProvider will be destructed, which calls Shutdown, but the endpoint is already removed.
}

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

    ReadOnlyBufferBuilder<DataModel::ServerClusterEntry> builder;
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

    ReadOnlyBufferBuilder<chip::ClusterId> builder;
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
    DataModel::DeviceTypeEntry deviceType;
    for (unsigned int i = 0; i < kTestMaxDeviceTypes; i++)
    {
        deviceType.deviceTypeId = static_cast<uint32_t>(i);
        ASSERT_EQ(epProvider->AddDeviceType(deviceType), CHIP_NO_ERROR);
    }
    deviceType.deviceTypeId++;

    ASSERT_EQ(mProvider.AddEndpoint(*epProvider), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(epProvider));

    ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> builder;
    ASSERT_EQ(mProvider.DeviceTypes(endpointEntry1.id, builder), CHIP_NO_ERROR);

    auto deviceTypes = builder.TakeBuffer();
    ASSERT_EQ(deviceTypes.size(), static_cast<size_t>(kTestMaxDeviceTypes));
    for (unsigned int i = 0; i < kTestMaxDeviceTypes; i++)
    {
        EXPECT_EQ(deviceTypes[i].deviceTypeId, static_cast<uint32_t>(i));
    }
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
    ReadOnlyBufferBuilder<DataModel::EndpointEntry> out;
    EXPECT_EQ(mProvider.Endpoints(out), CHIP_NO_ERROR);
    EXPECT_EQ(out.Size(), 1U);

    auto endpoints = out.TakeBuffer();
    EXPECT_EQ(endpoints[0].id, endpointEntry2.id);
}
