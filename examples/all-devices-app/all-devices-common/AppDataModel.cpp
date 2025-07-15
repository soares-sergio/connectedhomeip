#include "AppDataModel.h"
#include <cassert>

#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/endpoint/SpanEndpoint.h>
// #include <server-cluster-shim/ServerClusterShim.h>
#include <lib/core/CHIPPersistentStorageDelegate.h>

// #include <app/server/Server.h>
#include <platform/KeyValueStoreManager.h>
#include <platform/KvsPersistentStorageDelegate.h>

// TODO: include ServerClusterShim.h

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
using chip::app::DataModel::EndpointCompositionPattern;
using chip::app::DataModel::Provider;

class ContactSensorAppDataModelProvider : public CodeDrivenDataModelProvider
{
public:
    ContactSensorAppDataModelProvider(chip::PersistentStorageDelegate & persistentStorageDelegate) :
        CodeDrivenDataModelProvider(persistentStorageDelegate)
    {
        // TODO(sergiosoares): initialize data model here
        // mEndpoint0.AddServerCluster(mEndpoint0Clusters);
        // mEndpoint1.AddServerCluster(mEndpoint1Clusters);
        // AddEndpoint(mEndpoint0);
        // AddEndpoint(mEndpoint1);
    }

private:
    // ServerClusterShim mEndpoint0Clusters{ { 0, 1 }, { 0, 2 } }; // TODO(sergiosoares): fix paths
    // ServerClusterShim mEndpoint1Clusters{ { 1, 3 }, { 1, 4 } }; // TODO(sergiosoares): fix paths
    // SpanEndpoint mEndpoint0;
    // Span mEndpoint1;
};

class LightingAppDataModelProvider : public CodeDrivenDataModelProvider
{
public:
    LightingAppDataModelProvider(chip::PersistentStorageDelegate & persistentStorageDelegate) :
        CodeDrivenDataModelProvider(persistentStorageDelegate)
    {
        // // TODO(sergiosoares): initialize data model here
        // mEndpoint0.AddServerCluster(mEndpoint0Clusters);
        // mEndpoint1.AddServerCluster(mEndpoint1Clusters);
        // AddEndpoint(mEndpoint0);
        // AddEndpoint(mEndpoint1);
    }

private:
    // CodegenServerCluster mEndpoint0Clusters{ { 0, 1 }, { 0, 2 } }; // TODO(sergiosoares): fix paths
    // CodegenServerCluster mEndpoint1Clusters{ { 1, 3 }, { 1, 4 } }; // TODO(sergiosoares): fix paths
    // DynamicEndpointProvider mEndpoint0;
    // DynamicEndpointProvider mEndpoint1;
};

namespace {

chip::PersistentStorageDelegate & GetAppPersistentStorageDelegate()
{
    static chip::KvsPersistentStorageDelegate persistentStorageDelegate;
    static bool initialized = false;
    if (!initialized)
    {
        // Initialize the persistent storage delegate if it hasn't been initialized yet.
        persistentStorageDelegate.Init(&chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr());
        initialized = true;
    }
    return persistentStorageDelegate;
}

} // namespace

CodeDrivenDataModelProvider & GetContactSensorDataModelProvider()
{
    static ContactSensorAppDataModelProvider contactSensorAppDataModelProvider(GetAppPersistentStorageDelegate());
    return contactSensorAppDataModelProvider;
}

CodeDrivenDataModelProvider & GetLightingDataModelProvider()
{
    static LightingAppDataModelProvider lightingAppDataModelProvider(GetAppPersistentStorageDelegate());
    return lightingAppDataModelProvider;
}

CodeDrivenDataModelProvider & GetAppDataModelProvider(AppTypeEnum app)
{
    switch (app)
    {
    case AppTypeEnum::kContactSensorApp:
        return GetContactSensorDataModelProvider();
        break;
    case AppTypeEnum::kLightingApp:
        return GetLightingDataModelProvider();
        break;
    default:
        assert(false && "Unexpected value in GetAppDataModelProvider() switch statement");
        break;
    }
}
