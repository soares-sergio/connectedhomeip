#
#    Copyright (c) 2023 Project CHIP Authors
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

# See https://github.com/project-chip/connectedhomeip/blob/master/docs/testing/python.md#defining-the-ci-test-arguments
# for details about the block below.
#
# === BEGIN CI TEST ARGUMENTS ===
# test-runner-runs:
#   run1:
#     app: ${ALL_CLUSTERS_APP}
#     app-args: --discriminator 1234 --KVS kvs1 --trace-to json:${TRACE_APP}.json
#     script-args: >
#       --storage-path admin_storage.json
#       --manual-code 10054912339
#       --PICS src/app/tests/suites/certification/ci-pics-values
#       --trace-to json:${TRACE_TEST_JSON}.json
#       --trace-to perfetto:${TRACE_TEST_PERFETTO}.perfetto
#     factory-reset: true
#     quiet: true
#   run2:
#     app: ${CHIP_LOCK_APP}
#     app-args: --discriminator 1234 --KVS kvs1
#     script-args: --storage-path admin_storage.json --manual-code 10054912339
#     factory-reset: true
#     quiet: true
#   run3:
#     app: ${CHIP_LOCK_APP}
#     app-args: --discriminator 1234 --KVS kvs1
#     script-args: --storage-path admin_storage.json --qr-code MT:-24J0Q1212-10648G00
#     factory-reset: true
#     quiet: true
#   run4:
#     app: ${CHIP_LOCK_APP}
#     app-args: --discriminator 1234 --KVS kvs1
#     script-args: >
#       --storage-path admin_storage.json
#       --discriminator 1234
#       --passcode 20202021
#     factory-reset: true
#     quiet: true
#   run5:
#     app: ${CHIP_LOCK_APP}
#     app-args: --discriminator 1234 --KVS kvs1
#     script-args: >
#       --storage-path admin_storage.json
#       --manual-code 10054912339
#       --commissioning-method on-network
#     factory-reset: true
#     quiet: true
#   run6:
#     app: ${CHIP_LOCK_APP}
#     app-args: --discriminator 1234 --KVS kvs1
#     script-args: >
#       --storage-path admin_storage.json
#       --qr-code MT:-24J0Q1212-10648G00
#       --commissioning-method on-network
#     factory-reset: true
#     quiet: true
#   run7:
#     app: ${CHIP_LOCK_APP}
#     app-args: --discriminator 1234 --KVS kvs1
#     script-args: >
#       --storage-path admin_storage.json
#       --discriminator 1234
#       --passcode 20202021
#       --commissioning-method on-network
#     factory-reset: true
#     quiet: true
#   run8:
#     app: ${CHIP_LOCK_APP}
#     app-args: --discriminator 1234 --KVS kvs1
#     script-args: --storage-path admin_storage.json
#     factory-reset: false
#     quiet: true
#   run9:
#     app: ${ENERGY_MANAGEMENT_APP}
#     app-args: --discriminator 1234 --KVS kvs1 --trace-to json:${TRACE_APP}.json
#     script-args: >
#       --storage-path admin_storage.json
#       --manual-code 10054912339
#       --PICS src/app/tests/suites/certification/ci-pics-values
#       --trace-to json:${TRACE_TEST_JSON}.json
#       --trace-to perfetto:${TRACE_TEST_PERFETTO}.perfetto
#     factory-reset: true
#     quiet: true
#   run10:
#     app: ${LIT_ICD_APP}
#     app-args: --discriminator 1234 --KVS kvs1 --trace-to json:${TRACE_APP}.json
#     script-args: >
#       --storage-path admin_storage.json
#       --manual-code 10054912339
#       --PICS src/app/tests/suites/certification/ci-pics-values
#       --trace-to json:${TRACE_TEST_JSON}.json
#       --trace-to perfetto:${TRACE_TEST_PERFETTO}.perfetto
#     factory-reset: true
#     quiet: true
#   run11:
#     app: ${CHIP_MICROWAVE_OVEN_APP}
#     app-args: --discriminator 1234 --KVS kvs1 --trace-to json:${TRACE_APP}.json
#     script-args: >
#       --storage-path admin_storage.json
#       --manual-code 10054912339
#       --PICS src/app/tests/suites/certification/ci-pics-values
#       --trace-to json:${TRACE_TEST_JSON}.json
#       --trace-to perfetto:${TRACE_TEST_PERFETTO}.perfetto
#     factory-reset: true
#     quiet: true
#   run12:
#     app: ${CHIP_RVC_APP}
#     app-args: --discriminator 1234 --KVS kvs1 --trace-to json:${TRACE_APP}.json
#     script-args: >
#       --storage-path admin_storage.json
#       --manual-code 10054912339
#       --PICS src/app/tests/suites/certification/ci-pics-values
#       --trace-to json:${TRACE_TEST_JSON}.json
#       --trace-to perfetto:${TRACE_TEST_PERFETTO}.perfetto
#     factory-reset: true
#     quiet: true
#   run13:
#     app: ${NETWORK_MANAGEMENT_APP}
#     app-args: --discriminator 1234 --KVS kvs1 --trace-to json:${TRACE_APP}.json
#     script-args: >
#       --storage-path admin_storage.json
#       --manual-code 10054912339
#       --PICS src/app/tests/suites/certification/ci-pics-values
#       --trace-to json:${TRACE_TEST_JSON}.json
#       --trace-to perfetto:${TRACE_TEST_PERFETTO}.perfetto
#     factory-reset: true
#     quiet: true
#   run14:
#     app: ${LIGHTING_APP_NO_UNIQUE_ID}
#     app-args: --discriminator 1234 --KVS kvs1 --trace-to json:${TRACE_APP}.json
#     script-args: >
#       --storage-path admin_storage.json
#       --manual-code 10054912339
#       --PICS src/app/tests/suites/certification/ci-pics-values
#       --trace-to json:${TRACE_TEST_JSON}.json
#       --trace-to perfetto:${TRACE_TEST_PERFETTO}.perfetto
#     factory-reset: true
#     quiet: true
# === END CI TEST ARGUMENTS ===

# Run 1: runs through all tests
# Run 2: tests PASE connection using manual code (12.1 only)
# Run 3: tests PASE connection using QR code (12.1 only)
# Run 4: tests PASE connection using discriminator and passcode (12.1 only)
# Run 5: Tests CASE connection using manual code (12.1 only)
# Run 6: Tests CASE connection using QR code (12.1 only)
# Run 7: Tests CASE connection using manual discriminator and passcode (12.1 only)
# Run 8: Tests reusing storage from run7 (i.e. factory-reset=false)
# Run 9: Tests against energy-management-app
# Run 10: Tests against lit-icd app
# Run 11: Tests against microwave-oven app
# Run 12: Tests against chip-rvc app
# Run 13: Tests against network-management-app
# Run 14: Tests against lighting-app-data-mode-no-unique-id

import logging
from dataclasses import dataclass
from typing import Any, Callable

import chip.clusters as Clusters
import chip.clusters.ClusterObjects
import chip.tlv
from chip import ChipUtility
from chip.clusters.Attribute import ValueDecodeFailure
from chip.clusters.ClusterObjects import ClusterAttributeDescriptor, ClusterObjectFieldDescriptor
from chip.clusters.Types import Nullable
from chip.exceptions import ChipStackError
from chip.interaction_model import InteractionModelError, Status
from chip.testing.basic_composition import BasicCompositionTests
from chip.testing.global_attribute_ids import (AttributeIdType, ClusterIdType, CommandIdType, GlobalAttributeIds, attribute_id_type,
                                               cluster_id_type, command_id_type)
from chip.testing.matter_testing import MatterBaseTest, TestStep, async_test_body, default_matter_test_main
from chip.testing.problem_notices import AttributePathLocation, ClusterPathLocation, CommandPathLocation, UnknownProblemLocation
from chip.testing.taglist_and_topology_test import (create_device_type_list_for_root, create_device_type_lists,
                                                    find_tag_list_problems, find_tree_roots, flat_list_ok,
                                                    get_direct_children_of_root, parts_list_problems, separate_endpoint_types)
from chip.tlv import uint
from TC_DeviceConformance import get_supersets


def get_vendor_id(mei: int) -> int:
    """Get the vendor ID portion (MEI prefix) of an overall MEI."""
    return (mei >> 16) & 0xffff


def check_int_in_range(min_value: int, max_value: int, allow_null: bool = False) -> Callable:
    """Returns a checker for whether `obj` is an int that fits in a range."""
    def int_in_range_checker(obj: Any):
        """Inner checker logic for check_int_in_range

        Checker validates that `obj` must have decoded as an integral value in range [min_value, max_value].

        On failure, a ValueError is raised with a diagnostic message.
        """
        if obj is None and allow_null:
            return

        if not isinstance(obj, int) and not isinstance(obj, chip.tlv.uint):
            raise ValueError(f"Value {str(obj)} is not an integer or uint (decoded type: {type(obj)})")
        int_val = int(obj)
        if (int_val < min_value) or (int_val > max_value):
            raise ValueError(
                f"Value {int_val} (0x{int_val:X}) not in range [{min_value}, {max_value}] ([0x{min_value:X}, 0x{max_value:X}])")

    return int_in_range_checker


def check_list_of_ints_in_range(min_value: int, max_value: int, min_size: int = 0, max_size: int = 65535, allow_null: bool = False) -> Callable:
    """Returns a checker for whether `obj` is a list of ints that fit in a range."""
    def list_of_ints_in_range_checker(obj: Any):
        """Inner checker for check_list_of_ints_in_range.

        Checker validates that `obj` must have decoded as a list of integral values in range [min_value, max_value].
        The length of the list must be between [min_size, max_size].

        On failure, a ValueError is raised with a diagnostic message.
        """
        if obj is None and allow_null:
            return

        if not isinstance(obj, list):
            raise ValueError(f"Value {str(obj)} is not a list, but a list was expected (decoded type: {type(obj)})")

        if len(obj) < min_size or len(obj) > max_size:
            raise ValueError(
                f"Value {str(obj)} is a list of size {len(obj)}, but expected a list with size in range [{min_size}, {max_size}]")

        for val_idx, val in enumerate(obj):
            if not isinstance(val, int) and not isinstance(val, chip.tlv.uint):
                raise ValueError(
                    f"At index {val_idx} in {str(obj)}, value {val} is not an int/uint, but an int/uint was expected (decoded type: {type(val)})")

            int_val = int(val)
            if not ((int_val >= min_value) and (int_val <= max_value)):
                raise ValueError(
                    f"At index {val_idx} in {str(obj)}, value {int_val} (0x{int_val:X}) not in range [{min_value}, {max_value}] ([0x{min_value:X}, 0x{max_value:X}])")

    return list_of_ints_in_range_checker


def check_non_empty_list_of_ints_in_range(min_value: int, max_value: int, max_size: int = 65535, allow_null: bool = False) -> Callable:
    """Returns a checker for whether `obj` is a non-empty list of ints that fit in a range."""
    return check_list_of_ints_in_range(min_value, max_value, min_size=1, max_size=max_size, allow_null=allow_null)


def check_no_duplicates(obj: Any) -> None:
    if not isinstance(obj, list):
        raise ValueError(f"Value {str(obj)} is not a list, but a list was expected (decoded type: {type(obj)})")
    if len(set(obj)) != len(obj):
        raise ValueError(f"Value {str(obj)} contains duplicate values")


class TC_DeviceBasicComposition(MatterBaseTest, BasicCompositionTests):
    @async_test_body
    async def setup_class(self):
        super().setup_class()
        await self.setup_class_helper()
        self.build_spec_xmls()

    # ======= START OF ACTUAL TESTS =======
    def test_TC_SM_1_1(self):
        ROOT_NODE_DEVICE_TYPE = 0x16
        self.print_step(1, "Perform a wildcard read of attributes on all endpoints - already done")
        self.print_step(2, "Verify that endpoint 0 exists")
        if 0 not in self.endpoints:
            self.record_error(self.get_test_name(), location=AttributePathLocation(endpoint_id=0),
                              problem="Did not find Endpoint 0.", spec_location="Endpoint Composition")
            self.fail_current_test()

        self.print_step(3, "Verify that endpoint 0 descriptor cluster includes the root node device type")
        if Clusters.Descriptor not in self.endpoints[0]:
            self.record_error(self.get_test_name(), location=AttributePathLocation(endpoint_id=0),
                              problem="No descriptor cluster on Endpoint 0", spec_location="Root node device type")
            self.fail_current_test()

        listed_device_types = [i.deviceType for i in self.endpoints[0]
                               [Clusters.Descriptor][Clusters.Descriptor.Attributes.DeviceTypeList]]
        if ROOT_NODE_DEVICE_TYPE not in listed_device_types:
            self.record_error(self.get_test_name(), location=AttributePathLocation(endpoint_id=0),
                              problem="Root node device type not listed on endpoint 0", spec_location="Root node device type")
            self.fail_current_test()

        self.print_step(4, "Verify that the root node device type does not appear in any of the non-zero endpoints")
        for endpoint_id, endpoint in self.endpoints.items():
            if endpoint_id == 0:
                continue
            listed_device_types = [i.deviceType for i in endpoint[Clusters.Descriptor]
                                   [Clusters.Descriptor.Attributes.DeviceTypeList]]
            if ROOT_NODE_DEVICE_TYPE in listed_device_types:
                self.record_error(self.get_test_name(), location=AttributePathLocation(endpoint_id=endpoint_id),
                                  problem=f'Root node device type listed on endpoint {endpoint_id}', spec_location="Root node device type")
                self.fail_current_test()

        self.print_step(5, "Verify the existence of all the root node clusters on EP0")
        root = self.endpoints[0]
        required_clusters = [Clusters.BasicInformation, Clusters.AccessControl, Clusters.GroupKeyManagement,
                             Clusters.GeneralCommissioning, Clusters.AdministratorCommissioning, Clusters.OperationalCredentials, Clusters.GeneralDiagnostics]
        for c in required_clusters:
            if c not in root:
                self.record_error(self.get_test_name(), location=AttributePathLocation(endpoint_id=0),
                                  problem=f'Root node does not contain required cluster {c}', spec_location="Root node device type")
                self.fail_current_test()

    def test_TC_DT_1_1(self):
        self.print_step(1, "Perform a wildcard read of attributes on all endpoints - already done")
        self.print_step(2, "Verify that each endpoint includes a descriptor cluster")
        success = True
        for endpoint_id, endpoint in self.endpoints.items():
            has_descriptor = (Clusters.Descriptor in endpoint)
            logging.info(f"Checking descriptor on Endpoint {endpoint_id}: {'found' if has_descriptor else 'not_found'}")
            if not has_descriptor:
                self.record_error(self.get_test_name(), location=AttributePathLocation(endpoint_id=endpoint_id, cluster_id=Clusters.Descriptor.id),
                                  problem=f"Did not find a descriptor on endpoint {endpoint_id}", spec_location="Base Cluster Requirements for Matter")
                success = False

        if not success:
            self.fail_current_test("At least one endpoint was missing the descriptor cluster.")

    async def _read_non_standard_attribute_check_unsupported_read(self, endpoint_id, cluster_id, attribute_id) -> bool:
        @dataclass
        class TempAttribute(ClusterAttributeDescriptor):
            @ChipUtility.classproperty
            def cluster_id(cls) -> int:
                return cluster_id

            @ChipUtility.classproperty
            def attribute_id(cls) -> int:
                return attribute_id

            @ChipUtility.classproperty
            def attribute_type(cls) -> ClusterObjectFieldDescriptor:
                return ClusterObjectFieldDescriptor(Type=uint)

            @ChipUtility.classproperty
            def standard_attribute(cls) -> bool:
                return False

            value: 'uint' = 0

        result = await self.default_controller.Read(nodeid=self.dut_node_id, attributes=[(endpoint_id, TempAttribute)])
        try:
            attr_ret = result.tlvAttributes[endpoint_id][cluster_id][attribute_id]
        except KeyError:
            attr_ret = None

        error_type_ok = attr_ret is not None and isinstance(
            attr_ret, Clusters.Attribute.ValueDecodeFailure) and isinstance(attr_ret.Reason, InteractionModelError)

        got_expected_error = error_type_ok and attr_ret.Reason.status == Status.UnsupportedRead
        return got_expected_error

    @async_test_body
    async def test_TC_IDM_10_1(self):
        self.print_step(1, "Perform a wildcard read of attributes on all endpoints - already done")

        @dataclass
        class RequiredMandatoryAttribute:
            id: int
            name: str
            validators: list[Callable]

        ATTRIBUTES_TO_CHECK = [
            RequiredMandatoryAttribute(id=GlobalAttributeIds.CLUSTER_REVISION_ID, name="ClusterRevision",
                                       validators=[check_int_in_range(1, 0xFFFF)]),
            RequiredMandatoryAttribute(id=GlobalAttributeIds.FEATURE_MAP_ID, name="FeatureMap",
                                       validators=[check_int_in_range(0, 0xFFFF_FFFF)]),
            RequiredMandatoryAttribute(id=GlobalAttributeIds.ATTRIBUTE_LIST_ID, name="AttributeList",
                                       validators=[check_non_empty_list_of_ints_in_range(0, 0xFFFF_FFFF), check_no_duplicates]),
            # TODO: Check for EventList
            # RequiredMandatoryAttribute(id=0xFFFA, name="EventList", validator=check_list_of_ints_in_range(0, 0xFFFF_FFFF)),
            RequiredMandatoryAttribute(id=GlobalAttributeIds.ACCEPTED_COMMAND_LIST_ID, name="AcceptedCommandList",
                                       validators=[check_list_of_ints_in_range(0, 0xFFFF_FFFF), check_no_duplicates]),
            RequiredMandatoryAttribute(id=GlobalAttributeIds.GENERATED_COMMAND_LIST_ID, name="GeneratedCommandList",
                                       validators=[check_list_of_ints_in_range(0, 0xFFFF_FFFF), check_no_duplicates]),
        ]

        self.print_step(2, "Validate all global attributes are present")
        success = True
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            for cluster_id, cluster in endpoint.items():
                for req_attribute in ATTRIBUTES_TO_CHECK:
                    attribute_string = self.cluster_mapper.get_attribute_string(cluster_id, req_attribute.id)

                    has_attribute = (req_attribute.id in cluster)
                    location = AttributePathLocation(endpoint_id, cluster_id, req_attribute.id)
                    logging.debug(
                        f"Checking for mandatory global {attribute_string} on {location.as_cluster_string(self.cluster_mapper)}: {'found' if has_attribute else 'not_found'}")

                    # Check attribute is actually present
                    if not has_attribute:
                        self.record_error(self.get_test_name(), location=location,
                                          problem=f"Did not find mandatory global {attribute_string} on {location.as_cluster_string(self.cluster_mapper)}", spec_location="Global Elements")
                        success = False
                        continue

        self.print_step(3, "Validate the global attributes are in range and do not contain duplicates")
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            for cluster_id, cluster in endpoint.items():
                for req_attribute in ATTRIBUTES_TO_CHECK:
                    # Validate attribute value based on the provided validators.
                    for validator in req_attribute.validators:
                        try:
                            validator(cluster[req_attribute.id])
                        except ValueError as e:
                            location = AttributePathLocation(endpoint_id, cluster_id, req_attribute.id)
                            self.record_error(self.get_test_name(), location=location,
                                              problem=f"Failed validation of value on {location.as_string(self.cluster_mapper)}: {str(e)}", spec_location="Global Elements")
                            success = False
                            continue
                        except KeyError:
                            # A KeyError here means the attribute does not exist. This problem was already recorded in step 2,
                            # but we don't assert until the end of the test, so ignore this and don't re-record the error.
                            continue

        self.print_step(4, "Validate the attribute list exactly matches the set of reported attributes")
        if success:
            for endpoint_id, endpoint in self.endpoints_tlv.items():
                for cluster_id, cluster in endpoint.items():
                    attribute_list = cluster[GlobalAttributeIds.ATTRIBUTE_LIST_ID]
                    for attribute_id in attribute_list:
                        location = AttributePathLocation(endpoint_id, cluster_id, attribute_id)
                        has_attribute = attribute_id in cluster

                        attribute_string = self.cluster_mapper.get_attribute_string(cluster_id, attribute_id)
                        logging.debug(
                            f"Checking presence of claimed supported {attribute_string} on {location.as_cluster_string(self.cluster_mapper)}: {'found' if has_attribute else 'not_found'}")

                        if not has_attribute:
                            # Check if this is a write-only attribute by trying to read it.
                            # If it's present and write-only it should return an UNSUPPORTED_READ error. All other errors are a failure.
                            # Because these can be MEI attributes, we need to build the ClusterAttributeDescriptor manually since it's
                            # not guaranteed to be generated. Since we expect an error back anyway, the type doesn't matter.

                            write_only_attribute = await self._read_non_standard_attribute_check_unsupported_read(
                                endpoint_id=endpoint_id, cluster_id=cluster_id, attribute_id=attribute_id)

                            if not write_only_attribute:
                                self.record_error(self.get_test_name(), location=location,
                                                  problem=f"Did not find {attribute_string} on {location.as_cluster_string(self.cluster_mapper)} when it was claimed in AttributeList ({attribute_list})", spec_location="AttributeList Attribute")
                                success = False
                            continue

                        attribute_value = cluster[attribute_id]
                        if isinstance(attribute_value, ValueDecodeFailure) and cluster_id != Clusters.Objects.UnitTesting.id:
                            self.record_error(self.get_test_name(), location=location,
                                              problem=f"Found a failure to read/decode {attribute_string} on {location.as_cluster_string(self.cluster_mapper)} when it was claimed as supported in AttributeList ({attribute_list}): {str(attribute_value)}", spec_location="AttributeList Attribute")
                            success = False
                            continue
                    for attribute_id in cluster:
                        if attribute_id not in attribute_list:
                            attribute_string = self.cluster_mapper.get_attribute_string(cluster_id, attribute_id)
                            location = AttributePathLocation(endpoint_id, cluster_id, attribute_id)
                            self.record_error(self.get_test_name(), location=location,
                                              problem=f'Found attribute {attribute_string} on {location.as_cluster_string(self.cluster_mapper)} not listed in attribute list', spec_location="AttributeList Attribute")
                            success = False

        self.print_step(
            5, "Validate that the global attributes do not contain any additional values in the standard or scoped range that are not defined by the cluster specification")
        # Validate there are attributes in the global range that are not in the required list
        allowed_globals = [a.id for a in ATTRIBUTES_TO_CHECK]
        # also allow event list because it's not disallowed
        event_list_id = 0xFFFA
        allowed_globals.append(event_list_id)
        global_range_min = 0x0000_F000
        attribute_standard_range_max = 0x000_4FFF
        mei_range_min = 0x0001_0000
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            for cluster_id, cluster in endpoint.items():
                globals = [a for a in cluster[GlobalAttributeIds.ATTRIBUTE_LIST_ID] if a >= global_range_min and a < mei_range_min]
                unexpected_globals = sorted(list(set(globals) - set(allowed_globals)))
                for unexpected in unexpected_globals:
                    location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, attribute_id=unexpected)
                    self.record_error(self.get_test_name(), location=location,
                                      problem=f"Unexpected global attribute {unexpected} in cluster {cluster_id}", spec_location="Global elements")
                    success = False

        # validate that all the returned attributes in the standard clusters contain only known attribute ids
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            for cluster_id, cluster in endpoint.items():
                if cluster_id not in chip.clusters.ClusterObjects.ALL_ATTRIBUTES:
                    # Skip clusters that are not part of the standard generated corpus (e.g. MS clusters)
                    continue
                standard_attributes = [a for a in cluster[GlobalAttributeIds.ATTRIBUTE_LIST_ID]
                                       if a <= attribute_standard_range_max]
                allowed_standard_attributes = chip.clusters.ClusterObjects.ALL_ATTRIBUTES[cluster_id]
                unexpected_standard_attributes = sorted(list(set(standard_attributes) - set(allowed_standard_attributes)))
                for unexpected in unexpected_standard_attributes:
                    location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, attribute_id=unexpected)
                    self.record_error(self.get_test_name(), location=location,
                                      problem=f"Unexpected standard attribute {unexpected} in cluster {cluster_id}", spec_location=f"Cluster {cluster_id}")
                    success = False

        # validate there are no attributes in the range between standard and global
        # This is de-facto already covered in the check above, assuming the spec hasn't defined any values in this range, but we should make sure
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            for cluster_id, cluster in endpoint.items():
                bad_range_values = [a for a in cluster[GlobalAttributeIds.ATTRIBUTE_LIST_ID] if a >
                                    attribute_standard_range_max and a < global_range_min]
                for bad in bad_range_values:
                    location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, attribute_id=bad)
                    self.record_error(self.get_test_name(), location=location,
                                      problem=f"Attribute in undefined range {bad} in cluster {cluster_id}", spec_location=f"Cluster {cluster_id}")
                    success = False

        command_standard_range_max = 0x0000_00FF
        # Command lists only have a scoped range, so we only need to check for known command ids, no global range check
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            for cluster_id, cluster in endpoint.items():
                if cluster_id not in chip.clusters.ClusterObjects.ALL_CLUSTERS:
                    continue
                standard_accepted_commands = [
                    a for a in cluster[GlobalAttributeIds.ACCEPTED_COMMAND_LIST_ID] if a <= command_standard_range_max]
                standard_generated_commands = [
                    a for a in cluster[GlobalAttributeIds.GENERATED_COMMAND_LIST_ID] if a <= command_standard_range_max]
                if cluster_id in chip.clusters.ClusterObjects.ALL_ACCEPTED_COMMANDS:
                    allowed_accepted_commands = [a for a in chip.clusters.ClusterObjects.ALL_ACCEPTED_COMMANDS[cluster_id]]
                else:
                    allowed_accepted_commands = []
                if cluster_id in chip.clusters.ClusterObjects.ALL_GENERATED_COMMANDS:
                    allowed_generated_commands = [a for a in chip.clusters.ClusterObjects.ALL_GENERATED_COMMANDS[cluster_id]]
                else:
                    allowed_generated_commands = []

                # Compare the set of commands in the standard range that the DUT says it accepts vs. the commands we know about.
                unexpected_accepted_commands = sorted(list(set(standard_accepted_commands) - set(allowed_accepted_commands)))
                unexpected_generated_commands = sorted(list(set(standard_generated_commands) - set(allowed_generated_commands)))

                for unexpected in unexpected_accepted_commands:
                    location = CommandPathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, command_id=unexpected)
                    self.record_error(self.get_test_name(
                    ), location=location, problem=f'Unexpected accepted command {unexpected} in cluster {cluster_id} allowed: {allowed_accepted_commands} listed: {standard_accepted_commands}', spec_location=f'Cluster {cluster_id}')
                    success = False

                for unexpected in unexpected_generated_commands:
                    location = CommandPathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, command_id=unexpected)
                    self.record_error(self.get_test_name(
                    ), location=location, problem=f'Unexpected generated command {unexpected} in cluster {cluster_id} allowed: {allowed_generated_commands} listed: {standard_generated_commands}', spec_location=f'Cluster {cluster_id}')
                    success = False

        self.print_step(
            6, "Validate that none of the global attribute IDs contain values with prefixes outside of the allowed standard or MEI prefix range")
        if self.is_pics_sdk_ci_only:
            # test vendor prefixes are allowed in the CI because we use them internally in examples
            bad_prefix_min = 0xFFF5_0000
        else:
            # test vendor prefixes are not allowed in products
            bad_prefix_min = 0xFFF1_0000
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            for cluster_id, cluster in endpoint.items():
                attr_prefixes = [a & 0xFFFF_0000 for a in cluster[GlobalAttributeIds.ATTRIBUTE_LIST_ID]]
                cmd_values = cluster[GlobalAttributeIds.ACCEPTED_COMMAND_LIST_ID] + \
                    cluster[GlobalAttributeIds.GENERATED_COMMAND_LIST_ID]
                cmd_prefixes = [a & 0xFFFF_0000 for a in cmd_values]
                bad_attrs = [a for a in attr_prefixes if a >= bad_prefix_min]
                bad_cmds = [a for a in cmd_prefixes if a >= bad_prefix_min]
                for bad_attrib_id in bad_attrs:
                    location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, attribute_id=bad_attrib_id)
                    vendor_id = get_vendor_id(bad_attrib_id)
                    self.record_error(self.get_test_name(
                    ), location=location, problem=f'Attribute 0x{bad_attrib_id:08x} with bad prefix 0x{vendor_id:04x} in cluster 0x{cluster_id:08x}' + (' (Test Vendor)' if attribute_id_type(bad_attrib_id) == AttributeIdType.kTest else ''), spec_location='Manufacturer Extensible Identifier (MEI)')
                    success = False
                for bad_cmd_id in bad_cmds:
                    location = CommandPathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, command_id=bad_cmd_id)
                    vendor_id = get_vendor_id(bad_cmd_id)
                    self.record_error(self.get_test_name(
                    ), location=location, problem=f'Command 0x{bad_cmd_id:08x} with bad prefix 0x{vendor_id:04x} in cluster 0x{cluster_id:08x}' + (' (Test Vendor)' if command_id_type(bad_cmd_id) == CommandIdType.kTest else ''), spec_location='Manufacturer Extensible Identifier (MEI)')
                    success = False

        self.print_step(7, "Validate that none of the MEI global attribute IDs contain values outside of the allowed suffix range")
        # Validate that any attribute in the manufacturer prefix range is in the standard suffix range.
        suffix_mask = 0x0000_FFFF
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            for cluster_id, cluster in endpoint.items():
                manufacturer_range_values = [a for a in cluster[GlobalAttributeIds.ATTRIBUTE_LIST_ID] if a > mei_range_min]
                for manufacturer_value in manufacturer_range_values:
                    suffix = manufacturer_value & suffix_mask
                    location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id,
                                                     attribute_id=manufacturer_value)
                    if suffix > attribute_standard_range_max and suffix < global_range_min:
                        self.record_error(self.get_test_name(), location=location,
                                          problem=f"Manufacturer attribute in undefined range {manufacturer_value} in cluster {cluster_id}",
                                          spec_location=f"Cluster {cluster_id}")
                        success = False
                    elif suffix >= global_range_min:
                        self.record_error(self.get_test_name(), location=location,
                                          problem=f"Manufacturer attribute in global range {manufacturer_value} in cluster {cluster_id}",
                                          spec_location=f"Cluster {cluster_id}")
                        success = False

        for endpoint_id, endpoint in self.endpoints_tlv.items():
            for cluster_id, cluster in endpoint.items():
                accepted_manufacturer_range_values = [
                    a for a in cluster[GlobalAttributeIds.ACCEPTED_COMMAND_LIST_ID] if a > mei_range_min]
                generated_manufacturer_range_values = [
                    a for a in cluster[GlobalAttributeIds.GENERATED_COMMAND_LIST_ID] if a > mei_range_min]
                all_command_manufacturer_range_values = accepted_manufacturer_range_values + generated_manufacturer_range_values
                for manufacturer_value in all_command_manufacturer_range_values:
                    suffix = manufacturer_value & suffix_mask
                    location = CommandPathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, command_id=manufacturer_value)
                    if suffix > command_standard_range_max:
                        self.record_error(self.get_test_name(
                        ), location=location, problem=f'Manufacturer command in the undefined suffix range {manufacturer_value} in cluster {cluster_id}', spec_location='Manufacturer Extensible Identifier (MEI)')
                        success = False

        self.print_step(8, "Validate that all cluster ID prefixes are in the standard or MEI range")
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            cluster_prefixes = [a & 0xFFFF_0000 for a in endpoint.keys()]
            bad_clusters_ids = [a for a in cluster_prefixes if a >= bad_prefix_min]
            for bad_cluster_id in bad_clusters_ids:
                location = ClusterPathLocation(endpoint_id=endpoint_id, cluster_id=bad_cluster_id)
                vendor_id = get_vendor_id(bad_cluster_id)
                self.record_error(self.get_test_name(), location=location,
                                  problem=f'Cluster 0x{bad_cluster_id:08x} with bad prefix 0x{vendor_id:04x}' + (' (Test Vendor)' if cluster_id_type(bad_cluster_id) == ClusterIdType.kTest else ''), spec_location='Manufacturer Extensible Identifier (MEI)')
                success = False

        self.print_step(9, "Validate that all clusters in the standard range have a known cluster ID")
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            standard_clusters = [a for a in endpoint.keys() if a < mei_range_min]
            unknown_clusters = sorted(list(set(standard_clusters) - set(chip.clusters.ClusterObjects.ALL_CLUSTERS)))
            for bad in unknown_clusters:
                location = ClusterPathLocation(endpoint_id=endpoint_id, cluster_id=bad)
                self.record_error(self.get_test_name(
                ), location=location, problem=f'Unknown cluster ID in the standard range {bad}', spec_location='Manufacturer Extensible Identifier (MEI)')
                success = False

        self.print_step(10, "Validate that all clusters in the MEI range have a suffix in the manufacturer suffix range")
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            mei_clusters = [a for a in endpoint.keys() if a >= mei_range_min]
            bad_clusters = [a for a in mei_clusters if ((a & 0x0000_FFFF) < 0xFC00) or ((a & 0x0000_FFFF) > 0xFFFE)]
            for bad in bad_clusters:
                location = ClusterPathLocation(endpoint_id=endpoint_id, cluster_id=bad)
                self.record_error(self.get_test_name(
                ), location=location, problem=f'MEI cluster with an out of range suffix {bad}', spec_location='Manufacturer Extensible Identifier (MEI)')
                success = False

        self.print_step(11, "Validate that standard cluster FeatureMap attributes contains only known feature flags")
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            for cluster_id, cluster in endpoint.items():
                if cluster_id not in chip.clusters.ClusterObjects.ALL_CLUSTERS:
                    continue
                feature_map = cluster[GlobalAttributeIds.FEATURE_MAP_ID]
                feature_mask = 0
                try:
                    feature_map_enum = chip.clusters.ClusterObjects.ALL_CLUSTERS[cluster_id].Bitmaps.Feature
                    for f in feature_map_enum:
                        feature_mask = feature_mask | f
                except AttributeError:
                    # If there is no feature bitmap, feature mask 0 is correct
                    pass
                feature_map_extras = feature_map & ~feature_mask
                if feature_map_extras != 0:
                    location = ClusterPathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id)
                    self.record_error(self.get_test_name(), location=location,
                                      problem=f'Standard cluster {cluster_id} with unkonwn feature {feature_map_extras:02x}')
                    success = False

        if not success:
            self.fail_current_test(
                "At least one cluster has failed the range and support checks for its listed attributes, commands or features")

        # Wildcard reads of events: event list MUST NOT be empty since at least a boot event should be registered.
        self.print_step(12, "Validate that event wildcard subscription works")

        test_failure = None
        try:
            subscription = await self.default_controller.ReadEvent(nodeid=self.dut_node_id,
                                                                   events=[('*')],
                                                                   fabricFiltered=False,
                                                                   reportInterval=(100, 1000))
            if len(subscription.GetEvents()) == 0:
                test_failure = 'Wildcard event subscription returned no events'
        except ChipStackError as e:  # chipstack-ok: assert_raises not suitable here since error must be inspected before determining test outcome
            # Connection over PASE will fail subscriptions with "Unsupported access"
            # TODO: ideally we should SKIP this test for PASE connections
            _IM_UNSUPPORTED_ACCESS_CODE = 0x500 + Status.UnsupportedAccess
            if e.code != _IM_UNSUPPORTED_ACCESS_CODE:
                test_failure = f"Failed to wildcard subscribe events(*): {e}"

        if test_failure:
            self.record_error(self.get_test_name(), problem=test_failure, location=UnknownProblemLocation())
            self.fail_current_test(test_failure)

    def test_TC_IDM_11_1(self):
        success = True
        for endpoint_id, endpoint in self.endpoints_tlv.items():
            for cluster_id, cluster in endpoint.items():
                for attribute_id, attribute in cluster.items():
                    if cluster_id not in Clusters.ClusterObjects.ALL_ATTRIBUTES or attribute_id not in Clusters.ClusterObjects.ALL_ATTRIBUTES[cluster_id]:
                        continue
                    if Clusters.ClusterObjects.ALL_ATTRIBUTES[cluster_id][attribute_id].attribute_type.Type is not str:
                        continue
                    try:
                        cluster[attribute_id].encode('utf-8', errors='strict')
                    except UnicodeError:
                        location = AttributePathLocation(endpoint_id, cluster_id, attribute_id)
                        attribute_string = self.cluster_mapper.get_attribute_string(cluster_id, attribute_id)
                        self.record_error(self.get_test_name(
                        ), location=location, problem=f'Attribute {attribute_string} on {location.as_cluster_string(self.cluster_mapper)} is invalid UTF-8', spec_location="Data types - Character String")
                        success = False
        if not success:
            self.fail_current_test("At least one attribute string was not valid UTF-8")

    # def test_all_event_strings_valid(self):
    #     asserts.skip("TODO: Validate every string in the read events is valid UTF-8 and has no nulls")

    # def test_all_schema_scalars(self):
    #     asserts.skip("TODO: Validate all int/uint are in range of the schema (or null if nullable) for known attributes")

    # def test_all_commands_reported_are_executable(self):
    #     asserts.skip("TODO: Validate all commands reported in AcceptedCommandList are actually executable")

    # def test_dump_all_pics_for_all_endpoints(self):
    #     asserts.skip("TODO: Make a test that generates the basic PICS list for each endpoint based on actually reported contents")

    # def test_all_schema_mandatory_elements_present(self):
    #     asserts.skip(
    #         "TODO: Make a test that ensures every known cluster has the mandatory elements present (commands, attributes) based on features")

    # def test_all_endpoints_have_valid_composition(self):
    #     asserts.skip(
    #         "TODO: Make a test that verifies each endpoint has valid set of device types, and that the device type conformance is respected for each")

    def test_TC_SM_1_2(self):
        self.print_step(1, "Wildcard read of device - already done")

        self.print_step(2, "Verify the Descriptor cluster PartsList on endpoint 0 exactly lists all the other (non-0) endpoints on the DUT")
        parts_list_0 = self.endpoints[0][Clusters.Descriptor][Clusters.Descriptor.Attributes.PartsList]
        cluster_id = Clusters.Descriptor.id
        attribute_id = Clusters.Descriptor.Attributes.PartsList.attribute_id
        location = AttributePathLocation(endpoint_id=0, cluster_id=cluster_id, attribute_id=attribute_id)
        if len(self.endpoints.keys()) != len(set(self.endpoints.keys())):
            self.record_error(self.get_test_name(), location=location,
                              problem='duplicate endpoint ids found in the returned data', spec_location="PartsList Attribute")
            self.fail_current_test()

        if len(parts_list_0) != len(set(parts_list_0)):
            self.record_error(self.get_test_name(), location=location,
                              problem='Duplicate endpoint ids found in the parts list on ep0', spec_location="PartsList Attribute")
            self.fail_current_test()

        expected_parts = set(self.endpoints.keys())
        expected_parts.remove(0)
        if set(parts_list_0) != expected_parts:
            self.record_error(self.get_test_name(), location=location,
                              problem='EP0 Descriptor parts list does not match the set of returned endpoints', spec_location="PartsList Attribute")
            self.fail_current_test()

        self.print_step(
            3, "For each endpoint on the DUT (including EP 0), verify the PartsList in the Descriptor cluster on that endpoint does not include itself")
        for endpoint_id, endpoint in self.endpoints.items():
            if endpoint_id in endpoint[Clusters.Descriptor][Clusters.Descriptor.Attributes.PartsList]:
                location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, attribute_id=attribute_id)
                self.record_error(self.get_test_name(), location=location,
                                  problem=f"Endpoint {endpoint_id} parts list includes itself", spec_location="PartsList Attribute")
                self.fail_current_test()

        self.print_step(4, "Separate endpoints into flat and tree style")
        flat, tree = separate_endpoint_types(self.endpoints)

        self.print_step(5, "Check for cycles in the tree endpoints")
        part_list_errors = parts_list_problems(tree, self.endpoints)
        if len(part_list_errors) != 0:
            for id in part_list_errors:
                location = AttributePathLocation(endpoint_id=id, cluster_id=cluster_id, attribute_id=attribute_id)
                self.record_error(self.get_test_name(), location=location,
                                  problem=f"Endpoint {id} parts list includes a cycle or endpoint with multiple paths to the root or non-existent endpoint", spec_location="PartsList Attribute")
            self.fail_current_test()

        self.print_step(6, "Check flat lists include all sub ids")
        ok = True
        for endpoint_id in flat:
            # ensure that every sub-id in the parts list is included in the parent
            if not flat_list_ok(endpoint_id, self.endpoints):
                location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, attribute_id=attribute_id)
                self.record_error(self.get_test_name(), location=location,
                                  problem='Flat parts list does not exactly match sub-parts', spec_location='Endpoint composition')
                ok = False
        if not ok:
            self.fail_current_test()

    def test_TC_PS_3_1(self):
        BRIDGED_NODE_DEVICE_TYPE_ID = 0x13
        success = True
        self.print_step(1, "Wildcard read of device - already done")

        self.print_step(2, "Verify that all endpoints listed in the EndpointList are valid")
        attribute_id = Clusters.PowerSource.Attributes.EndpointList.attribute_id
        cluster_id = Clusters.PowerSource.id
        attribute_string = self.cluster_mapper.get_attribute_string(cluster_id, attribute_id)
        for endpoint_id, endpoint in self.endpoints.items():
            if Clusters.PowerSource not in endpoint:
                continue
            location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, attribute_id=attribute_id)
            cluster_revision = Clusters.PowerSource.Attributes.ClusterRevision
            if cluster_revision not in endpoint[Clusters.PowerSource]:
                location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id,
                                                 attribute_id=cluster_revision.attribute_id)
                self.record_error(self.get_test_name(
                ), location=location, problem=f'Did not find Cluster revision on {location.as_cluster_string(self.cluster_mapper)}', spec_location='Global attributes')
            if endpoint[Clusters.PowerSource][cluster_revision] < 2:
                location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id,
                                                 attribute_id=cluster_revision.attribute_id)
                self.record_note(self.get_test_name(), location=location,
                                 problem='Power source ClusterRevision is < 2, skipping remainder of test for this endpoint')
                continue
            if Clusters.PowerSource.Attributes.EndpointList not in endpoint[Clusters.PowerSource]:
                self.record_error(self.get_test_name(), location=location,
                                  problem=f'Did not find {attribute_string} on {location.as_cluster_string(self.cluster_mapper)}', spec_location="EndpointList Attribute")
                success = False
                continue

            endpoint_list = endpoint[Clusters.PowerSource][Clusters.PowerSource.Attributes.EndpointList]
            non_existent = set(endpoint_list) - set(self.endpoints.keys())
            if non_existent:
                location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, attribute_id=attribute_id)
                self.record_error(self.get_test_name(), location=location,
                                  problem=f'{attribute_string} lists a non-existent endpoint', spec_location="EndpointList Attribute")
                success = False

        self.print_step(3, "Verify that all Bridged Node endpoint lists are correct")
        device_types = {}
        parts_list = {}
        for endpoint_id, endpoint in self.endpoints.items():
            if Clusters.PowerSource not in endpoint or Clusters.PowerSource.Attributes.EndpointList not in endpoint[Clusters.PowerSource]:
                continue

            def GetPartValidityProblem(endpoint):
                if Clusters.Descriptor not in endpoint:
                    return "Missing cluster descriptor"
                if Clusters.Descriptor.Attributes.PartsList not in endpoint[Clusters.Descriptor]:
                    return "Missing PartList in descriptor cluster"
                if Clusters.Descriptor.Attributes.DeviceTypeList not in endpoint[Clusters.Descriptor]:
                    return "Missing DeviceTypeList in descriptor cluster"
                return None

            problem = GetPartValidityProblem(endpoint)
            if problem:
                location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=Clusters.Descriptor.id,
                                                 attribute_id=Clusters.Descriptor.Attributes.PartsList.id)
                self.record_error(self.get_test_name(), location=location,
                                  problem=problem, spec_location="PartsList Attribute")
                success = False
                continue

            device_types[endpoint_id] = [i.deviceType for i in endpoint[Clusters.Descriptor]
                                         [Clusters.Descriptor.Attributes.DeviceTypeList]]
            parts_list[endpoint_id] = endpoint[Clusters.Descriptor][Clusters.Descriptor.Attributes.PartsList]

        bridged_nodes = [id for (id, dev_type) in device_types.items() if BRIDGED_NODE_DEVICE_TYPE_ID in dev_type]

        for endpoint_id in bridged_nodes:
            if Clusters.PowerSource not in self.endpoints[endpoint_id]:
                continue
            # using a list because we do want to preserve duplicates and error on those.
            desired_endpoint_list = parts_list[endpoint_id].copy()
            desired_endpoint_list.append(endpoint_id)
            desired_endpoint_list.sort()
            ep_list = self.endpoints[endpoint_id][Clusters.PowerSource][Clusters.PowerSource.Attributes.EndpointList]
            ep_list.sort()
            if ep_list != desired_endpoint_list:
                location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, attribute_id=attribute_id)
                self.record_error(self.get_test_name(), location=location,
                                  problem=f'Power source EndpointList on bridged node endpoint {endpoint_id} is not as expected. Desired: {desired_endpoint_list} Actual: {ep_list}', spec_location="EndpointList Attribute")
                success = False

        self.print_step(4, "Verify that all Bridged Node children endpoint lists are correct")
        children = []
        # note, this doesn't handle the full tree structure, single layer only
        for endpoint_id in bridged_nodes:
            children = children + parts_list[endpoint_id]

        for endpoint_id in children:
            if Clusters.PowerSource not in self.endpoints[endpoint_id]:
                continue
            desired_endpoint_list = [endpoint_id]
            ep_list = self.endpoints[endpoint_id][Clusters.PowerSource][Clusters.PowerSource.Attributes.EndpointList]
            ep_list.sort()
            if ep_list != desired_endpoint_list:
                location = AttributePathLocation(endpoint_id=endpoint_id, cluster_id=cluster_id, attribute_id=attribute_id)
                self.record_error(self.get_test_name(), location=location,
                                  problem=f'Power source EndpointList on bridged child endpoint {endpoint_id} is not as expected. Desired: {desired_endpoint_list} Actual: {ep_list}', spec_location="EndpointList Attribute")
                success = False

        if not success:
            self.fail_current_test("power source EndpointList attribute is incorrect")

    def test_TC_DESC_2_2(self):
        self.print_step(0, "Wildcard read of device - already done")

        self.print_step(
            1, "Identify all endpoints that are roots of a tree-composition. Omit any endpoints that include the Content App device type.")
        _, tree = separate_endpoint_types(self.endpoints)
        roots = find_tree_roots(tree, self.endpoints)

        self.print_step(
            1.1, "For each tree root, go through each of the children and add their endpoint IDs to a list of device types based on the DeviceTypes list")
        device_types = create_device_type_lists(roots, self.endpoints)

        self.print_step(
            1.2, "For device types with more than one endpoint listed, ensure each of the listed endpoints has a tag attribute and the tag attributes are not the same")
        problems = find_tag_list_problems(roots, device_types, self.endpoints)

        def record_problems(problems):
            for ep, problem in problems.items():
                location = AttributePathLocation(endpoint_id=ep, cluster_id=Clusters.Descriptor.id,
                                                 attribute_id=Clusters.Descriptor.Attributes.TagList.attribute_id)
                msg = f'problem on ep {ep}: missing feature = {problem.missing_feature}, missing attribute = {problem.missing_attribute}, duplicates = {problem.duplicates}, same_tags = {problem.same_tag}'
                self.record_error(self.get_test_name(), location=location, problem=msg, spec_location="Descriptor TagList")

        record_problems(problems)

        self.print_step(2, "Identify all the direct children of the root node endpoint")
        root_direct_children = get_direct_children_of_root(self.endpoints)
        self.print_step(
            2.1, "Go through each of the direct children of the root node and add their endpoint IDs to a list of device types based on the DeviceTypes list")
        device_types = create_device_type_list_for_root(root_direct_children, self.endpoints)
        self.print_step(
            2.2, "For device types with more than one endpoint listed, ensure each of the listed endpoints has a tag attribute and the tag attributes are not the same")
        root_problems = find_tag_list_problems([0], {0: device_types}, self.endpoints)
        record_problems(root_problems)

        if problems or root_problems:
            self.fail_current_test("Problems with tags lists")

    def steps_TC_IDM_12_1(self):
        return [TestStep(0, "TH performs a wildcard read of all attributes and endpoints on the device"),
                TestStep(1, "TH creates a MatterTlvJson dump of the wildcard attributes for submission to certification.")]

    def test_TC_IDM_12_1(self):
        # wildcard read - already done.
        self.step(0)

        # Create the dump
        self.step(1)
        pid = self.endpoints[0][Clusters.BasicInformation][Clusters.BasicInformation.Attributes.ProductID]
        vid = self.endpoints[0][Clusters.BasicInformation][Clusters.BasicInformation.Attributes.VendorID]
        software_version = self.endpoints[0][Clusters.BasicInformation][Clusters.BasicInformation.Attributes.SoftwareVersion]
        filename = f'device_dump_0x{vid:04X}_0x{pid:04X}_{software_version}.json'
        dump_device_composition_path = self.user_params.get("dump_device_composition_path", filename)
        json_str, txt_str = self.dump_wildcard(dump_device_composition_path)

        # Structured dump so we can pull these back out of the logs
        def log_structured_data(start_tag: str, dump_string):
            lines = dump_string.splitlines()
            logging.info(f'{start_tag}BEGIN ({len(lines)} lines)====')
            for line in lines:
                logging.info(f'{start_tag}{line}')
            logging.info(f'{start_tag}END ====')

        log_structured_data('==== json: ', json_str)
        log_structured_data('==== txt: ', txt_str)

    @async_test_body
    async def test_TC_DESC_2_1(self):

        EP_RANGE_MIN = 1
        EP_RANGE_MAX = 65534

        TAG_LIST_EP_RANGE_MIN = 1
        TAG_LIST_EP_RANGE_MAX = 7

        BRIDGED_NODE_DEVICE_TYPE = 0x0013
        DEVICE_ENERGY_MANAGEMENT_DEVICE_TYPE = 0x050D
        ELECTRICAL_SENSOR_DEVICE_TYPE = 0x0510
        JOINT_FABRIC_ADMINSTRATOR_DEVICE_TYPE = 0x0130
        OTA_PROVIDER_DEVICE_TYPE = 0x0014
        OTA_REQUESTOR_DEVICE_TYPE = 0x0012
        POWER_SOURCE_DEVICE_TYPE = 0x0011
        ROOT_NODE_DEVICE_TYPE = 0x0016
        SECONDARY_NETWORK_INTERFACE_DEVICE_TYPE = 0x0019

        COMMON_AREA_NAMESPACE_ID = 0x10
        COMMON_CLOSURE_NAMESPACE_NAMESPACE_ID = 0x01
        COMMON_COMPASS_DIRECTION_NAMESPACE_ID = 0x02
        COMMON_COMPASS_LOCATION_NAMESPACE_ID = 0x03
        COMMON_DIRECTION_NAMESPACE_ID = 0x04
        COMMON_LANDMARK_NAMESPACE_ID = 0x11
        COMMON_LEVEL_NAMESPACE_ID = 0x05
        COMMON_LOCATION_NAMESPACE_ID = 0x06
        COMMON_NUMBERNAME_SPACE_ID = 0x07
        COMMON_POSITION_NAMESPACE_ID = 0x08
        COMMON_RELATIVE_POSITION_ID = 0x12
        ELECTRICAL_MEASUREMENT_NAMESPACE_ID = 0x0A
        LAUNDRY_NAMESPACE_ID = 0x0E
        POWER_SOURCE_NAMESPACE_ID = 0x0F
        REFRIGERATOR_NAMESPACE_ID = 0x41
        ROOM_AIRCONDITIONER_NAMESPACE_ID = 0x42
        SWITCHES_NAMESPACE_ID = 0x43

        END_POINT_UNIQUE_ID_LENGTH_BYTES = 32

        unique_ids = []
        non_application_device_types = [ROOT_NODE_DEVICE_TYPE, POWER_SOURCE_DEVICE_TYPE,
                                        OTA_REQUESTOR_DEVICE_TYPE, OTA_PROVIDER_DEVICE_TYPE,
                                        BRIDGED_NODE_DEVICE_TYPE, ELECTRICAL_SENSOR_DEVICE_TYPE,
                                        DEVICE_ENERGY_MANAGEMENT_DEVICE_TYPE, SECONDARY_NETWORK_INTERFACE_DEVICE_TYPE,
                                        JOINT_FABRIC_ADMINSTRATOR_DEVICE_TYPE]

        self.print_step("1a", "TH reads DeviceTypeList and PartsList attributes from DUT for Endpoint 0")
        parts_list_ep_0 = self.endpoints[0][Clusters.Descriptor][Clusters.Descriptor.Attributes.PartsList]
        listed_device_types_ep_0 = [i.deviceType for i in self.endpoints[0]
                                    [Clusters.Descriptor][Clusters.Descriptor.Attributes.DeviceTypeList]]
        device_revision_ep_0 = [i.revision for i in self.endpoints[0]
                                [Clusters.Descriptor][Clusters.Descriptor.Attributes.DeviceTypeList]]

        self.print_step("1a.1", "Verify that PartsList attribute is not empty and contains a list of all endpoints.")
        if not parts_list_ep_0:
            self.fail_current_test("PartsList attribute is empty")

        self.print_step("1a.2", "Verify that the DeviceTypeList count is at least one.")
        if not listed_device_types_ep_0:
            self.fail_current_test("DeviceTypeList count is not at least 1")

        self.print_step("1a.3", "Verify that the DeviceTypeList contains one Root Node Device Type.")
        if listed_device_types_ep_0.count(ROOT_NODE_DEVICE_TYPE) > 1:
            self.fail_current_test("There are more than 1 ROOT_NODE_DEVICE_TYPE")

        self.print_step("1a.3", "Verify that the DeviceTypeList may only contain other Node Device Types (device types with scope=node, it can be any of the following Power Source, OTA Requestor, OTA Provider) next to the Root Node Device Type.")
        if listed_device_types_ep_0:
            allowed_device_types = {ROOT_NODE_DEVICE_TYPE,
                                    POWER_SOURCE_DEVICE_TYPE,
                                    OTA_REQUESTOR_DEVICE_TYPE,
                                    OTA_PROVIDER_DEVICE_TYPE}
            if not set(listed_device_types_ep_0).issubset(allowed_device_types):
                self.fail_current_test(
                    "Device types not in ROOT_NODE_DEVICE_TYPE, POWER_SOURCE_DEVICE_TYPE, OTA_REQUESTOR_DEVICE_TYPE, OTA_PROVIDER_DEVICE_TYPE")

        # TODO : Step 1b.4.1, 1b.4.2 needs to the run manually as PIXIT.DESC.DeviceTypeConformanceList
        #       should be selected and based on DUT type.
        for device_revision in device_revision_ep_0:
            if not device_revision:
                self.fail_current_test("Revision is less than 1")

        self.print_step(3, "TH reads PartsList attribute- covered in step 2")
        for endpoint_id, endpoint in self.endpoints.items():
            self.print_step(8, "TH reads ServerList attribute for endpoint :{endpoint_id}".format(endpoint_id=endpoint_id))
            if endpoint_id != 0:

                self.print_step("1b", "TH reads DeviceTypeList and PartsList attributes from DUT for Endpoint {endpoint_id} supported by DUT (except Endpoint 0).".format(
                    endpoint_id=endpoint_id))
                parts_list_per_ep_non_0 = endpoint[Clusters.Descriptor][Clusters.Descriptor.Attributes.PartsList]

                listed_device_types_ep_non_0 = [i.deviceType for i in endpoint
                                                [Clusters.Descriptor][Clusters.Descriptor.Attributes.DeviceTypeList]]
                self.print_step("1b.1", "Verify that the DeviceTypeList count is at least one for end point {endpoint_id}".format(
                    endpoint_id=endpoint_id))
                if not listed_device_types_ep_non_0:
                    self.fail_current_test("DeviceTypeList count is not at least 1")

                self.print_step("1b.2", "the DeviceTypeList contains more than one Application Device Type, verify that all the Application Device Types are part of the same superset for end point {endpoint_id}".format(
                    endpoint_id=endpoint_id))

                supersets = get_supersets(self.xml_device_types)
                for item in non_application_device_types:
                    if item in listed_device_types_ep_non_0:
                        listed_device_types_ep_non_0.remove(item)

                device_type_is_part_of_superset = False
                if listed_device_types_ep_non_0:
                    if len(listed_device_types_ep_non_0) > 1:
                        for eachset in supersets:
                            if set(listed_device_types_ep_non_0).issubset(eachset):
                                device_type_is_part_of_superset = True
                                break
                    else:
                        device_type_is_part_of_superset = True

                    if not device_type_is_part_of_superset:
                        self.fail_current_test("Device type list is more than 1 and it is not matching any superset")

                self.print_step("1b.3", "Verify the DeviceTypeList does not contain the Root Node Device Type {endpoint_id}".format(
                    endpoint_id=endpoint_id))
                if ROOT_NODE_DEVICE_TYPE in listed_device_types_ep_non_0:
                    self.record_error(self.get_test_name(), location=AttributePathLocation(endpoint_id=0),
                                      problem="Root node device type is listed on non zero endpoints",
                                      spec_location="Root node device type")
                    self.fail_current_test("Root node device type is listed on non zero endpoints")

                # TODO : Step 1b.4.1, 1b.4.2 needs to be run manually as PIXIT.DESC.DeviceTypeConformanceList
                #       is not availbale as of now. An issue is created : https://github.com/project-chip/connectedhomeip/issues/38640
                self.print_step("1b.4.1", "Step 1b.4.1 is skipped, test is manually")
                self.print_step("1b.4.2", "Step 1b.4.2 is skipped, test is manually")

                self.print_step("1b.4.3", "Revision should not be less than 1")

                device_revision_ep_non_0 = [i.revision for i in endpoint[Clusters.Descriptor]
                                            [Clusters.Descriptor.Attributes.DeviceTypeList]]
                for device_revision in device_revision_ep_non_0:
                    if not device_revision:
                        self.fail_current_test("Revision is less than 1")

                # TODO : Step 2 and 3 needs to be run manually as PIXIT.DESC.DeviceTypeConformanceList
                #       is not availbale as of now. An issue is created : https://github.com/project-chip/connectedhomeip/issues/38640
                self.print_step("2", "Step 2 is skipped, test is manually")
                self.print_step("3", "Step 3 is skipped, test is manually")

                self.print_step("4", "TH reads PartsList attribute for each endpoint")

                if parts_list_per_ep_non_0:
                    for ep in parts_list_per_ep_non_0:
                        self.print_step("4.1", "Endpoint is in the range of 1 to 65534 for endpoint {endpoint_id}".format(
                            endpoint_id=endpoint_id))
                        if ep not in range(EP_RANGE_MIN, EP_RANGE_MAX):
                            self.fail_current_test("Endpoint is not in the range of 1 to 65534")

                        self.print_step(
                            "4.2", "Endpoint is not equal to the Endpoint of the Endpoint where this PartsList was read (i.e. no self-reference) for {endpoint_id}".format(endpoint_id=endpoint_id))
                        if ep == endpoint_id:
                            self.fail_current_test("Endpoint is self referencing")
        for endpoint_id, endpoint in self.endpoints.items():
            if Clusters.Descriptor.Attributes.TagList not in endpoint[Clusters.Descriptor]:
                continue

            self.print_step(5, "TH reads TagList attribute for endpoint {endpoint_id}.".format(endpoint_id=endpoint_id))

            taglist = endpoint[Clusters.Descriptor][Clusters.Descriptor.Attributes.TagList]

            self.print_step(5.1, "verifying tagList is in the range of 1 to 6 for endpoint: {endpoint_id}".format(
                endpoint_id=endpoint_id))
            if taglist:
                if len(taglist) not in range(TAG_LIST_EP_RANGE_MIN, TAG_LIST_EP_RANGE_MAX):
                    self.fail_current_test("Number of tagList is not in the range of 1 to 6")

            no_duplicate_tag = []
            for tag_struct in taglist:
                self.print_step(5.1, "verifying atleast 1 NamespaceID and tag property in the taglist struct for endpoint: {endpoint_id}".format(
                    endpoint_id=endpoint_id))
                if not tag_struct.namespaceID:
                    self.fail_current_test("Atleast 1 NamespaceID is not present")
                self.print_step(5.1, "verifying Tag property in the taglist struct for endpoint: {endpoint_id}".format(
                    endpoint_id=endpoint_id))
                if not isinstance(tag_struct.tag, int):
                    self.fail_current_test("Atleast 1 Tag is not present")

                if tag_struct.tag in no_duplicate_tag:
                    self.record_error(self.get_test_name(), location=AttributePathLocation(endpoint_id=endpoint_id),
                                      problem="duplicate Tags found in taglist struct", spec_location="TagList")
                    self.fail_current_test("Duplicate tag found")
                no_duplicate_tag.append(tag_struct.tag)

                self.print_step(5.2, "verifying namespaceID value falls under defined namespaces for endpoint: {endpoint_id}".format(
                    endpoint_id=endpoint_id))

                if isinstance(tag_struct.mfgCode, Nullable):
                    if tag_struct.namespaceID not in [COMMON_CLOSURE_NAMESPACE_NAMESPACE_ID,
                                                      COMMON_COMPASS_DIRECTION_NAMESPACE_ID,
                                                      COMMON_COMPASS_LOCATION_NAMESPACE_ID,
                                                      COMMON_DIRECTION_NAMESPACE_ID,
                                                      COMMON_LEVEL_NAMESPACE_ID,
                                                      COMMON_LOCATION_NAMESPACE_ID,
                                                      COMMON_NUMBERNAME_SPACE_ID,
                                                      COMMON_POSITION_NAMESPACE_ID,
                                                      ELECTRICAL_MEASUREMENT_NAMESPACE_ID,
                                                      LAUNDRY_NAMESPACE_ID,
                                                      POWER_SOURCE_NAMESPACE_ID,
                                                      COMMON_AREA_NAMESPACE_ID,
                                                      COMMON_LANDMARK_NAMESPACE_ID,
                                                      COMMON_RELATIVE_POSITION_ID,
                                                      REFRIGERATOR_NAMESPACE_ID,
                                                      ROOM_AIRCONDITIONER_NAMESPACE_ID,
                                                      SWITCHES_NAMESPACE_ID]:
                        self.fail_current_test("Non manufacturer specific tag is not a tag from namespace defined in spec")
                else:

                    self.print_step(5.5, "verifying label field is not null in the tag list construct for end point {endpoint_id}".format(
                        endpoint_id=endpoint_id))
                    if tag_struct.label is None:
                        self.fail_current_test("The Label field is null when the MfgCode is not null.")

        self.print_step(7, "Validate EndpointUniqueId attribute in Descriptor cluster")
        parts_list_ep_0 = self.endpoints[0][Clusters.Descriptor][Clusters.Descriptor.Attributes.PartsList]
        endpoint_to_clusters = dict(self.endpoints.items())
        for endpoint_id in parts_list_ep_0:
            descriptor_cluster = endpoint_to_clusters[endpoint_id][Clusters.Descriptor]
            if Clusters.Descriptor.Attributes.EndpointUniqueID in descriptor_cluster:
                value = descriptor_cluster[Clusters.Descriptor.Attributes.EndpointUniqueID]
                self.print_step(7.1, "Validate EndpointUniqueId attribute in Descriptor cluster is of string type for endpoint : {endpoint_id}".format(
                    endpoint_id=endpoint_id))
                if isinstance(value, str):
                    if not value:
                        continue
                    self.print_step(7.2, "Validate EndpointUniqueId attribute in Descriptor cluster is not more than 32 bytes for endpoint : {endpoint_id}".format(
                        endpoint_id=endpoint_id))
                    if len(value) > END_POINT_UNIQUE_ID_LENGTH_BYTES:
                        location = AttributePathLocation(
                            endpoint_id,
                            Clusters.Descriptor.id,
                            Clusters.Descriptor.Attributes.EndpointUniqueID.attribute_id
                        )
                        self.record_error(
                            self.get_test_name(),
                            location=location,
                            problem=f"EndpointUniqueId attribute length is {len(value)} bytes which exceeds the maximum allowed 32 bytes",
                            spec_location="EndpointUniqueId attribute"
                        )
                        self.fail_current_test(
                            "EndpointUniqueId attribute in Descriptor cluster is more than 32 bytes for endpoint")
                    self.print_step(7.3, "Validate EndpointUniqueId attribute in Descriptor cluster is not a duplicate for endpoint : {endpoint_id}".format(
                        endpoint_id=endpoint_id))

                    if value in unique_ids:
                        location = AttributePathLocation(endpoint_id, Clusters.Descriptor.id,
                                                         Clusters.Descriptor.Attributes.EndpointUniqueID.attribute_id)
                        self.record_error(
                            self.get_test_name(),
                            location=location,
                            problem=f"Duplicate EndpointUniqueId found: {value}",
                            spec_location="EndpointUniqueId attribute"
                        )
                        self.fail_current_test("Duplicate EndpointUniqueId found")
                    unique_ids.append(value)
                else:
                    location = AttributePathLocation(
                        endpoint_id,
                        Clusters.Descriptor.id,
                        Clusters.Descriptor.Attributes.EndpointUniqueID.attribute_id
                    )
                    self.record_error(
                        self.get_test_name(),
                        location=location,
                        problem="EndpointUniqueId attribute is present but not a string",
                        spec_location="EndpointUniqueId attribute"
                    )
                    self.fail_current_test("EndpointUniqueId attribute is present but not a string")


if __name__ == "__main__":
    default_matter_test_main()
