//
// Created by MED45 on 09.04.2020.
//
/*
MIT License
Copyright (c) 2020 Florian Weischer
Copyright (c) 2015 Steam Database
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef COMPRESSED_INDEX_BUFFER_H
#define COMPRESSED_INDEX_BUFFER_H

#include <cstdint>
#include <vector>
#include <utils.hpp>
#include <binary_io/binary_io.h>

class MeshOptimizerIndexDecoder {
public:
    static constexpr uint8_t IndexHeader = 0xe0;

    static void PushEdgeFifo(std::array<std::pair<uint32_t, uint32_t>, 16> &fifo, int &offset, uint32_t a, uint32_t b);

    static void PushVertexFifo(std::array<uint32_t, 16> &fifo, int &offset, uint32_t v, bool cond = true);

    static uint32_t DecodeVByte(MemoryBinaryIO &data);

    static uint32_t DecodeIndex(MemoryBinaryIO &data, uint32_t next, uint32_t last);

    static void
    WriteTriangle(std::vector<uint8_t> &destination, int offset, int indexSize, uint32_t a, uint32_t b, uint32_t c);

    static std::vector<uint8_t> DecodeIndexBuffer(int indexCount, int indexSize, std::vector<uint8_t> &buffer);
};

#endif //COMPRESSED_INDEX_BUFFER_H
