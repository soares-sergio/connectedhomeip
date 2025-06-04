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

#include <algorithm> // For std::sort
#include <app/data-model-provider/MetadataTypes.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/ServerClusterContext.h>
#include <app/server-cluster/ServerClusterInterface.h>
#include <app/server-cluster/testing/TestServerClusterContext.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/endpoint/SpanEndpointProvider.h>
#include <lib/support/ReadOnlyBuffer.h>
#include <lib/support/Span.h>
#include <list>
#include <vector> // For std::vector

using namespace chip;
using namespace chip::app;
using SemanticTag = chip::app::Clusters::Descriptor::Structs::SemanticTagStruct::Type;

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
    std::vector<std::unique_ptr<SpanEndpointProvider>> mEndpointStorage; // To keep providers alive
    std::list<EndpointProviderRegistration> mOwnedRegistrations;         // To keep registration objects alive
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
        mOwnedRegistrations.clear();
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

    chip::Span<const ConcreteClusterPath> GetPaths() const override { return chip::Span<const ConcreteClusterPath>(&mPath, 1); }
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

class MockServerClusterMultiPath : public MockServerCluster
{
public:
    MockServerClusterMultiPath(std::initializer_list<ConcreteClusterPath> paths, DataVersion dataVersion,
                               BitFlags<DataModel::ClusterQualityFlags> flags) :
        MockServerCluster({ 0, 0 }, dataVersion, flags), mPaths(paths)
    {}
    ~MockServerClusterMultiPath() override = default; // NOLINT(modernize-use-override)

    chip::Span<const ConcreteClusterPath> GetPaths() const override
    {
        return chip::Span<const ConcreteClusterPath>(mPaths.data(), mPaths.size());
    }

private:
    const std::vector<ConcreteClusterPath> mPaths;
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

MockServerClusterMultiPath mockServerClusterMultiPath({ { 3, 40 }, { 3, 50 } }, 1, BitFlags<DataModel::ClusterQualityFlags>());

constexpr chip::EndpointId clientClusterId1 = 1;
constexpr chip::EndpointId clientClusterId2 = 2;

// Define kMax constants locally for testing purposes
constexpr unsigned int kTestMaxDeviceTypes = 5;

// Static data for EndpointProvider tests
constexpr EndpointId kEpProviderTestEndpointId           = 4; // Use a new ID to avoid conflicts
constexpr DataModel::EndpointEntry kEpProviderTestEntry1 = { .id       = kEpProviderTestEndpointId,
                                                             .parentId = kInvalidEndpointId,
                                                             .compositionPattern =
                                                                 DataModel::EndpointCompositionPattern::kFullFamily };

} // namespace

TEST_F(TestCodeDrivenDataModelProvider, IterateOverEndpoints)
{
    auto build_pair1 = SpanEndpointProvider::Builder(endpointEntry1.id)
                           .SetComposition(endpointEntry1.compositionPattern)
                           .SetParentId(endpointEntry1.parentId)
                           .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair1));
    auto ep1Provider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair1)));

    auto build_pair2 = SpanEndpointProvider::Builder(endpointEntry2.id)
                           .SetComposition(endpointEntry2.compositionPattern)
                           .SetParentId(endpointEntry2.parentId)
                           .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair2));
    auto ep2Provider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair2)));

    auto build_pair3 = SpanEndpointProvider::Builder(endpointEntry3.id)
                           .SetComposition(endpointEntry3.compositionPattern)
                           .SetParentId(endpointEntry3.parentId)
                           .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair3));
    auto ep3Provider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair3)));

    mEndpointStorage.push_back(std::move(ep1Provider));
    mOwnedRegistrations.emplace_back(*mEndpointStorage.back());
    ASSERT_EQ(mProvider.AddEndpoint(mOwnedRegistrations.back()), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(ep2Provider));
    mOwnedRegistrations.emplace_back(*mEndpointStorage.back());
    ASSERT_EQ(mProvider.AddEndpoint(mOwnedRegistrations.back()), CHIP_NO_ERROR);
    mEndpointStorage.push_back(std::move(ep3Provider));
    mOwnedRegistrations.emplace_back(*mEndpointStorage.back());
    ASSERT_EQ(mProvider.AddEndpoint(mOwnedRegistrations.back()), CHIP_NO_ERROR);

    ReadOnlyBufferBuilder<DataModel::EndpointEntry> endpointsBuilder;

    ASSERT_EQ(mProvider.Endpoints(endpointsBuilder), CHIP_NO_ERROR);

    ReadOnlyBuffer<DataModel::EndpointEntry> endpoints_rb = endpointsBuilder.TakeBuffer();
    // ReadOnlyBuffer<T> inherits from Span<const T>, so we can use its iterators directly.
    std::vector<DataModel::EndpointEntry> actual_endpoints(endpoints_rb.begin(), endpoints_rb.end());
    // Sort actual endpoints by ID to make the test order-independent
    std::sort(actual_endpoints.begin(), actual_endpoints.end(),
              [](const DataModel::EndpointEntry & a, const DataModel::EndpointEntry & b) { return a.id < b.id; });

    ASSERT_EQ(actual_endpoints.size(), 3u);

    // Compare against expected entries, assuming expected entries are defined in ID-sorted order
    EXPECT_EQ(actual_endpoints[0], endpointEntry1);
    EXPECT_EQ(actual_endpoints[1], endpointEntry2);
    EXPECT_EQ(actual_endpoints[2], endpointEntry3);
}

TEST_F(TestCodeDrivenDataModelProvider, IterateOverServerClusters)
{
    // mockServerCluster1 and mockServerCluster2 are global.
    static ServerClusterInterface * sServerClustersArray[] = { &mockServerCluster1, &mockServerCluster2 };

    auto build_pair = SpanEndpointProvider::Builder(endpointEntry1.id)
                          .SetComposition(endpointEntry1.compositionPattern)
                          .SetParentId(endpointEntry1.parentId)
                          .SetServerClusters(Span<ServerClusterInterface *>(sServerClustersArray))
                          .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair));
    auto epProvider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair)));

    mEndpointStorage.push_back(std::move(epProvider));
    mOwnedRegistrations.emplace_back(*mEndpointStorage.back());
    ASSERT_EQ(mProvider.AddEndpoint(mOwnedRegistrations.back()), CHIP_NO_ERROR);

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
    // clientClusterId1 and clientClusterId2 are global.
    static const ClusterId sClientClustersArray[] = { clientClusterId1, clientClusterId2 };

    auto build_pair = SpanEndpointProvider::Builder(endpointEntry1.id)
                          .SetComposition(endpointEntry1.compositionPattern)
                          .SetParentId(endpointEntry1.parentId)
                          .SetClientClusters(Span<const ClusterId>(sClientClustersArray))
                          .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair));
    auto epProvider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair)));

    mEndpointStorage.push_back(std::move(epProvider));
    mOwnedRegistrations.emplace_back(*mEndpointStorage.back());
    ASSERT_EQ(mProvider.AddEndpoint(mOwnedRegistrations.back()), CHIP_NO_ERROR);

    ReadOnlyBufferBuilder<chip::ClusterId> builder;
    ASSERT_EQ(mProvider.ClientClusters(endpointEntry1.id, builder), CHIP_NO_ERROR);

    auto clientClusters = builder.TakeBuffer();
    ASSERT_EQ(clientClusters.size(), 2u);
    EXPECT_EQ(clientClusters[0], clientClusterId1);
    EXPECT_EQ(clientClusters[1], clientClusterId2);
}

TEST_F(TestCodeDrivenDataModelProvider, IterateOverTags)
{
    // semanticTag1 and semanticTag2 are global.
    static const SemanticTag sSemanticTagsArray[] = { semanticTag1, semanticTag2 };

    auto build_pair = SpanEndpointProvider::Builder(endpointEntry1.id)
                          .SetComposition(endpointEntry1.compositionPattern)
                          .SetParentId(endpointEntry1.parentId)
                          .SetSemanticTags(Span<const SemanticTag>(sSemanticTagsArray))
                          .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair));
    auto epProvider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair)));

    mEndpointStorage.push_back(std::move(epProvider));
    mOwnedRegistrations.emplace_back(*mEndpointStorage.back());
    ASSERT_EQ(mProvider.AddEndpoint(mOwnedRegistrations.back()), CHIP_NO_ERROR);

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
    static DataModel::DeviceTypeEntry sDeviceTypesData[kTestMaxDeviceTypes];
    for (unsigned int i = 0; i < kTestMaxDeviceTypes; i++)
    {
        sDeviceTypesData[i].deviceTypeId       = static_cast<uint32_t>(i);
        sDeviceTypesData[i].deviceTypeRevision = static_cast<uint8_t>((i % 255) + 1); // Ensure non-zero and varying revision
    }

    auto build_pair = SpanEndpointProvider::Builder(endpointEntry1.id)
                          .SetComposition(endpointEntry1.compositionPattern)
                          .SetParentId(endpointEntry1.parentId)
                          .SetDeviceTypes(Span<const DataModel::DeviceTypeEntry>(sDeviceTypesData))
                          .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair));
    auto epProvider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair)));

    mEndpointStorage.push_back(std::move(epProvider));
    mOwnedRegistrations.emplace_back(*mEndpointStorage.back());
    ASSERT_EQ(mProvider.AddEndpoint(mOwnedRegistrations.back()), CHIP_NO_ERROR);

    ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> builder;
    ASSERT_EQ(mProvider.DeviceTypes(endpointEntry1.id, builder), CHIP_NO_ERROR);

    auto deviceTypesResult = builder.TakeBuffer();
    ASSERT_EQ(deviceTypesResult.size(), static_cast<size_t>(kTestMaxDeviceTypes));
    for (unsigned int i = 0; i < kTestMaxDeviceTypes; i++)
    {
        EXPECT_EQ(deviceTypesResult[i].deviceTypeId, static_cast<uint32_t>(i));
        EXPECT_EQ(deviceTypesResult[i].deviceTypeRevision, static_cast<uint8_t>((i % 255) + 1));
    }
}

TEST_F(TestCodeDrivenDataModelProvider, AddAndRemoveEndpoints)
{
    // Add 3 endpoints
    auto build_pair1 = SpanEndpointProvider::Builder(endpointEntry1.id)
                           .SetComposition(endpointEntry1.compositionPattern)
                           .SetParentId(endpointEntry1.parentId)
                           .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair1));
    auto ep1Provider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair1)));

    auto build_pair2 = SpanEndpointProvider::Builder(endpointEntry2.id)
                           .SetComposition(endpointEntry2.compositionPattern)
                           .SetParentId(endpointEntry2.parentId)
                           .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair2));
    auto ep2Provider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair2)));

    auto build_pair3 = SpanEndpointProvider::Builder(endpointEntry3.id)
                           .SetComposition(endpointEntry3.compositionPattern)
                           .SetParentId(endpointEntry3.parentId)
                           .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair3));
    auto ep3Provider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair3)));

    mEndpointStorage.push_back(std::move(ep1Provider));
    mOwnedRegistrations.emplace_back(*mEndpointStorage.back()); // ep1Provider is moved, back() is now ep1Provider
    EXPECT_EQ(mProvider.AddEndpoint(mOwnedRegistrations.back()), CHIP_NO_ERROR);

    mEndpointStorage.push_back(std::move(ep2Provider)); // Will be kept
    mOwnedRegistrations.emplace_back(*mEndpointStorage.back());
    EXPECT_EQ(mProvider.AddEndpoint(mOwnedRegistrations.back()), CHIP_NO_ERROR);

    mEndpointStorage.push_back(std::move(ep3Provider));
    mOwnedRegistrations.emplace_back(*mEndpointStorage.back());
    EXPECT_EQ(mProvider.AddEndpoint(mOwnedRegistrations.back()), CHIP_NO_ERROR);

    // Remove endpoints 1 and 3
    EXPECT_EQ(mProvider.RemoveEndpoint(endpointEntry1.id), CHIP_NO_ERROR);
    EXPECT_EQ(mProvider.RemoveEndpoint(endpointEntry3.id), CHIP_NO_ERROR);

    // Remove from mOwnedRegistrations first, while the SpanEndpointProvider objects are still valid
    mOwnedRegistrations.remove_if([&](const EndpointProviderRegistration & reg) {
        return reg.endpointProviderInterface->GetEndpointEntry().id == endpointEntry1.id ||
            reg.endpointProviderInterface->GetEndpointEntry().id == endpointEntry3.id;
    });

    // Then remove from mEndpointStorage, which will delete the SpanEndpointProvider objects
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

TEST_F(TestCodeDrivenDataModelProvider, EndpointProviderWithStaticData)
{
    CodeDrivenDataModelProvider localProvider; // Use a local provider for focused testing
    localProvider.SetPersistentStorageDelegate(&mServerClusterTestContext.StorageDelegate());
    ASSERT_EQ(localProvider.Startup(mContext), CHIP_NO_ERROR);

    // Define static data directly in arrays. MockServerCluster instances need to be static
    // or have a lifetime that outlasts their use by the Span.
    static MockServerCluster mockEpServerCluster1(ConcreteClusterPath(kEpProviderTestEndpointId, 300), 1,
                                                  BitFlags<DataModel::ClusterQualityFlags>());
    static MockServerCluster mockEpServerCluster2(ConcreteClusterPath(kEpProviderTestEndpointId, 301), 2,
                                                  BitFlags<DataModel::ClusterQualityFlags>());

    static ServerClusterInterface * serverClustersArray[]      = { &mockEpServerCluster1, &mockEpServerCluster2 };
    static const ClusterId clientClustersArray[]               = { 0xD001, 0xD002 };
    static const SemanticTag semanticTagsArray[]               = { { .mfgCode = VendorId::Google, .namespaceID = 10, .tag = 100 },
                                                                   { .mfgCode = VendorId::Google, .namespaceID = 11, .tag = 101 } };
    static const DataModel::DeviceTypeEntry deviceTypesArray[] = { { .deviceTypeId = 0x7001, .deviceTypeRevision = 1 },
                                                                   { .deviceTypeId = 0x7002, .deviceTypeRevision = 2 } };

    auto build_pair = SpanEndpointProvider::Builder(kEpProviderTestEntry1.id)
                          .SetComposition(kEpProviderTestEntry1.compositionPattern)
                          .SetParentId(kEpProviderTestEntry1.parentId)
                          .SetServerClusters(chip::Span<ServerClusterInterface *>(serverClustersArray))
                          .SetClientClusters(chip::Span<const ClusterId>(clientClustersArray))
                          .SetSemanticTags(chip::Span<const SemanticTag>(semanticTagsArray))
                          .SetDeviceTypes(chip::Span<const DataModel::DeviceTypeEntry>(deviceTypesArray))
                          .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair));
    SpanEndpointProvider ep = std::move(std::get<SpanEndpointProvider>(build_pair));

    EndpointProviderRegistration registration(ep);
    ASSERT_EQ(localProvider.AddEndpoint(registration), CHIP_NO_ERROR);

    // Verify Endpoints through localProvider
    ReadOnlyBufferBuilder<DataModel::EndpointEntry> epBuilder;
    ASSERT_EQ(localProvider.Endpoints(epBuilder), CHIP_NO_ERROR);
    auto eps = epBuilder.TakeBuffer();
    ASSERT_EQ(eps.size(), 1u);
    EXPECT_EQ(eps[0].id, kEpProviderTestEndpointId);
    EXPECT_EQ(eps[0].compositionPattern, kEpProviderTestEntry1.compositionPattern);

    // Verify Server Clusters
    ReadOnlyBufferBuilder<DataModel::ServerClusterEntry> scBuilder;
    ASSERT_EQ(localProvider.ServerClusters(kEpProviderTestEndpointId, scBuilder), CHIP_NO_ERROR);
    auto scs = scBuilder.TakeBuffer();
    ASSERT_EQ(scs.size(), std::size(serverClustersArray));
    EXPECT_EQ(scs[0].clusterId, mockEpServerCluster1.GetPaths()[0].mClusterId);
    EXPECT_EQ(scs[1].clusterId, mockEpServerCluster2.GetPaths()[0].mClusterId);

    // Verify Client Clusters
    ReadOnlyBufferBuilder<ClusterId> ccBuilder;
    ASSERT_EQ(localProvider.ClientClusters(kEpProviderTestEndpointId, ccBuilder), CHIP_NO_ERROR);
    auto ccs = ccBuilder.TakeBuffer();
    ASSERT_EQ(ccs.size(), std::size(clientClustersArray));
    EXPECT_EQ(ccs[0], clientClustersArray[0]);
    EXPECT_EQ(ccs[1], clientClustersArray[1]);

    // Verify Semantic Tags
    ReadOnlyBufferBuilder<SemanticTag> stBuilder;
    ASSERT_EQ(localProvider.SemanticTags(kEpProviderTestEndpointId, stBuilder), CHIP_NO_ERROR);
    auto sts = stBuilder.TakeBuffer();
    ASSERT_EQ(sts.size(), std::size(semanticTagsArray));
    EXPECT_EQ(sts[0].mfgCode, semanticTagsArray[0].mfgCode);
    EXPECT_EQ(sts[0].namespaceID, semanticTagsArray[0].namespaceID);
    EXPECT_EQ(sts[0].tag, semanticTagsArray[0].tag);
    EXPECT_EQ(sts[1].mfgCode, semanticTagsArray[1].mfgCode);
    EXPECT_EQ(sts[1].namespaceID, semanticTagsArray[1].namespaceID);
    EXPECT_EQ(sts[1].tag, semanticTagsArray[1].tag);

    // Verify Device Types
    ReadOnlyBufferBuilder<DataModel::DeviceTypeEntry> dtBuilder;
    ASSERT_EQ(localProvider.DeviceTypes(kEpProviderTestEndpointId, dtBuilder), CHIP_NO_ERROR);
    auto dts = dtBuilder.TakeBuffer();
    ASSERT_EQ(dts.size(), std::size(deviceTypesArray));
    EXPECT_EQ(dts[0].deviceTypeId, deviceTypesArray[0].deviceTypeId);
    EXPECT_EQ(dts[0].deviceTypeRevision, deviceTypesArray[0].deviceTypeRevision);
    EXPECT_EQ(dts[1].deviceTypeId, deviceTypesArray[1].deviceTypeId);
    EXPECT_EQ(dts[1].deviceTypeRevision, deviceTypesArray[1].deviceTypeRevision);

    // Verify GetServerCluster on EndpointProvider instance
    EXPECT_EQ(ep.GetServerCluster(mockEpServerCluster1.GetPaths()[0].mClusterId), &mockEpServerCluster1);
    EXPECT_EQ(ep.GetServerCluster(mockEpServerCluster2.GetPaths()[0].mClusterId), &mockEpServerCluster2);
    EXPECT_EQ(ep.GetServerCluster(0xFFFF /* Non-existent cluster ID */), nullptr);

    localProvider.Shutdown();
}

TEST_F(TestCodeDrivenDataModelProvider, EndpointProviderWithEmptyStaticData)
{
    auto build_pair = SpanEndpointProvider::Builder(kEpProviderTestEntry1.id)
                          .SetComposition(kEpProviderTestEntry1.compositionPattern)
                          .SetParentId(kEpProviderTestEntry1.parentId)
                          .SetServerClusters(Span<ServerClusterInterface *>())
                          .SetClientClusters(Span<const ClusterId>())
                          .SetSemanticTags(Span<const SemanticTag>())
                          .SetDeviceTypes(Span<const DataModel::DeviceTypeEntry>())
                          .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair));
    SpanEndpointProvider ep = std::move(std::get<SpanEndpointProvider>(build_pair));

    ReadOnlyBufferBuilder<ServerClusterInterface *> scBuilder;
    EXPECT_EQ(ep.ServerClusterInterfaces(scBuilder), CHIP_NO_ERROR);
    EXPECT_EQ(scBuilder.Size(), 0u);
}

TEST_F(TestCodeDrivenDataModelProvider, EndpointProviderSetInvalidServerCluster)
{
    static MockServerCluster mockEpServerCluster1_local(ConcreteClusterPath(kEpProviderTestEndpointId, 400), 1,
                                                        BitFlags<DataModel::ClusterQualityFlags>());
    ServerClusterInterface * serverClustersArrayWithNull[] = { &mockEpServerCluster1_local, nullptr };

    auto build_pair = SpanEndpointProvider::Builder(kEpProviderTestEntry1.id)
                          .SetComposition(kEpProviderTestEntry1.compositionPattern)
                          .SetParentId(kEpProviderTestEntry1.parentId)
                          .SetServerClusters(chip::Span<ServerClusterInterface *>(serverClustersArrayWithNull))
                          .build();
    ASSERT_TRUE(std::holds_alternative<CHIP_ERROR>(build_pair));
    EXPECT_EQ(std::get<CHIP_ERROR>(build_pair), CHIP_ERROR_INVALID_ARGUMENT);
}

TEST_F(TestCodeDrivenDataModelProvider, ClusterStartupIsCalledWhenAddingToStartedProvider)
{
    // Use a local provider, started up, to test adding an endpoint to an already started provider.
    // This ensures its lifecycle (including Shutdown) is fully contained within this test,
    // happening before local stack objects like testCluster are destroyed.
    CodeDrivenDataModelProvider localProvider;
    localProvider.SetPersistentStorageDelegate(&mServerClusterTestContext.StorageDelegate());
    ASSERT_EQ(localProvider.Startup(mContext), CHIP_NO_ERROR); // Provider is now "already started"

    MockServerCluster testCluster(ConcreteClusterPath(endpointEntry1.id, 100), 1, BitFlags<DataModel::ClusterQualityFlags>());
    ASSERT_FALSE(testCluster.startupCalled);

    ServerClusterInterface * serverClustersArray[] = { &testCluster }; // Local, lifetime OK with localEndpointStorage

    auto build_pair = SpanEndpointProvider::Builder(endpointEntry1.id)
                          .SetComposition(endpointEntry1.compositionPattern)
                          .SetParentId(endpointEntry1.parentId)
                          .SetServerClusters(Span<ServerClusterInterface *>(serverClustersArray))
                          .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair));
    auto epProvider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair)));
    // Keep epProvider alive for the scope of localProvider by storing it locally.
    std::vector<std::unique_ptr<SpanEndpointProvider>> localEndpointStorage;
    localEndpointStorage.push_back(std::move(epProvider));

    EndpointProviderRegistration registration(*localEndpointStorage.back());
    ASSERT_EQ(localProvider.AddEndpoint(registration), CHIP_NO_ERROR);

    EXPECT_TRUE(testCluster.startupCalled);

    localProvider.Shutdown(); // Explicitly shutdown the local provider before local objects (testCluster, epProvider in
                              // localEndpointStorage) are destroyed.
}

TEST_F(TestCodeDrivenDataModelProvider, ClusterStartupNotCalledWhenAddingToNonStartedProviderThenCalledOnProviderStartup)
{
    CodeDrivenDataModelProvider localProvider; // Not started yet
    localProvider.SetPersistentStorageDelegate(&mServerClusterTestContext.StorageDelegate());

    MockServerCluster testCluster(ConcreteClusterPath(endpointEntry1.id, 101), 1, BitFlags<DataModel::ClusterQualityFlags>());
    ASSERT_FALSE(testCluster.startupCalled);

    ServerClusterInterface * serverClustersArray[] = { &testCluster }; // Local, lifetime OK with localEndpointStorage

    auto build_pair = SpanEndpointProvider::Builder(endpointEntry1.id)
                          .SetComposition(endpointEntry1.compositionPattern)
                          .SetParentId(endpointEntry1.parentId)
                          .SetServerClusters(Span<ServerClusterInterface *>(serverClustersArray))
                          .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair));
    auto epProvider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair)));
    std::vector<std::unique_ptr<SpanEndpointProvider>> localEndpointStorage;
    localEndpointStorage.push_back(std::move(epProvider));

    EndpointProviderRegistration registration(*localEndpointStorage.back());
    ASSERT_EQ(localProvider.AddEndpoint(registration), CHIP_NO_ERROR);

    EXPECT_FALSE(
        testCluster.startupCalled); // Startup on cluster should not be called directly when adding to a non-started provider

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

    MockServerCluster testCluster(ConcreteClusterPath(endpointEntry1.id, 102), 1, BitFlags<DataModel::ClusterQualityFlags>());
    ServerClusterInterface * serverClustersArray[] = { &testCluster }; // Local, data is testCluster which is also local

    auto build_pair = SpanEndpointProvider::Builder(endpointEntry1.id)
                          .SetComposition(endpointEntry1.compositionPattern)
                          .SetParentId(endpointEntry1.parentId)
                          .SetServerClusters(Span<ServerClusterInterface *>(serverClustersArray))
                          .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair));
    auto epProvider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair)));

    // Add the endpoint provider to the localProvider.
    // The localProvider will call Startup() on testCluster's clusters if it's already started.
    std::vector<std::unique_ptr<SpanEndpointProvider>> localEndpointStorage; // Keep epProvider alive
    localEndpointStorage.push_back(std::move(epProvider));
    EndpointProviderRegistration registration(*localEndpointStorage.back());
    ASSERT_EQ(localProvider.AddEndpoint(registration), CHIP_NO_ERROR);

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

    MockServerCluster testCluster(ConcreteClusterPath(endpointEntry1.id, 103), 1, BitFlags<DataModel::ClusterQualityFlags>());
    ServerClusterInterface * serverClustersArray[] = { &testCluster }; // Local, data is testCluster which is also local

    auto build_pair = SpanEndpointProvider::Builder(endpointEntry1.id)
                          .SetComposition(endpointEntry1.compositionPattern)
                          .SetParentId(endpointEntry1.parentId)
                          .SetServerClusters(Span<ServerClusterInterface *>(serverClustersArray))
                          .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair));
    auto epProvider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair)));
    std::vector<std::unique_ptr<SpanEndpointProvider>> localEndpointStorage;
    localEndpointStorage.push_back(std::move(epProvider));

    EndpointProviderRegistration registration(*localEndpointStorage.back());
    ASSERT_EQ(localProvider.AddEndpoint(registration), CHIP_NO_ERROR);

    ASSERT_FALSE(testCluster.startupCalled);
    ASSERT_FALSE(testCluster.shutdownCalled);

    ASSERT_EQ(localProvider.RemoveEndpoint(endpointEntry1.id), CHIP_NO_ERROR);
    EXPECT_FALSE(testCluster.shutdownCalled); // Should not be called as provider was never started

    // localProvider will be destructed, which calls Shutdown, but the endpoint is already removed.
}

// Check that multiple paths for a single cluster are handled correctly
TEST_F(TestCodeDrivenDataModelProvider, ServerClustersMultiPath)
{
    // mockServerCluster1 and mockServerCluster2 are global.
    static ServerClusterInterface * sServerClustersArray[] = { &mockServerClusterMultiPath };

    auto build_pair = SpanEndpointProvider::Builder(endpointEntry1.id)
                          .SetComposition(endpointEntry1.compositionPattern)
                          .SetParentId(endpointEntry1.parentId)
                          .SetServerClusters(Span<ServerClusterInterface *>(sServerClustersArray))
                          .build();
    ASSERT_TRUE(std::holds_alternative<SpanEndpointProvider>(build_pair));
    auto epProvider = std::make_unique<SpanEndpointProvider>(std::move(std::get<SpanEndpointProvider>(build_pair)));

    mEndpointStorage.push_back(std::move(epProvider));
    mOwnedRegistrations.emplace_back(*mEndpointStorage.back());
    ASSERT_EQ(mProvider.AddEndpoint(mOwnedRegistrations.back()), CHIP_NO_ERROR);

    ReadOnlyBufferBuilder<DataModel::ServerClusterEntry> builder;
    ASSERT_EQ(mProvider.ServerClusters(endpointEntry1.id, builder), CHIP_NO_ERROR);

    auto serverClusters = builder.TakeBuffer();
    ASSERT_EQ(serverClusters.size(), 2u);

    EXPECT_EQ(serverClusters[0].clusterId, mockServerClusterMultiPath.GetPaths()[0].mClusterId);
    EXPECT_EQ(serverClusters[0].dataVersion, mockServerClusterMultiPath.GetDataVersion({}));
    EXPECT_EQ(serverClusters[0].flags, mockServerClusterMultiPath.GetClusterFlags({}));

    EXPECT_EQ(serverClusters[1].clusterId, mockServerClusterMultiPath.GetPaths()[1].mClusterId);
    EXPECT_EQ(serverClusters[1].dataVersion, mockServerClusterMultiPath.GetDataVersion({}));
    EXPECT_EQ(serverClusters[1].flags, mockServerClusterMultiPath.GetClusterFlags({}));
}
