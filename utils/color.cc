//
// Created by ParticleG on 2022/3/28.
//

#include <utils/color.h>
#include <cmath>

using namespace hephaestus;
using namespace std;

array<double, 3> utils::rgb2hsv(uint32_t rgb) {
    return rgb2hsv({static_cast<uint8_t>((rgb >> 16) & 0xFF),
                    static_cast<uint8_t>((rgb >> 8) & 0xFF),
                    static_cast<uint8_t>(rgb & 0xFF)});
}

std::array<double, 3> utils::rgb2hsv(const array<uint8_t, 3> &rgb) {
    array<double, 3> rgbNorm{};

    if (ranges::any_of(rgb, [](uint8_t channel) {
        return channel < 0 || channel > 255;
    })) {
        throw runtime_error("Invalid RGB color");
    }

    transform(
            rgb.begin(),
            rgb.end(),
            rgbNorm.begin(),
            [](uint8_t channel) { return channel / 255.0; }
    );
    const auto[r, g, b] = rgbNorm;

    double maxVal = *max_element(rgbNorm.begin(), rgbNorm.end()),
            minVal = *min_element(rgbNorm.begin(), rgbNorm.end()),
            delta = maxVal - minVal,
            h;

    if (maxVal == minVal) {
        h = 0;
    } else if (maxVal == r) {
        h = 60 * (g - b) / delta;
    } else if (maxVal == g) {
        h = 60 * (b - r) / delta + 120;
    } else {
        h = 60 * (r - g) / delta + 240;
    }

    return {h, maxVal == 0 ? 0 : delta / maxVal, maxVal};
}

array<uint8_t, 3> utils::hsv2rgb(const array<double, 3> &hsv) {
    double hue = hsv[0],
            saturation = hsv[1],
            value = hsv[2],
            r, g, b;

    if (hue < 0) {
        hue += 360;
    } else if (hue > 360) {
        hue -= 360;
    }

    if (saturation < 0 || saturation > 1)
        throw runtime_error("Invalid saturation");
    if (value < 0 || value > 1)
        throw runtime_error("Invalid value");

    double c = saturation * value;
    double x = c * (1 - abs(fmod(hue / 60.0, 2) - 1));
    double m = value - c;

    if (hue >= 0 && hue < 60) {
        r = c, g = x, b = 0;
    } else if (hue >= 60 && hue < 120) {
        r = x, g = c, b = 0;
    } else if (hue >= 120 && hue < 180) {
        r = 0, g = c, b = x;
    } else if (hue >= 180 && hue < 240) {
        r = 0, g = x, b = c;
    } else if (hue >= 240 && hue < 300) {
        r = x, g = 0, b = c;
    } else {
        r = c, g = 0, b = x;
    }
    return {
            static_cast<uint8_t>((r + m) * 255),
            static_cast<uint8_t>((g + m) * 255),
            static_cast<uint8_t>((b + m) * 255)
    };
}
