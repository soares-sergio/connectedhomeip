
#include "AppMain.h"
#include "AppDataModel.h"
#include <cassert>
#include <data-model-providers/codedriven/CodeDrivenDataModelProvider.h>

using chip::app::CodeDrivenDataModelProvider;

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
