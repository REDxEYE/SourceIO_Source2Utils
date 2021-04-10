//
// Created by i.getsman on 30.03.2020.
//

#ifndef SOURCEIOTEXTUREUTILS_UTILS_HPP

#include <emmintrin.h>
#define TCB_SPAN_NO_CONTRACT_CHECKING
#include <tcb/span.hpp>

#ifdef __GNUC__
#define dll_export  __attribute__ ((dllexport))
#else
#define dll_export   __declspec( dllexport )
#define dll_import   __declspec( dllimport )
#endif


static float half2float(const uint16_t &val) {
#ifndef __F16CINTRIN_H
    // Extract the sign from the bits
    const uint32_t sign = static_cast<uint32_t>(val & 0x8000) << 16;
    // Extract the exponent from the bits
    const uint8_t exp16 = (val & 0x7c00) >> 10;
    // Extract the fraction from the bits
    uint16_t frac16 = val & 0x3ff;

    uint32_t exp32 = 0;
    if (__builtin_expect(exp16 == 0x1f, 0)) {
        exp32 = 0xff;
    } else if (__builtin_expect(exp16 == 0, 0)) {
        exp32 = 0;
    } else {
        exp32 = static_cast<uint32_t>(exp16) + 112;
    }

    // corner case: subnormal -> normal
    // The denormal number of FP16 can be represented by FP32, therefore we need
    // to recover the exponent and recalculate the fration.
    if (__builtin_expect(exp16 == 0 && frac16 != 0, 0)) {
        uint8_t OffSet = 0;
        do {
            ++OffSet;
            frac16 <<= 1;
        } while ((frac16 & 0x400) != 0x400);
        // mask the 9th bit
        frac16 &= 0x3ff;
        exp32 = 113 - OffSet;
    }

    uint32_t frac32 = frac16 << 13;

    // Compose the final FP32 binary
    uint32_t bits = 0;

    bits |= sign;
    bits |= (exp32 << 23);
    bits |= frac32;

    return *reinterpret_cast<float*>(&bits);
#else
    return _cvtsh_ss(val);
#endif
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-non-const-parameter"
static void flip_image(uint32_t* data, uint32_t width, uint32_t height) {
    for (auto y = 0; y < height / 2; y++) {
        for (auto x = 0; x < width; x++) {
            auto &a = data[y * width + x];
            auto &b = data[(height - y - 1) * width + x];
            a ^= b;
            b ^= a;
            a ^= b;
        }
    }
}
#pragma clang diagnostic pop

static tcb::span<uint8_t> slice(const tcb::span<uint8_t> &data,uint32_t start,uint32_t len=std::numeric_limits<uint32_t>::max())
{
    return data.subspan(start,len);
}

#define SOURCEIOTEXTUREUTILS_UTILS_HPP

#endif //SOURCEIOTEXTUREUTILS_UTILS_HPP
