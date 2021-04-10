#ifndef SOURCEIOTEXTUREUTILS_TEXTURE_DECOMPRESSOR_H
#define SOURCEIOTEXTUREUTILS_TEXTURE_DECOMPRESSOR_H

#include <cstdint>

#include <utils.hpp>
#include <detex/detex.h>

dll_export bool
read_i8(const char* buffer, char* out_buffer, uint32_t width,
        uint32_t height);

dll_export bool
read_ia88(const char* buffer, char* out_buffer, uint32_t width,
          uint32_t height);

dll_export bool
read_r8g8b8(const char* buffer, char* out_buffer, uint32_t width,
            uint32_t height);

dll_export bool
read_r8g8b8a8(const char* buffer, char* out_buffer, uint32_t width,
              uint32_t height);

dll_export bool
read_r16(const char* buffer, char* out_buffer, uint32_t width,
         uint32_t height);

dll_export bool
read_r16g16(const char* buffer, char* out_buffer, uint32_t width,
            uint32_t height);

dll_export bool
read_r16f(const char* buffer, char* out_buffer, uint32_t width,
          uint32_t height);

dll_export bool
read_r16g16f(const char* buffer, char* out_buffer, uint32_t width,
             uint32_t height);

dll_export bool
read_r16g16b16a16(const char* buffer, char* out_buffer, uint32_t width,
                  uint32_t height);

dll_export bool
read_r16g16b16a16f(const char* buffer, char* out_buffer, uint32_t width,
                   uint32_t height);

dll_export bool
read_r32f(const char* buffer, char* out_buffer, uint32_t width,
          uint32_t height);

dll_export bool
read_r32g32f(const char* buffer, char* out_buffer, uint32_t width,
             uint32_t height);

dll_export bool
read_r32g32b32f(const char* buffer, char* out_buffer, uint32_t width,
                uint32_t height);

dll_export bool
read_r32g32b32a32f(const char* buffer, char* out_buffer, uint32_t width,
                   uint32_t height);

dll_export bool
read_ATI1N(const char* buffer, char* out_buffer, uint32_t width,
           uint32_t height);

dll_export bool
read_ATI2N(const char* buffer, char* out_buffer, uint32_t width,
           uint32_t height);

dll_export bool
read_DXT1(const char* buffer, char* out_buffer, uint32_t width,
          uint32_t height);

dll_export bool
read_DXT5(const char* buffer, char* out_buffer, uint32_t width,
          uint32_t height);

dll_export bool
read_bc7(const char* buffer, char* out_buffer, uint32_t width,
         uint32_t height, bool hemi_ocr_rb);

#endif //SOURCEIOTEXTUREUTILS_TEXTURE_DECOMPRESSOR_H
