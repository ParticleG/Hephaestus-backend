//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/HttpController.h>
#include <helpers/I18nHelper.h>
#include <plugins/Perfmon.h>
#include <structures/ExceptionHandlers.h>

namespace hephaestus::api::v1 {
    class Control :
            public drogon::HttpController<Control>,
            public structures::ResponseJsonHandler,
            public helpers::I18nHelper<Control> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        Control();

        METHOD_LIST_BEGIN
            METHOD_ADD(
                    Control::captureScreen,
                    "/game",
                    drogon::Post,
                    drogon::Options,
                    "drogon::IntranetIpFilter",
                    "hephaestus::filters::ControlCaptureScreen"
            );
        METHOD_LIST_END

        void captureScreen(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    private:
        plugins::Perfmon *_perfmon;
    };
}

