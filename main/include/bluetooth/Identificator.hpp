//
// Created by jadjer on 9/26/24.
//

#pragma once

#include "NimBLEUUID.h"

auto const SERVICE_CONFIGURATION_UUID = NimBLEUUID("02d7fa9b-c320-4a01-ac0f-26d5a26c6deb");
auto const CHARACTERISTIC_PUMP_TIMEOUT_UUID = NimBLEUUID("0a8608b2-063c-4778-83e3-5b59b8b8183c");
auto const CHARACTERISTIC_WHEEL_DIAMETER_UUID = NimBLEUUID("9b3b071b-c830-418c-9aaa-9b188d300aef");
auto const CHARACTERISTIC_MINIMAL_SPEED_UUID = NimBLEUUID("2793a1e8-fdb1-4c49-b3f5-2ff524f49b55");
auto const CHARACTERISTIC_DISTANCE_FOR_ENABLE_UUID = NimBLEUUID("cfd2f7de-7a42-4825-b9d5-66455a33d183");
auto const CHARACTERISTIC_TOTAL_DISTANCE_UUID = NimBLEUUID("f4a5565e-c488-4b53-a95d-73fd2ea0c19a");
auto const CHARACTERISTIC_NEXT_DISTANCE_UUID = NimBLEUUID("d7a8e60c-df93-409d-825a-331881193bec");

auto const SERVICE_CONTROL_UUID = NimBLEUUID("cc9d7e39-f78b-47c1-b8ce-9c4cf49ffabb");
auto const CHARACTERISTIC_MANUAL_LUBRICATE_UUID = NimBLEUUID("ccbfe972-5551-4c69-bd7a-f18d1dfcd488");

auto const SERVICE_OTA_UUID = NimBLEUUID("8018");
auto const CHARACTERISTIC_RECEIVE_FIRMWARE_UUID = NimBLEUUID("8020");
auto const CHARACTERISTIC_PROGRESS_UUID = NimBLEUUID("8021");
auto const CHARACTERISTIC_COMMAND_UUID = NimBLEUUID("8022");
auto const CHARACTERISTIC_CUSTOMER_UUID = NimBLEUUID("8023");
