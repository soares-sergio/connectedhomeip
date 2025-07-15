#include "AppDataModel.h"
#include <cassert>

#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/endpoint/SpanEndpoint.h>
// #include <server-cluster-shim/ServerClusterShim.h>
#include <lib/core/CHIPPersistentStorageDelegate.h>

// #include <app/server/Server.h>
#include <app/persistence/DefaultAttributePersistenceProvider.h>
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

chip::app::AttributePersistenceProvider & GetAppAttributePersistenceProvider()
{
    static chip::app::DefaultAttributePersistenceProvider sAttributePersistenceProvider;
    return sAttributePersistenceProvider;
}

CodeDrivenDataModelProvider dataModelProvider(GetAppPersistentStorageDelegate(), GetAppAttributePersistenceProvider());

} // namespace

CodeDrivenDataModelProvider & GetAppDataModelProvider()
{
    return dataModelProvider;
}

// TODO: Implement a DeviceRegistrationInterface class with 2 pure virtual methods: Span<ServerClusterInterfaceRegistration>
// GetServerClusterRegistrations() and GetEndpointRegistrations()
