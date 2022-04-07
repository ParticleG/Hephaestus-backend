//
// Created by ParticleG on 2022/3/28.
//

#pragma once

#include <drogon/drogon.h>

namespace hephaestus::utils {
    std::array<double, 3> rgb2hsv(uint32_t rgb);

    std::array<double, 3> rgb2hsv(const std::array<uint8_t, 3> &rgb);

    std::array<uint8_t, 3> hsv2rgb(const std::array<double, 3> &hsv);
}