//
// Created by Parti on 2021/2/19.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <structures/HandlerManagerBase.h>
#include <utils/websocket.h>

namespace hephaestus::plugins {
    class HandlerManager : public structures::HandlerManagerBase<HandlerManager> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;
    public:
        void initAndStart(const Json::Value &config) override;

        void shutdown() override;
    };
}