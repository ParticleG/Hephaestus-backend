//
// Created by ParticleG on 2022/3/28.
//

#include <drogon/drogon.h>
#include <plugins/HidManager.h>
#include <structures/Exceptions.h>

using namespace drogon;
using namespace hephaestus::plugins;
using namespace hephaestus::structures;
using namespace hephaestus::types;
using namespace std;

void HidManager::initAndStart(const Json::Value &config) {
    if (!(
            config["taskInterval"].isDouble() &&
            config["taskInterval"].asDouble() >= 5.0
    )) {
        LOG_ERROR << R"("Invalid taskInterval config")";
        abort();
    }

    if (hid_init()) {
        LOG_ERROR << "HIDAPI library failed to initialize";
        abort();
    }

    updateHidList();
    app().getLoop()->runEvery(config["taskInterval"].asDouble(), [this] {
        updateHidList();
    });

    LOG_INFO << "HidManager loaded.";
}

void HidManager::shutdown() {
    if (hid_exit()) {
        LOG_ERROR << "HIDAPI library failed to exit";
    }
    LOG_INFO << "HidManager shutdown.";
}

void HidManager::updateHidList() {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _hidList.clear();
    auto devicesPtr = hid_enumerate({}, {});
    auto currentDevicePtr = devicesPtr;
    while (currentDevicePtr) {
        _hidList.emplace_back(currentDevicePtr);
        currentDevicePtr = currentDevicePtr->next;
    }
    hid_free_enumeration(devicesPtr);
}

std::string HidManager::getPath(
        uint16_t vendor_id,
        uint16_t product_id,
        int32_t collection_number,
        int32_t interface_number
) {
    string path;
    if (!_checkDevice([=, &path](const HidInfo &device) {
        if (vendor_id != device.vendor_id || product_id != device.product_id) {
            return false;
        }
        if (collection_number >= 0 && collection_number != device.collection_number) {
            return false;
        }
        if (interface_number >= 0 && interface_number != device.interface_number) {
            return false;
        }
        path = device.path;
        return true;
    })) {
        throw MessageException("deviceNotFound");
    }
    return path;
}

shared_ptr<HidDevice> HidManager::getDevice(const string &path) {
    bool found;
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        found = _deviceMap.contains(path);
    }
    if (!found) {
        _subscribe(path);
    }
    return shared_ptr<HidDevice>(&_deviceMap.at(path));
}

void HidManager::_subscribe(const string &path) {
    if (!_checkDevice([&path](const HidInfo &device) {
        return device.path == path;
    })) {
        throw MessageException("deviceNotFound");
    }
    unique_lock<shared_mutex> lock(_sharedMutex);
    _deviceMap.try_emplace(path, path);
}

void HidManager::_unsubscribe(const string &path) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _deviceMap.erase(path);
}

bool HidManager::_checkDevice(const function<bool(const HidInfo &)> &func) {
    bool result;
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        result = ranges::any_of(_hidList, func);
    }
    if (!result) {
        updateHidList();
        shared_lock<shared_mutex> lock(_sharedMutex);
        result = ranges::any_of(_hidList, func);
    }
    return result;
}
