#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by i.getsman on 30.03.2020.
//

#include <cmath>

#include <texture_decompressor.h>


bool read_bc7(const char* buffer, char* out_buffer, uint32_t width,
              uint32_t height, bool hemi_ocr_rb) {
    auto texture = new detexTexture;
    texture->data = (uint8_t*) buffer;
    texture->width = width;
    texture->width_in_blocks = (width + 3) / 4;
    texture->height = height;
    texture->height_in_blocks = (height + 3) / 4;
    texture->format = DETEX_TEXTURE_FORMAT_BPTC;

    return detexDecompressTextureLinear(texture, (uint8_t*) out_buffer, DETEX_PIXEL_FORMAT_RGBA8);
}

bool read_ATI1N(const char* buffer, char* out_buffer, uint32_t width,
                uint32_t height) {
    auto texture = new detexTexture;
    texture->data = (uint8_t*) buffer;
    texture->width = width;
    texture->width_in_blocks = (width + 3) / 4;
    texture->height = height;
    texture->height_in_blocks = (height + 3) / 4;
    texture->format = DETEX_TEXTURE_FORMAT_RGTC1;

    return detexDecompressTextureLinear(texture, (uint8_t*) out_buffer, DETEX_PIXEL_FORMAT_RGBA8);
}

bool
read_ATI2N(const char* buffer, char* out_buffer, uint32_t width,
           uint32_t height) {
    auto texture = new detexTexture;
    texture->data = (uint8_t*) buffer;
    texture->width = width;
    texture->width_in_blocks = (width + 3) / 4;
    texture->height = height;
    texture->height_in_blocks = (height + 3) / 4;
    texture->format = DETEX_TEXTURE_FORMAT_RGTC2;

    return detexDecompressTextureLinear(texture, (uint8_t*) out_buffer, DETEX_PIXEL_FORMAT_RGBA8);
}

bool
read_DXT1(const char* buffer, char* out_buffer, uint32_t width,
          uint32_t height) {
    auto texture = new detexTexture;
    texture->data = (uint8_t*) buffer;
    texture->width = width;
    texture->width_in_blocks = (width + 3) / 4;
    texture->height = height;
    texture->height_in_blocks = (height + 3) / 4;
    texture->format = DETEX_TEXTURE_FORMAT_BC1;

    return detexDecompressTextureLinear(texture, (uint8_t*) out_buffer, DETEX_PIXEL_FORMAT_RGBA8);
}

bool
read_DXT5(const char* buffer, char* out_buffer, uint32_t width,
          uint32_t height) {
    auto texture = new detexTexture;
    texture->data = (uint8_t*) buffer;
    texture->width = width;
    texture->width_in_blocks = (width + 3) / 4;
    texture->height = height;
    texture->height_in_blocks = (height + 3) / 4;
    texture->format = DETEX_TEXTURE_FORMAT_BC3;

    return detexDecompressTextureLinear(texture, (uint8_t*) out_buffer, DETEX_PIXEL_FORMAT_RGBA8);
}

#pragma clang diagnostic pop