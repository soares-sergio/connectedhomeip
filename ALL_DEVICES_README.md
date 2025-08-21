# Description

Development iteration for a code-driven application. Focus on a bridge
implementation.

## Checking out

We are developing on a shared branch for now. You can use:

```sh
git clone -b all_devices_app git@github.com:soares-sergio/connectedhomeip.git connectedhomeip-all-devices
```

## Initial compilation

One-liner:

```sh
bash -c 'source ./scripts/activate.sh && ./scripts/build/build_examples.py --target linux-x64-all-devices-app-boringssl build'
```

Or alternatively activate and run:

```sh
/scripts/build/build_examples.py --target linux-x64-all-devices-app-boringssl
```

## Run

```sh
./out/linux-x64-all-devices-app-boringssl/all-devices-app
```

Data model capture one-liner:

```sh
rm -f /tmp/chip_* \
  && scripts/run_in_python_env.sh out/python_env './scripts/tests/run_python_test.py --app ./out/linux-x64-all-devices-app-boringssl/all-devices-app --app-args "--trace-to json:log" --script src/python_testing/TC_DeviceBasicComposition.py --script-args "--manual-code 34970112332 --tests test_TC_IDM_12_1"'
```

## Console via PWRPC

The following examples use [Uv](git@github.com:project-chip/connectedhomeip.git)
for a fast pip equivalent. Any venv will also work.

```sh
# create a Venv
uv venv out/console-env

# Source according to your shell
source out/console-env/bin/activate

# install the console wheels
uv pip install out/linux-x64-all-devices-app-boringssl/all_devices_rpc_console_wheels/*.whl

# Run the all devices console (after the app is started)
all-devices-console -s localhost:33000

# Combined:
uv venv out/console-env && source out/console-env/bin/activate && uv pip install out/linux-x64-all-devices-app-boringssl/all_devices_rpc_console_wheels/*.whl && all-devices-console -s localhost:33000

# Try something like:
#  device.rpcs.all_devices.rpc.TestService.GetDeviceInfo()
#  device.rpcs.all_devices.rpc.Bridge.ListDevices()

# For occupancy sensor
#  device.rpcs.all_devices.rpc.Bridge.AddDevice(unique_id="ABC", device_type=2)
#  device.rpcs.all_devices.rpc.Bridge.UpdateDevice(unique_id="ABC", occupied=True)
#  device.rpcs.all_devices.rpc.Bridge.RemoveDevice(unique_id="ABC")
#
#  from bridge_service.bridge_service_pb2 import CONTACT_SENSOR
#  device.rpcs.all_devices.rpc.Bridge.AddDevice(unique_id="ABC", device_type=CONTACT_SENSOR)

```

### Validating tests in Jupyter Lab

Install stuff:

```sh
pip install jupyterlab ipykernel jupyterlab-lsp python-lsp-server pprint pandas

```

Startup blurb:

```
%reset -f
import os
# TODO: fix your own directory if needed
# os.chdir('/some/path/here/connectedhomeip')
import importlib.util
spec = importlib.util.find_spec('matter.ChipReplStartup')
%run {spec.origin}
```

Commission:

```
devices = await devCtrl.DiscoverCommissionableNodes(filterType=matter.discovery.FilterType.LONG_DISCRIMINATOR, filter=3840, stopOnFirst=True, timeoutSecond=2)
await devices[0].Commission(2, 20202021)
```

Subscribe:

```
import pprint
def cb(data, tran):
    if data.Path is not None:
        value = tran.GetAttribute(data)
        pprint.pp("Attribute: %s => %r" % (data.Path, value))
    else:
        pprint.pp("UNKNOWN: %r, %r" % (data, tran))
sub = await devCtrl.ReadAttribute(2, '*', reportInterval=(0,30), autoResubscribe=False)
sub.SetAttributeUpdateCallback(cb)
```
