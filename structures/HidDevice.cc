//
// Created by ParticleG on 2022/3/28.
//

#include <structures/Exceptions.h>
#include <structures/HidDevice.h>

using namespace hephaestus::structures;
using namespace hephaestus::types;
using namespace std;

HidDevice::HidDevice(const string &path, uint8_t reportLength, bool enableRead) : reportLength(reportLength) {
    _deviceHandle = hid_open_path(path.c_str());
    if (!_deviceHandle) {
        throw HidException(error(), HidAction::open);
    }
    isOpen = true;

    if (enableRead) {
        thread([this]() {
            vector<uint8_t> data(this->reportLength, 0);
            while (isOpen) {
                if (read(data)) {
                    _messageCallback(data);
                }
            }
        }).detach();
    }
}

void HidDevice::write(const vector<uint8_t> &data) {
    if (data.size() != reportLength) {
        throw MessageException(i18n("invalidSize"));
    }
    const auto result = hid_write(_deviceHandle, data.data(), data.size());
    if (result != data.size()) {
        throw HidException(error(), HidAction::write);
    }
}

bool HidDevice::read(vector<uint8_t> &data) {
    if (data.size() != reportLength) {
        data.resize(reportLength, 0);
    }
    const auto result = hid_read(_deviceHandle, data.data(), data.size());
    if (result == 0) {
        return false;
    }
    if (result != data.size()) {
        throw HidException(error(), HidAction::read);
    }
    return true;
}

void HidDevice::setMessageCallback(const HidDevice::MessageHandler &callback) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _messageCallback = callback;
}

std::string HidDevice::error() {
    return converter.to_bytes(hid_error(_deviceHandle));
}

HidDevice::~HidDevice() {
    if (_deviceHandle) {
        hid_close(_deviceHandle);
    }
    isOpen = false;
}
