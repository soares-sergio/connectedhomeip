/*
 *
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

#include "chef-microwave-oven-mode.h"
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/reporting/reporting.h>
#include <app/util/attribute-storage.h>
#include <app/util/endpoint-config-api.h>
#include <lib/support/logging/CHIPLogging.h>

#ifdef MATTER_DM_PLUGIN_MICROWAVE_OVEN_MODE_SERVER

using namespace chip;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ModeBase;
using chip::Protocols::InteractionModel::Status;
template <typename T>
using List              = chip::app::DataModel::List<T>;
using ModeTagStructType = chip::app::Clusters::detail::Structs::ModeTagStruct::Type;

namespace ChefMicrowaveOvenMode {
constexpr uint32_t kMicrowaveOvenFeatureMap  = 0;
constexpr size_t kMicrowaveOvenModeTableSize = MATTER_DM_MICROWAVE_OVEN_MODE_CLUSTER_SERVER_ENDPOINT_COUNT;
static_assert(kMicrowaveOvenModeTableSize <= kEmberInvalidEndpointIndex, "MicrowaveOvenMode table size error");

std::unique_ptr<MicrowaveOvenMode::ChefDelegate> gDelegateTable[kMicrowaveOvenModeTableSize];
std::unique_ptr<ModeBase::Instance> gInstanceTable[kMicrowaveOvenModeTableSize];

/**
 * Initializes MicrowaveOvenMode cluster for the app (all endpoints).
 */
void InitChefMicrowaveOvenModeCluster()
{
    const uint16_t endpointCount = emberAfEndpointCount();

    for (uint16_t endpointIndex = 0; endpointIndex < endpointCount; endpointIndex++)
    {
        EndpointId endpointId = emberAfEndpointFromIndex(endpointIndex);
        if (endpointId == kInvalidEndpointId)
        {
            continue;
        }

        // Check if endpoint has MicrowaveOvenMode cluster enabled
        uint16_t epIndex = emberAfGetClusterServerEndpointIndex(endpointId, MicrowaveOvenMode::Id,
                                                                MATTER_DM_MICROWAVE_OVEN_MODE_CLUSTER_SERVER_ENDPOINT_COUNT);
        if (epIndex >= kMicrowaveOvenModeTableSize)
            continue;

        gDelegateTable[epIndex] = std::make_unique<MicrowaveOvenMode::ChefDelegate>();
        gDelegateTable[epIndex]->Init();

        gInstanceTable[epIndex] = std::make_unique<ModeBase::Instance>(gDelegateTable[epIndex].get(), endpointId,
                                                                       MicrowaveOvenMode::Id, kMicrowaveOvenFeatureMap);
        gInstanceTable[epIndex]->Init();

        ChipLogProgress(DeviceLayer, "Endpoint %d MicrowaveOvenMode Initialized.", endpointId);
    }
}

ModeBase::Instance * GetInstance(EndpointId endpointId)
{
    uint16_t epIndex = emberAfGetClusterServerEndpointIndex(endpointId, MicrowaveOvenMode::Id,
                                                            MATTER_DM_MICROWAVE_OVEN_MODE_CLUSTER_SERVER_ENDPOINT_COUNT);
    if (epIndex < kMicrowaveOvenModeTableSize)
    {
        return gInstanceTable[epIndex].get();
    }
    return nullptr;
}
} // namespace ChefMicrowaveOvenMode

CHIP_ERROR MicrowaveOvenMode::ChefDelegate::Init()
{
    return CHIP_NO_ERROR;
}

void MicrowaveOvenMode::ChefDelegate::HandleChangeToMode(uint8_t NewMode, ModeBase::Commands::ChangeToModeResponse::Type & response)
{
    EndpointId endpointId = mInstance->GetEndpointId();
    ChipLogDetail(DeviceLayer, "HandleChangeToMode: Endpoint %d", endpointId);
    response.status = to_underlying(ModeBase::StatusCode::kSuccess);
}

CHIP_ERROR MicrowaveOvenMode::ChefDelegate::GetModeLabelByIndex(uint8_t modeIndex, chip::MutableCharSpan & label)
{
    EndpointId endpointId = mInstance->GetEndpointId();
    ChipLogDetail(DeviceLayer, "GetModeLabelByIndex: Endpoint %d", endpointId);
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    return chip::CopyCharSpanToMutableCharSpan(kModeOptions[modeIndex].label, label);
}

CHIP_ERROR MicrowaveOvenMode::ChefDelegate::GetModeValueByIndex(uint8_t modeIndex, uint8_t & value)
{
    EndpointId endpointId = mInstance->GetEndpointId();
    ChipLogDetail(DeviceLayer, "GetModeValueByIndex: Endpoint %d", endpointId);
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    value = kModeOptions[modeIndex].mode;
    return CHIP_NO_ERROR;
}

CHIP_ERROR MicrowaveOvenMode::ChefDelegate::GetModeTagsByIndex(uint8_t modeIndex, List<ModeTagStructType> & tags)
{
    EndpointId endpointId = mInstance->GetEndpointId();
    ChipLogDetail(DeviceLayer, "GetModeTagsByIndex: Endpoint %d", endpointId);
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    if (tags.size() < kModeOptions[modeIndex].modeTags.size())
    {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    std::copy(kModeOptions[modeIndex].modeTags.begin(), kModeOptions[modeIndex].modeTags.end(), tags.begin());
    tags.reduce_size(kModeOptions[modeIndex].modeTags.size());

    return CHIP_NO_ERROR;
}

#endif // MATTER_DM_PLUGIN_MICROWAVE_OVEN_MODE_SERVER
