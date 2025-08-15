/*
<<<<<<<< HEAD:examples/all-devices-app/linux/main.cpp
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    All rights reserved.
|||||||| 08c1a5ea61:src/app/clusters/basic-information/basic-information.h
 *
 *    Copyright (c) 2020 Project CHIP Authors
========
 *    Copyright (c) 2025 Project CHIP Authors
 *    All rights reserved.
>>>>>>>> master:src/app/data-model-provider/ClusterMetadataProvider.h
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

<<<<<<<< HEAD:examples/all-devices-app/linux/main.cpp
#include <AppMain.h>
#include <platform/CHIPDeviceConfig.h>
|||||||| 08c1a5ea61:src/app/clusters/basic-information/basic-information.h
#pragma once
========
#pragma once
#include <type_traits>
>>>>>>>> master:src/app/data-model-provider/ClusterMetadataProvider.h

<<<<<<<< HEAD:examples/all-devices-app/linux/main.cpp
using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
|||||||| 08c1a5ea61:src/app/clusters/basic-information/basic-information.h
#include <app/util/basic-types.h>
========
#include <lib/core/DataModelTypes.h>
>>>>>>>> master:src/app/data-model-provider/ClusterMetadataProvider.h

<<<<<<<< HEAD:examples/all-devices-app/linux/main.cpp
void ApplicationInit() {}

void ApplicationShutdown() {}

int main(int argc, char * argv[])
{
    VerifyOrDie(ChipLinuxAppInit(argc, argv) == 0);

    ChipLogProgress(AppServer, "Hello from all-devices-app!");

    ChipLinuxAppMainLoop();

    return 0;
}
|||||||| 08c1a5ea61:src/app/clusters/basic-information/basic-information.h
namespace chip {
namespace app {
namespace Clusters {
namespace BasicInformation {
/**
 * Check whether LocalConfigDisabled is set (on endpoint 0, which is the only
 * place the Basic Information cluster exists and can have the attribute be
 * set).
 */
bool IsLocalConfigDisabled();
} // namespace BasicInformation
} // namespace Clusters
} // namespace app
} // namespace chip
========
namespace chip {
namespace app {
namespace DataModel {

// This template will be specialized for each cluster type, in generated code.
template <class TypeInfo, ClusterId cluster>
struct ClusterMetadataProvider
{
    static_assert(!std::is_same_v<TypeInfo, TypeInfo>,
                  "Metadata provider for this TypeInfo and Cluster is not implemented, try importing "
                  "<clusters/<cluster>/MetadataProvider.h>");
};

} // namespace DataModel
} // namespace app
} // namespace chip
>>>>>>>> master:src/app/data-model-provider/ClusterMetadataProvider.h
