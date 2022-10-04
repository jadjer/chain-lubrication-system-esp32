//
// Created by jadjer on 29.08.22.
//

#include "Controller.hpp"
#include "AdvertisedDevice.hpp"
#include "BLE2901.hpp"
#include "BlinkIndicator.hpp"
#include "ServicesUUID.hpp"
#include "SetDelayCallback.hpp"
#include "SetDistanceCallback.hpp"
#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>

Controller::Controller() : m_button(0), m_power(32), m_pump(17) {
  BLEDevice::init("CLS");

  m_speed = 0;
  m_indicator = new BlinkIndicator(2);
  m_maxDistance = 500;
  m_enableDelay = 5000;
  m_vehicleSpeed = nullptr;

  log_i("Bluetooth server start ...");
  m_indicator->blink(100);

  m_server = BLEDevice::createServer();
  m_server->setCallbacks(&m_serverCallback);

  m_client = BLEDevice::createClient();

  {
    auto service = m_server->createService(serviceSettingsUUID, 9);

    m_settingDistance = service->createCharacteristic(settingDistanceUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    m_settingDistance->setCallbacks(new SetDistanceCallback());
    m_settingDistance->addDescriptor(new BLE2901("Minimal Distance"));

    m_settingDelay = service->createCharacteristic(settingDelayUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    m_settingDelay->setCallbacks(new SetDelayCallback());
    m_settingDelay->addDescriptor(new BLE2901("Lubricate Time"));

    service->start();
  }

  {
    auto service = m_server->createService(serviceMonitorUUID, 11);

    m_monitorState = service->createCharacteristic(monitorStateUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    m_monitorState->addDescriptor(new BLE2901("State"));
    m_monitorState->addDescriptor(new BLE2902());

    m_monitorDistance = service->createCharacteristic(monitorDistanceUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    m_monitorDistance->addDescriptor(new BLE2901("Distance"));
    m_monitorDistance->addDescriptor(new BLE2902());

    service->start();
  }

  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(serviceSettingsUUID);
  advertising->start();

  log_i("HDS start scan...");
  m_indicator->blink(250);

  log_i("Initialize done");
  m_indicator->enable();

  m_monitorState->setValue("Ready");
}

Controller::~Controller() = default;

[[noreturn]] void Controller::spin() {
  while (true) {
    if (not m_client->isConnected()) { connectToServer(); }
    if (m_serverCallback.isConnected()) { updateCharacteristics(); }
    if (m_button.isPressed()) { manualLubricate(); }
    if (m_distance.getDistance() >= m_maxDistance) { m_pump.enable(m_enableDelay); }
    if (m_speed <= 0) { m_pump.pause(); }
    if (m_speed > 0) { m_pump.unpause(); }
    if (not m_power.isEnabled()) { sleep(); }

    spinPump();

    delay(100);
  }
}

void Controller::sleep() {
  if (m_pump.isEnabled()) { return; }
  m_power.sleep();
}

void Controller::connectToServer() {
  AdvertisedDevice advertisedDevice;

  auto scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(&advertisedDevice);
  scan->setInterval(1349);
  scan->setWindow(449);
  scan->setActiveScan(false);
  scan->start(5, false);

  auto device = advertisedDevice.getAdvertisedDevice();
  if (device == nullptr) { return; }

  log_i("Forming a connection to %s", device->getAddress().toString().c_str());

  m_client->connect(device);

  if (not m_client->isConnected()) { return; }

  log_i(" - Connected to server");

  m_client->setMTU(517);

  BLERemoteService* serviceVehicle = m_client->getService(serviceVehicleUUID);
  if (serviceVehicle == nullptr) {
    m_client->disconnect();
    return;
  }

  m_vehicleSpeed = serviceVehicle->getCharacteristic(vehicleSpeedUUID);
  if (m_vehicleSpeed == nullptr or not m_vehicleSpeed->canNotify()) {
    m_client->disconnect();
    return;
  }

  m_vehicleSpeed->registerForNotify([=](auto characteristic, uint8_t const* data, size_t length, bool isNotify) {
    if (not isNotify) { return; }
    if (length == 0) { return; }

    m_speed = data[0];
    m_distance.updateSpeed(m_speed);
  });
}

void Controller::updateCharacteristics() {
  auto state = m_pump.getState();
  auto distance = m_distance.getDistance();

  switch (state) {
    case PumpState::ENABLE:
      m_monitorState->setValue("Enabled");
      break;
    case PumpState::PAUSE:
      m_monitorState->setValue("Paused");
      break;
    case PumpState::DISABLE:
      m_monitorState->setValue("Disabled");
      break;
  }
  m_monitorState->notify();

  m_monitorDistance->setValue(distance);
  m_monitorDistance->notify();
}

void Controller::manualLubricate() {
  m_pump.enable(m_enableDelay);
  m_button.resetState();
}

void Controller::spinPump() {
  m_pump.spinOnce();

  auto state = m_pump.getState();
  switch (state) {
    case PumpState::ENABLE:
      m_indicator->blink(500);
      break;
    case PumpState::PAUSE:
      m_indicator->blink(1000);
      break;
    case PumpState::DISABLE:
      m_indicator->enable();
      break;
  }
}
