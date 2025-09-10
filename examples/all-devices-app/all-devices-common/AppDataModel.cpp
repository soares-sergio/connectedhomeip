#include "AppDataModel.h"
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/endpoint/SpanEndpoint.h>
#include <devices/AggregatorDevice.h>
#include <devices/BridgedNodeDevice.h>
#include <devices/ContactSensorDevice.h>
#include <devices/Device.h>
#include <devices/OccupancySensorDevice.h>
#include <devices/OnOffDevice.h>
#include <devices/RootNodeDevice.h>
#include <memory>
#include <platform/KvsPersistentStorageDelegate.h>



CHIP_ERROR RegisterNewDevice(DeviceType deviceType, std::string unique_id, chip::EndpointId parentEndpointId,
                             chip::app::DeviceManager & deviceManager, chip::DeviceLayer::NetworkCommissioning::WiFiDriver * wifiDriver)
{
    switch (deviceType)
    {
    case DeviceType::kContactSensor:
        return deviceManager.AddDevice(std::make_unique<chip::app::ContactSensorDevice>(unique_id), parentEndpointId);
    case DeviceType::kOccupancySensor:
        return deviceManager.AddDevice(std::make_unique<chip::app::OccupancySensorDevice>(unique_id), parentEndpointId);
    case DeviceType::kOnOffLight:
        return deviceManager.AddDevice(std::make_unique<chip::app::OnOffDevice>(unique_id, DeviceType::kOnOffLight),
                                       parentEndpointId);
    case DeviceType::kOnOffPlug:
        return deviceManager.AddDevice(std::make_unique<chip::app::OnOffDevice>(unique_id, DeviceType::kOnOffPlug),
                                       parentEndpointId);
    case DeviceType::kAggregator:
        return deviceManager.AddDevice(std::make_unique<chip::app::AggregatorDevice>(unique_id), parentEndpointId);
    case DeviceType::kBridgedNodeDevice:
        return deviceManager.AddDevice(std::make_unique<chip::app::BridgedNodeDevice>(unique_id), parentEndpointId);
    case DeviceType::kRootNode:
        VerifyOrDie(wifiDriver != nullptr);
        return deviceManager.AddDevice(std::make_unique<chip::app::RootNodeDevice>(unique_id, *wifiDriver),
                                       parentEndpointId);
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
}