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

class TestAllDevicesApp : public ::testing::Test
{
public:
    static void SetUpTestSuite() { ASSERT_EQ(chip::Platform::MemoryInit(), CHIP_NO_ERROR); }
    static void TearDownTestSuite() { chip::Platform::MemoryShutdown(); }

protected:
    chip::Test::TestProviderChangeListener mChangeListener;
    chip::Test::LogOnlyEvents mEventGenerator;
    TestActionContext mActionContext;
    InteractionModelContext mContext{
        .eventsGenerator         = mEventGenerator,
        .dataModelChangeListener = mChangeListener,
        .actionContext           = mActionContext,
    };
    CodeDrivenDataModelProvider * mProvider = nullptr;
    chip::Test::TestServerClusterContext mServerClusterTestContext;

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

TEST_F(TestAllDevicesApp, GetAppDataModel)
{
    mProvider = &GetAppDataModelProvider();
    ASSERT_TRUE(mProvider != nullptr);

    // Check there is nothing in the DM Provider yet
    ReadOnlyBufferBuilder<DataModel::EndpointEntry> endpointsBuilder;
    ASSERT_EQ(mProvider->Endpoints(endpointsBuilder), CHIP_NO_ERROR);
    auto endpoints = endpointsBuilder.TakeBuffer();
    ASSERT_EQ(endpoints.size(), 0u);
}
