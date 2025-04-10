---
orphan: true
---

# Any Device App

## What is this?

This example application implements a Matter application that can be instantiated as "Any Device Type" in runtime, without requiring a rebuild.
This is different from existing apps and it uses the new `CodeDrivenDataModelProvider`, `EndpointProvider` and `ServerClusterInterface` as mechanisms to achieve its runtime configurability.

## Context

The motivation for this paradigm shift in how to structure a Matter application stems from feedback and observation of current issues in the Matter SDK, including:
- Users report it's hard to build and customize their own application
- Limited runtime configuration and circular dependency between data model and application
- Low maintainability: duplication among multiple example apps, .zap files, etc. makes it hard to keep apps updated
- Slow CI: requiring a rebuild for every example app is the main reason for long CI times
- Low reusability: not enough dependency injection and strict directory structure makes it harder to reuse code
- [Device conformance](https://matter-build-automation.ue.r.appspot.com/conformance_report.html): most example apps do not pass conformance tests. This is a side effect of having hard to maintain/duplicated apps.

### Goals
- Address the issues outlined above
- Serve as a reference for new apps and users

## Challenges
- This example app relies heavily on a bigger effort to decouple the core of Matter data model from ember and global codegenerated constructs.
- A significant part of this effort is well under way, with `CodeDrivenDataModelProvider`, `EndpointProvider` and `ServerClusterInterface` providing the main connection points for an application to be built.
- This also depends on the migration of the existing cluster implementations which is progress and tracked in this [GitHub Project](https://github.com/orgs/project-chip/projects/136_). While this is in progress, we've developed a "wrapper" layer around existing ember-based clusters named `CodegenServerCluster` so we can start building this app.
- This means we still need a `.zap` containing all of the required clusters and device types during this transition period.

## General Directory Structure

In order to achieve runtime configurability while keeping the code reusable, this application needs to abide to certain rules:
- `device_typeX` subdirectories: device type code specific code, shall live in its own subdirectory and can be built on its own. It must have its own unit tests that check basic operations, data model, etc. For example, subdirectories `contact_sensor`, `on_off_light`, etc, only contain code related to those device types.
- `platform/` subdirectory: contains code specific to a single platform/build target. As much as possible, a common method signature must be defined in `AppPlatform.h` and implemented in all platform subdirectories under `platform/`.
- `AppDataModel.h / .cpp` files: contain logic to select and build the application data model. For example, it might include all of the device type headers and decide which one to instantiate based on a command line argument, push of a button or previous NVM value stored.
- `legacy_to_be_removed/`: contains legacy files currently required in the transition phase, while we move from the legacy ember-based monolith build into a modular/decoupled structure. For example, it keeps a `.zap` file with all the ember configuration (to be removed once all the clusters are migrated in https://github.com/orgs/project-chip/projects/136).

<pre>
root/
    README.md
    BUILD.gn
    AppPlatform.h
    AppDataModel.h
    AppDataModel.cpp
    TestAnyDeviceAppViaMocks.cpp
    Main.cpp
    contact_sensor/
        ContactSensor.h
        ContactSensor.cpp
        TestContactSensorViaMocks.cpp
        BUILD.gn
    on_off_light/
        OnOffLight.h
        OnOffLight.cpp
        TestOnOffLightViaMocks.cpp
        BUILD.gn
    device_typeX/
        DeviceTypeX.h
        DeviceTypeX.cpp
        TestDeviceTypeXViaMocks.cpp
        BUILD.gn
    device_typeY/
        DeviceTypeY.h
        DeviceTypeY.cpp
        TestDeviceTypeYViaMocks.cpp
        BUILD.gn
    ...
    platform/
        linux/
            AppPlatformLinux.cpp
        esp32/
            AppPlatformEsp32.cpp
    legacy_to_be_removed/
        any-device-app.zap
</pre>
