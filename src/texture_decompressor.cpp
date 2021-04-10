#include <texture_decompressor.h>

bool read_r8g8b8a8(const char* buffer, char* out_buffer, uint32_t width, uint32_t height) {
    uint64_t out_offset = 0;
    uint64_t in_offset = 0;
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            out_buffer[out_offset++] = buffer[in_offset++]; //R
            out_buffer[out_offset++] = buffer[in_offset++]; //G
            out_buffer[out_offset++] = buffer[in_offset++]; //B
            out_buffer[out_offset++] = buffer[in_offset++]; //A
        }
    }
    return true;
}


