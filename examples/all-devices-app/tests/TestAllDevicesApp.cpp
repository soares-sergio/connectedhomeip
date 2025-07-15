#include <AppDataModel.h>
#include <app/data-model-provider/MetadataTypes.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <app/server-cluster/testing/TestServerClusterContext.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/endpoint/SpanEndpoint.h>
#include <lib/support/ReadOnlyBuffer.h>
#include <pw_unit_test/framework.h>

#include <memory> // For std::unique_ptr
#include <vector> // For std::vector

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

class TestAllDevicesApp : public ::testing::Test
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
    CodeDrivenDataModelProvider * mProvider = nullptr;
    chip::Test::TestServerClusterContext mServerClusterTestContext;

    CHIP_ERROR Startup(AppTypeEnum appType)
    {
        mProvider = &GetAppDataModelProvider(appType);
        // mProvider->SetPersistentStorageDelegate(&mServerClusterTestContext.StorageDelegate());
        return mProvider->Startup(mContext);
    }

    TestAllDevicesApp() {}

    ~TestAllDevicesApp() override
    {
        if (mProvider != nullptr)
        {
            mProvider->Shutdown();
        }
        mProvider = nullptr;
    }
};

namespace {} // namespace

TEST_F(TestAllDevicesApp, GetContactSensorAppDataModel)
{
    Startup(AppTypeEnum::kContactSensorApp);
    ASSERT_TRUE(mProvider != nullptr);

    ReadOnlyBufferBuilder<DataModel::EndpointEntry> endpointsBuilder;
    ASSERT_EQ(mProvider->Endpoints(endpointsBuilder), CHIP_NO_ERROR);

    auto endpoints = endpointsBuilder.TakeBuffer();

    ASSERT_EQ(endpoints.size(), 2u);

    constexpr EndpointEntry expectedEndpoint0 = { .id                 = 0,
                                                  .parentId           = chip::kInvalidEndpointId,
                                                  .compositionPattern = EndpointCompositionPattern::kFullFamily };

    constexpr EndpointEntry expectedEndpoint1 = { .id                 = 1,
                                                  .parentId           = chip::kInvalidEndpointId,
                                                  .compositionPattern = EndpointCompositionPattern::kFullFamily };

    EXPECT_EQ(endpoints[0], expectedEndpoint0);
    EXPECT_EQ(endpoints[1], expectedEndpoint1);
}

TEST_F(TestAllDevicesApp, GetLightingAppDataModel)
{
    Startup(AppTypeEnum::kLightingApp);
    ASSERT_TRUE(mProvider != nullptr);

    ReadOnlyBufferBuilder<DataModel::EndpointEntry> endpointsBuilder;
    ASSERT_EQ(mProvider->Endpoints(endpointsBuilder), CHIP_NO_ERROR);

    auto endpoints = endpointsBuilder.TakeBuffer();

    ASSERT_EQ(endpoints.size(), 2u);

    constexpr EndpointEntry expectedEndpoint0 = { .id                 = 0,
                                                  .parentId           = chip::kInvalidEndpointId,
                                                  .compositionPattern = EndpointCompositionPattern::kFullFamily };

    constexpr EndpointEntry expectedEndpoint1 = { .id                 = 1,
                                                  .parentId           = chip::kInvalidEndpointId,
                                                  .compositionPattern = EndpointCompositionPattern::kFullFamily };

    EXPECT_EQ(endpoints[0], expectedEndpoint0);
    EXPECT_EQ(endpoints[1], expectedEndpoint1);
}
