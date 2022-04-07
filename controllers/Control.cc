//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Control.h>
#include <helpers/ResponseJson.h>
#include <types/JsonValue.h>
#include <utils/win32.h>

using namespace drogon;
using namespace std;
using namespace hephaestus::api::v1;
using namespace hephaestus::helpers;
using namespace hephaestus::plugins;
using namespace hephaestus::structures;
using namespace hephaestus::types;
using namespace hephaestus::utils;

Control::Control() :
        ResponseJsonHandler(
                [](const ResponseException &e, ResponseJson &response) {
                    response.setStatusCode(e.statusCode());
                    // TODO: Check if this causes too much copying
                    response(e.toJson());
                },
                [this](const orm::DrogonDbException &e, ResponseJson &response) {
                    LOG_ERROR << e.base().what();
                    response.setStatusCode(k500InternalServerError);
                    response.setResultCode(ResultCode::databaseError);
                    response.setMessage(i18n("databaseError"));
                },
                [this](const exception &e, ResponseJson &response) {
                    LOG_ERROR << e.what();
                    response.setStatusCode(k500InternalServerError);
                    response.setResultCode(ResultCode::internalError);
                    response.setMessage(i18n("internalError"));
                    response.setReason(e);
                }
        ),
        _perfmon(app().getPlugin<Perfmon>()) {}

void Control::captureScreen(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        HWND targetWindow;
        const auto requestJson = req->attributes()->get<RequestJson>("requestJson");

        string processName, windowName;
        if (requestJson.check("processName", JsonValue::String)) {
            processName = requestJson["processName"].asString();
        }
        if (requestJson.check("windowName", JsonValue::String)) {
            windowName = requestJson["windowName"].asString();
        }
        if (processName.empty() && windowName.empty()) {
            targetWindow = GetDesktopWindow();
        } else {
            targetWindow = FindWindow(
                    processName.empty() ? nullptr : processName.c_str(),
                    windowName.empty() ? nullptr : windowName.c_str()
            );
        }
        if (!targetWindow) {
            response.setStatusCode(k406NotAcceptable);
            response.setResultCode(ResultCode::notAcceptable);
            response.setMessage(i18n("windowNotFound"));
            return;
        }

        int32_t timeout = -1, keycode = -1;
        if (requestJson.check("timeout", JsonValue::Int)) {
            timeout = requestJson["timeout"].asInt();
        }
        if (requestJson.check("keycode", JsonValue::Int)) {
            keycode = requestJson["keycode"].asInt();
        }

        thread([&]() {
            while (true) {
                if (timeout < 0 && keycode < 0) {
                    break;
                } else {
                    if (keycode > 0 && GetAsyncKeyState(keycode)) {
                        this_thread::sleep_for(chrono::milliseconds(timeout));
                    }
                }
                this_thread::sleep_for(chrono::milliseconds(1));
                timeout--;
            }
            response.setData(win32::screen2File(targetWindow));
        }).join();
    }, response);
    response.httpCallback(callback, "*");
}
