/*
 *
 *    Copyright (c) 2020-2021 Project CHIP Authors
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

#pragma once

#include <app/CASEClientPool.h>
#include <app/OperationalSessionSetup.h>
#include <app/OperationalSessionSetupPool.h>
#include <lib/core/CHIPConfig.h>
#include <lib/core/CHIPCore.h>
#include <lib/support/Pool.h>
#include <platform/CHIPDeviceLayer.h>
#include <transport/SessionDelegate.h>
#include <transport/SessionManager.h>
#include <transport/SessionUpdateDelegate.h>

namespace chip {

struct CASESessionManagerConfig
{
    CASEClientInitParams sessionInitParams;
    CASEClientPoolDelegate * clientPool                    = nullptr;
    OperationalSessionSetupPoolDelegate * sessionSetupPool = nullptr;
};

/**
 * This class provides the following
 * 1. Manage a pool of operational device proxy objects for peer nodes that have active message exchange with the local node.
 * 2. The pool contains atmost one device proxy object for a given peer node.
 * 3. API to lookup an existing proxy object, or allocate a new one by triggering session establishment with the peer node.
 * 4. During session establishment, trigger node ID resolution (if needed), and update the DNS-SD cache (if resolution is
 * successful)
 */
class CASESessionManager : public OperationalSessionReleaseDelegate, public SessionUpdateDelegate
{
public:
    CASESessionManager() = default;
    virtual ~CASESessionManager()
    {
        if (mConfig.sessionInitParams.Validate() == CHIP_NO_ERROR)
        {
            mConfig.sessionInitParams.exchangeMgr->GetReliableMessageMgr()->RegisterSessionUpdateDelegate(nullptr);
        }
    }

    CHIP_ERROR Init(chip::System::Layer * systemLayer, const CASESessionManagerConfig & params);
    void Shutdown();

    /**
     * Find an existing session for the given node ID, or trigger a new session
     * request.
     *
     * The caller can optionally provide `onConnection` and `onFailure` callback
     * objects. If provided, these will be used to inform the caller about
     * successful or failed connection establishment.
     *
     * If the connection is already established, the `onConnection` callback
     * will be immediately called, before FindOrEstablishSession returns.
     *
     * The `onFailure` callback may be called before the FindOrEstablishSession
     * call returns, for error cases that are detected synchronously.
     *
     * attemptCount can be set to a value greater than 1 to automatically make at least
     * attemptCount session establishment attempts until session setup is successful.
     */
    void FindOrEstablishSession(const ScopedNodeId & peerId, Callback::Callback<OnDeviceConnected> * onConnection,
                                Callback::Callback<OnDeviceConnectionFailure> * onFailure,
#if CHIP_DEVICE_CONFIG_ENABLE_AUTOMATIC_CASE_RETRIES
                                uint8_t attemptCount = 1, Callback::Callback<OnDeviceConnectionRetry> * onRetry = nullptr,
#endif // CHIP_DEVICE_CONFIG_ENABLE_AUTOMATIC_CASE_RETRIES
                                TransportPayloadCapability transportPayloadCapability = TransportPayloadCapability::kMRPPayload);

    /**
     * Find an existing session for the given node ID or trigger a new session request.
     *
     * The caller can optionally provide `onConnection` and `onSetupFailure`
     * callback objects. If provided, these will be used to inform the caller about successful or
     * failed connection establishment.
     *
     * If the connection is already established, the `onConnection` callback will be immediately called,
     * before `FindOrEstablishSession` returns.
     *
     * The `onSetupFailure` callback may be called before the `FindOrEstablishSession`
     * call returns, for error cases that are detected synchronously.
     *
     * The `attemptCount` parameter can be set to a value greater than 1 to automatically make
     * at least attemptCount session establishment attempts until session setup is successful.
     *
     * @param peerId The node ID to find or establish a session with.
     * @param onConnection A callback to be called upon successful connection establishment.
     * @param onSetupFailure A callback to be called upon an extended device connection failure.
     * @param attemptCount The number of attempts to make at establishing a session.  If set to a number larger than 1,
     *                     a session setup failure will lead to a retry, with at least attemptCount total attempts.
     * @param onRetry A callback to be called on a retry attempt (enabled by a config flag).
     * @param transportPayloadCapability An indicator of what payload types the session needs to be able to transport.
     */
    void FindOrEstablishSession(const ScopedNodeId & peerId, Callback::Callback<OnDeviceConnected> * onConnection,
                                Callback::Callback<OperationalSessionSetup::OnSetupFailure> * onSetupFailure,
#if CHIP_DEVICE_CONFIG_ENABLE_AUTOMATIC_CASE_RETRIES
                                uint8_t attemptCount = 1, Callback::Callback<OnDeviceConnectionRetry> * onRetry = nullptr,
#endif // CHIP_DEVICE_CONFIG_ENABLE_AUTOMATIC_CASE_RETRIES
                                TransportPayloadCapability transportPayloadCapability = TransportPayloadCapability::kMRPPayload);

    /**
     * Find an existing session for the given node ID or trigger a new session request.
     *
     * The caller can optionally provide `onConnection`
     * callback objects. If provided, these will be used to inform the caller about successful connection establishment.
     *
     * If the connection is already established, the `onConnection` callback will be immediately called,
     * before `FindOrEstablishSession` returns.
     *
     * The `attemptCount` parameter can be set to a value greater than 1 to automatically make
     * at least attemptCount session establishment attempts until session setup is successful.
     *
     * This function allows passing 'nullptr' for the error handler to compile, which is useful in scenarios where error
     * handling is not needed.
     *
     * @param peerId The node ID to find or establish a session with.
     * @param onConnection A callback to be called upon successful connection establishment.
     * @param attemptCount The number of attempts to make at establishing a session.  If set to a number larger than 1,
     *                     a session setup failure will lead to a retry, with at least attemptCount total attempts.
     * @param onRetry A callback to be called on a retry attempt (enabled by a config flag).
     * @param transportPayloadCapability An indicator of what payload types the session needs to be able to transport.
     */
    void FindOrEstablishSession(const ScopedNodeId & peerId, Callback::Callback<OnDeviceConnected> * onConnection, std::nullptr_t,
#if CHIP_DEVICE_CONFIG_ENABLE_AUTOMATIC_CASE_RETRIES
                                uint8_t attemptCount = 1, Callback::Callback<OnDeviceConnectionRetry> * onRetry = nullptr,
#endif // CHIP_DEVICE_CONFIG_ENABLE_AUTOMATIC_CASE_RETRIES
                                TransportPayloadCapability transportPayloadCapability = TransportPayloadCapability::kMRPPayload);

    /**
     * Find an existing session for the given node ID or trigger a new session request.
     *
     * The caller can optionally provide `onConnection`
     * callback objects. If provided, these will be used to inform the caller about successful connection establishment.
     *
     * If the connection is already established, the `onConnection` callback will be immediately called,
     * before `FindOrEstablishSession` returns.
     *
     * The `onFailure` callback may be called before the FindOrEstablishSession
     * call returns, for error cases that are detected synchronously.
     *
     * @note This API uses default values for automatic CASE retries, if enabled.
     *
     * @param peerId The node ID to find or establish a session with.
     * @param onConnection A callback to be called upon successful connection establishment.
     * @param onSetupFailure A callback to be called upon an extended device connection failure.
     * @param transportPayloadCapability An indicator of what payload types the session needs to be able to transport.
     */
    void FindOrEstablishSession(const ScopedNodeId & peerId, Callback::Callback<OnDeviceConnected> * onConnection,
                                Callback::Callback<OnDeviceConnectionFailure> * onFailure,
                                TransportPayloadCapability transportPayloadCapability);

    void ReleaseSession(const ScopedNodeId & peerId);
    void ReleaseSessionsForFabric(FabricIndex fabricIndex);

    void ReleaseAllSessions();

    /**
     * This API returns the address for the given node ID.
     * If the CASESessionManager is configured with a DNS-SD cache, the cache is looked up
     * for the node ID.
     * If the DNS-SD cache is not available, the CASESessionManager looks up the list for
     * an ongoing session with the peer node. If the session doesn't exist, the API will return
     * `CHIP_ERROR_NOT_CONNECTED` error.
     */
    CHIP_ERROR GetPeerAddress(const ScopedNodeId & peerId, Transport::PeerAddress & addr,
                              TransportPayloadCapability transportPayloadCapability = TransportPayloadCapability::kMRPPayload);

    //////////// OperationalSessionReleaseDelegate Implementation ///////////////
    void ReleaseSession(OperationalSessionSetup * device) override;

    //////////// SessionUpdateDelegate Implementation ///////////////
    void UpdatePeerAddress(ScopedNodeId peerId) override;

private:
    OperationalSessionSetup * FindExistingSessionSetup(const ScopedNodeId & peerId, bool forAddressUpdate = false) const;

    Optional<SessionHandle> FindExistingSession(
        const ScopedNodeId & peerId,
        const TransportPayloadCapability transportPayloadCapability = TransportPayloadCapability::kMRPPayload) const;

    void FindOrEstablishSessionHelper(const ScopedNodeId & peerId, Callback::Callback<OnDeviceConnected> * onConnection,
                                      Callback::Callback<OnDeviceConnectionFailure> * onFailure,
                                      Callback::Callback<OperationalSessionSetup::OnSetupFailure> * onSetupFailure,
#if CHIP_DEVICE_CONFIG_ENABLE_AUTOMATIC_CASE_RETRIES
                                      uint8_t attemptCount, Callback::Callback<OnDeviceConnectionRetry> * onRetry,
#endif
                                      TransportPayloadCapability transportPayloadCapability);

    CASESessionManagerConfig mConfig;
};

} // namespace chip
