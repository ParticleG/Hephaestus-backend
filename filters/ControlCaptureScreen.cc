//
// Created by particleg on 2021/9/27.
//

#include <filters/ControlCaptureScreen.h>
#include <helpers/ResponseJson.h>

using namespace drogon;
using namespace std;
using namespace hephaestus::filters;
using namespace hephaestus::helpers;
using namespace hephaestus::types;

void ControlCaptureScreen::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    if (req->method() == Options) {
        ResponseJson().httpCallback(failedCb, "*");
        return;
    }

    handleExceptions([&]() {
        auto request = RequestJson(req);
        request.trim("processName", JsonValue::String);
        request.trim("windowName", JsonValue::String);
        request.trim("timeout", JsonValue::UInt);
        request.trim("keycode", JsonValue::Int);
        req->attributes()->insert("requestJson", request);
        nextCb();
    }, failedCb);
}
