//
// Created by ParticleG on 2022/3/11.
//

#pragma once

#include <drogon/drogon.h>

namespace hephaestus::types {
    enum class MessageType {
        error,
        failed,
        client,
        server,
    };
}