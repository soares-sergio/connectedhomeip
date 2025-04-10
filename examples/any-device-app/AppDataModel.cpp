#include "AppDataModel.h"
#include <cassert>

#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/DynamicEndpointProvider.h>
#include <data-model-providers/codegen/CodegenServerCluster.h>

/*
 * Note: This is temporarily backed by a Zap file located in the `legacy_to_be_removed` directory.
 * While clusters are being migrated from Ember-based to Code-driven, this code uses
 * a wrapper (named `CodegenServerCluster`) around Ember-based clusters (zap) so we can
 * develop apps using the new `ServerClusterInterface` and `CodeDrivenDataModelProvider`.
 * The Zap file contains all clusters required for all apps. Then, the `CodegenServerCluster`
 * instantiated per app, filters in only what's needed for the specific app and hides everything
 * else.
 */

using chip::app::CodeDrivenDataModelProvider;
using chip::app::CodegenServerCluster;
using chip::app::DynamicEndpointProvider;
using chip::app::DataModel::EndpointCompositionPattern;
using chip::app::DataModel::Provider;

namespace {

class ContactSensorAppDataModelProvider : public CodeDrivenDataModelProvider
{
public:
    ContactSensorAppDataModelProvider() :
        mEndpoint0(0, EndpointCompositionPattern::kFullFamily), mEndpoint1(1, EndpointCompositionPattern::kFullFamily)
    {
        // TODO(sergiosoares): initialize data model here
        mEndpoint0.AddServerCluster(mEndpoint0Clusters);
        mEndpoint1.AddServerCluster(mEndpoint1Clusters);
        AddEndpoint(mEndpoint0);
        AddEndpoint(mEndpoint1);
    }

private:
    CodegenServerCluster mEndpoint0Clusters{ { 0, 1 }, { 0, 2 } }; // TODO(sergiosoares): fix paths
    CodegenServerCluster mEndpoint1Clusters{ { 1, 3 }, { 1, 4 } }; // TODO(sergiosoares): fix paths
    DynamicEndpointProvider mEndpoint0;
    DynamicEndpointProvider mEndpoint1;
};

class LightingAppDataModelProvider : public CodeDrivenDataModelProvider
{
public:
    LightingAppDataModelProvider() :
        mEndpoint0(0, EndpointCompositionPattern::kFullFamily), mEndpoint1(1, EndpointCompositionPattern::kFullFamily)
    {
        // TODO(sergiosoares): initialize data model here
        mEndpoint0.AddServerCluster(mEndpoint0Clusters);
        mEndpoint1.AddServerCluster(mEndpoint1Clusters);
        AddEndpoint(mEndpoint0);
        AddEndpoint(mEndpoint1);
    }

private:
    CodegenServerCluster mEndpoint0Clusters{ { 0, 1 }, { 0, 2 } }; // TODO(sergiosoares): fix paths
    CodegenServerCluster mEndpoint1Clusters{ { 1, 3 }, { 1, 4 } }; // TODO(sergiosoares): fix paths
    DynamicEndpointProvider mEndpoint0;
    DynamicEndpointProvider mEndpoint1;
};

} // namespace
CodeDrivenDataModelProvider & GetContactSensorDataModelProvider()
{
    static ContactSensorAppDataModelProvider contactSensorAppDataModelProvider;
    return contactSensorAppDataModelProvider;
}

CodeDrivenDataModelProvider & GetLightingDataModelProvider()
{
    static LightingAppDataModelProvider lightingAppDataModelProvider;
    return lightingAppDataModelProvider;
}
