//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <helpers/I18nHelper.h>
#include <structures/ExceptionHandlers.h>

/**
 * @brief This filter checks "Control::captureScreen" request body
 * @param processName: String
 * @param windowName: String
 * @param timeout: UInt
 * @param keycode: Int
 * @return windowHandle: in request attributes
 */

namespace hephaestus::filters {
    class ControlCaptureScreen :
            public drogon::HttpFilter<ControlCaptureScreen>,
            public structures::RequestJsonHandler<ControlCaptureScreen> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}