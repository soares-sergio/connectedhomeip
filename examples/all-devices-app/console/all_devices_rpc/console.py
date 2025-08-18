#
#    Copyright (c) 2021 Project CHIP Authors
#    All rights reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
"""Console for interacting with CHIP lighting app over RPC.

To start the console, provide a serial port as the --device argument

  python -m all_devices_rpc.console --device /dev/ttyUSB0

Alternatively to connect to a linux CHIP device provide the port.

  python -m all_devices_rpc.console -s localhost:33000

This starts an IPython console for communicating with the connected device. A
few variables are predefined in the interactive console. These include:

    rpcs   - used to invoke RPCs
    device - the serial device used for communication
    client - the HDLC rpc client
    protos - protocol buffer messages indexed by proto package

An example RPC command:
    rpcs.chip.rpc.TestService.GetDeviceInfo()
    device.rpcs.chip.rpc.TestService.GetDeviceInfo()
"""
import argparse
import sys
from pathlib import Path
from typing import Collection

import pw_system.console
from pw_hdlc import rpc

# Protos
# isort: off
from test_service import test_service_pb2


def _parse_args():
    """Parses and returns the command line arguments."""
    parser = argparse.ArgumentParser(description=__doc__)
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("-d", "--device", help="the serial port to use")
    parser.add_argument(
        "-b", "--baudrate", type=int, default=115200, help="the baud rate to use"
    )
    parser.add_argument(
        "-r",
        "--raw_serial",
        action="store_true",
        help=("Use raw serial instead of HDLC/RPC"),
    )
    parser.add_argument(
        "--token-databases",
        metavar="elf_or_token_database",
        nargs="+",
        type=Path,
        help="Path to tokenizer database csv file(s).",
    )
    group.add_argument(
        "-s",
        "--socket-addr",
        type=str,
        help="use socket to connect to server, type default for\
            localhost:33000, or manually input the server address:port",
    )
    return parser.parse_args()


def show_console(
    device: str,
    baudrate: int,
    token_databases: Collection[Path],
    socket_addr: str,
    raw_serial: bool,
) -> int:

    # TODO: this shows a default console with little customization
    #       Ideally we should at least customize the default messages
    #
    #
    # For now example of how to run commands:
    #
    #   device.rpcs.chip.rpc.Device.GetDeviceInfo()
    #

    pw_system.console.console(
        device=device,
        baudrate=baudrate,
        socket_addr=socket_addr,
        hdlc_encoding=not raw_serial,
        token_databases=token_databases,
        logfile="",
        device_logfile="",
        channel_id=rpc.DEFAULT_CHANNEL_ID,
        # Defaults beyond the original console
        ticks_per_second=None,
        host_logfile="",
        json_logfile="",
        rpc_logging=False,
        compiled_protos=[
            test_service_pb2,
        ],
    )


def main() -> int:
    return show_console(**vars(_parse_args()))


if __name__ == "__main__":
    sys.exit(main())
