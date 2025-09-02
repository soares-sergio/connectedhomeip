#include "AppDataModel.h"
#include <devices/OccupancySensorDevice.h>
#include <devices/ContactSensorDevice.h>
#include <devices/BridgedNodeDevice.h>
#include <devices/Device.h>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>
#include <data-model-providers/codedriven/endpoint/SpanEndpoint.h>
#include <platform/KvsPersistentStorageDelegate.h>
#include <memory>


using chip::app::CodeDrivenDataModelProvider;
using chip::app::DataModel::EndpointCompositionPattern;
using chip::app::DataModel::Provider;
using chip::app::Device;

std::unique_ptr<Device> RegisterNewDevice(AppDeviceType deviceType,
                                             chip::app::CodeDrivenDataModelProvider & provider,
                                             chip::EndpointId endpointId,
                                             chip::EndpointId parentEndpointId)
{
    std::unique_ptr<Device> device;
    switch (deviceType)
    {
    case AppDeviceType::kContactSensor:
        device = std::make_unique<chip::app::ContactSensorDevice>("contact_sensor");
        break;
    case AppDeviceType::kOccupancySensor:
        device = std::make_unique<chip::app::OccupancySensorDevice>("occupancy_sensor");
        break;
    // case AppDeviceType::kBridge:
    //     return // TODO: create an AggregatorDevice; call contact sensor device register
    // case AppDeviceType::kBridgedNode:
    //     return // TODO: call Bridged Node device register
    default:
        return nullptr;
    }

    if (device)
    {
        CHIP_ERROR err = device->Register(endpointId, provider, parentEndpointId);
        if (err != CHIP_NO_ERROR)
        {
            return nullptr;
        }
    }

    return device;
}