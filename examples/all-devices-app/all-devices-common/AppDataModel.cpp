#include "AppDataModel.h"
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/endpoint/SpanEndpoint.h>
#include <devices/BridgedNodeDevice.h>
#include <devices/ContactSensorDevice.h>
#include <devices/Device.h>
#include <devices/OccupancySensorDevice.h>
#include <devices/OnOffDevice.h>
#include <memory>
#include <platform/KvsPersistentStorageDelegate.h>

CHIP_ERROR RegisterNewDevice(AppDeviceType deviceType, std::string unique_id, chip::EndpointId parentEndpointId,
                             chip::app::DeviceManager & deviceManager)
{
    switch (deviceType)
    {
    case AppDeviceType::kContactSensor:
        return deviceManager.AddDevice(std::make_unique<chip::app::ContactSensorDevice>(unique_id), parentEndpointId);
    case AppDeviceType::kOccupancySensor:
        return deviceManager.AddDevice(std::make_unique<chip::app::OccupancySensorDevice>(unique_id), parentEndpointId);
    case AppDeviceType::kLight:
        return deviceManager.AddDevice(std::make_unique<chip::app::OnOffDevice>("light", 0x0100, 3), parentEndpointId);
    case AppDeviceType::kPlug:
        return deviceManager.AddDevice(std::make_unique<chip::app::OnOffDevice>("plug", 0x010A, 4), parentEndpointId);
    // case AppDeviceType::kBridge:
    //     return // TODO: create an AggregatorDevice; call contact sensor device register
    // case AppDeviceType::kBridgedNode:
    //     return // TODO: call Bridged Node device register
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
}
