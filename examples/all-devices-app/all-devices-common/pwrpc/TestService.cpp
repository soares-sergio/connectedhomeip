#include "TestService.h"

#include <platform/CommissionableDataProvider.h>
#include <platform/ConfigurationManager.h>
#include <platform/DeviceInstanceInfoProvider.h>
#include <setup_payload/OnboardingCodesUtil.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>

namespace chip::rpc {

pw::Status TestService::GetDeviceInfo(const pw_protobuf_Empty & request, chip_rpc_DeviceInfo & response)
{
    uint16_t vendor_id;
    if (DeviceLayer::GetDeviceInstanceInfoProvider()->GetVendorId(vendor_id) == CHIP_NO_ERROR)
    {
        response.vendor_id = static_cast<uint32_t>(vendor_id);
    }
    else
    {
        return pw::Status::Internal();
    }

    uint16_t product_id;
    if (DeviceLayer::GetDeviceInstanceInfoProvider()->GetProductId(product_id) == CHIP_NO_ERROR)
    {
        response.product_id = static_cast<uint32_t>(product_id);
    }
    else
    {
        return pw::Status::Internal();
    }

    uint32_t software_version;
    if (DeviceLayer::ConfigurationMgr().GetSoftwareVersion(software_version) == CHIP_NO_ERROR)
    {
        response.software_version = software_version;
    }
    else
    {
        return pw::Status::Internal();
    }

    if (DeviceLayer::ConfigurationMgr().GetSoftwareVersionString(response.software_version_string,
                                                                 sizeof(response.software_version_string)) != CHIP_NO_ERROR)
    {
        return pw::Status::Internal();
    }

    uint32_t code;
    if (DeviceLayer::GetCommissionableDataProvider()->GetSetupPasscode(code) == CHIP_NO_ERROR)
    {
        response.pairing_info.code = code;
        response.has_pairing_info  = true;
    }

    uint16_t discriminator;
    if (DeviceLayer::GetCommissionableDataProvider()->GetSetupDiscriminator(discriminator) == CHIP_NO_ERROR)
    {
        response.pairing_info.discriminator = static_cast<uint32_t>(discriminator);
        response.has_pairing_info           = true;
    }

    if (DeviceLayer::GetDeviceInstanceInfoProvider()->GetSerialNumber(response.serial_number, sizeof(response.serial_number)) !=
        CHIP_NO_ERROR)
    {
        response.serial_number[0] = '\0'; // optional serial field not set.
    }

    // Create buffer for QR code that can fit max size and null terminator.
    char qrCodeBuffer[chip::QRCodeBasicSetupPayloadGenerator::kMaxQRCodeBase38RepresentationLength + 1];
    chip::MutableCharSpan qrCodeText(qrCodeBuffer);
    if (GetQRCode(qrCodeText, chip::RendezvousInformationFlags(chip::RendezvousInformationFlag::kBLE)) == CHIP_NO_ERROR)
    {
        snprintf(response.pairing_info.qr_code, sizeof(response.pairing_info.qr_code), "%s", qrCodeText.data());
        GetQRCodeUrl(response.pairing_info.qr_code_url, sizeof(response.pairing_info.qr_code_url), qrCodeText);
        response.has_pairing_info = true;
    }

    return pw::OkStatus();
}

} // namespace chip::rpc
