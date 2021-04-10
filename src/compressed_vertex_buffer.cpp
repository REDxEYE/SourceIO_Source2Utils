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
#include <compressed_vertex_buffer.h>
#include <memory.h>

// TODO: Move these to the header and replace tcb::span with std::span once C++-20 is available
static tcb::span<uint8_t>
DecodeBytesGroup(const tcb::span<uint8_t> &data, tcb::span<uint8_t> destination, int bitslog2);

static tcb::span<uint8_t> DecodeBytes(tcb::span<uint8_t> data, tcb::span<uint8_t> destination);

static tcb::span<uint8_t>
DecodeVertexBlock(tcb::span<uint8_t> data, tcb::span<uint8_t> vertexData, int vertexCount, int vertexSize,
                  tcb::span<uint8_t> lastVertex);


static void copy_to(const tcb::span<uint8_t> &data, tcb::span<uint8_t> dst) {
    memcpy(dst.data(), data.data(), data.size() * sizeof(data.front()));
}

uint32_t MeshOptimizerVertexDecoder::GetVertexBlockSize(uint32_t vertexSize) {
    auto result = VertexBlockSizeBytes / vertexSize;
    result &= ~(ByteGroupSize - 1);

    return result < VertexBlockMaxSize ? result : VertexBlockMaxSize;
}

uint8_t MeshOptimizerVertexDecoder::Unzigzag8(uint8_t v) {
    return static_cast<uint8_t>(-(v & 1) ^ (v >> 1));
}

tcb::span<uint8_t> DecodeBytesGroup(const tcb::span<uint8_t> &data, tcb::span<uint8_t> destination, int bitslog2) {
    uint32_t dataOffset = 0u;
    int dataVar;
    uint8_t b;

    auto Next = [&b, &dataVar](int bits, uint8_t encv) -> uint8_t {
        auto enc = b >> (8 - bits);
        b <<= bits;

        auto isSame = enc == (1 << bits) - 1;

        dataVar += isSame ? 1 : 0;
        return isSame ? encv : (uint8_t) enc;
    };

    switch (bitslog2) {
        case 0:
            for (auto k = 0; k < MeshOptimizerVertexDecoder::ByteGroupSize; k++) {
                destination[k] = 0;
            }

            return data;
        case 1:
            dataVar = 4;

            b = data[dataOffset++];
            destination[0] = Next(2, data[dataVar]);
            destination[1] = Next(2, data[dataVar]);
            destination[2] = Next(2, data[dataVar]);
            destination[3] = Next(2, data[dataVar]);

            b = data[dataOffset++];
            destination[4] = Next(2, data[dataVar]);
            destination[5] = Next(2, data[dataVar]);
            destination[6] = Next(2, data[dataVar]);
            destination[7] = Next(2, data[dataVar]);

            b = data[dataOffset++];
            destination[8] = Next(2, data[dataVar]);
            destination[9] = Next(2, data[dataVar]);
            destination[10] = Next(2, data[dataVar]);
            destination[11] = Next(2, data[dataVar]);

            b = data[dataOffset++];
            destination[12] = Next(2, data[dataVar]);
            destination[13] = Next(2, data[dataVar]);
            destination[14] = Next(2, data[dataVar]);
            destination[15] = Next(2, data[dataVar]);

            return slice(data, dataVar);
        case 2:
            dataVar = 8;

            b = data[dataOffset++];
            destination[0] = Next(4, data[dataVar]);
            destination[1] = Next(4, data[dataVar]);

            b = data[dataOffset++];
            destination[2] = Next(4, data[dataVar]);
            destination[3] = Next(4, data[dataVar]);

            b = data[dataOffset++];
            destination[4] = Next(4, data[dataVar]);
            destination[5] = Next(4, data[dataVar]);

            b = data[dataOffset++];
            destination[6] = Next(4, data[dataVar]);
            destination[7] = Next(4, data[dataVar]);

            b = data[dataOffset++];
            destination[8] = Next(4, data[dataVar]);
            destination[9] = Next(4, data[dataVar]);

            b = data[dataOffset++];
            destination[10] = Next(4, data[dataVar]);
            destination[11] = Next(4, data[dataVar]);

            b = data[dataOffset++];
            destination[12] = Next(4, data[dataVar]);
            destination[13] = Next(4, data[dataVar]);

            b = data[dataOffset++];
            destination[14] = Next(4, data[dataVar]);
            destination[15] = Next(4, data[dataVar]);

            return slice(data, dataVar);
        case 3: {
            copy_to(slice(data, 0, MeshOptimizerVertexDecoder::ByteGroupSize), destination);

            return slice(data, MeshOptimizerVertexDecoder::ByteGroupSize);
        }
        default:
            throw std::invalid_argument{"Unexpected bit length"};
    }
}

tcb::span<uint8_t> DecodeBytes(tcb::span<uint8_t> data, tcb::span<uint8_t> destination) {
    if (destination.size() % MeshOptimizerVertexDecoder::ByteGroupSize != 0)
        throw std::invalid_argument{"Expected data length to be a multiple of ByteGroupSize."};

    auto headerSize = ((destination.size() / MeshOptimizerVertexDecoder::ByteGroupSize) + 3) / 4;
    auto header = slice(data, 0);

    data = slice(data, headerSize);

    for (auto i = 0; i < destination.size(); i += MeshOptimizerVertexDecoder::ByteGroupSize) {
        if (data.size() < MeshOptimizerVertexDecoder::TailMaxSize)
            throw std::runtime_error{"Cannot decode"};

        auto headerOffset = i / MeshOptimizerVertexDecoder::ByteGroupSize;

        auto bitslog2 = (header[headerOffset / 4] >> ((headerOffset % 4) * 2)) & 3;

        data = DecodeBytesGroup(data, slice(destination, i), bitslog2);
    }

    return data;
}

tcb::span<uint8_t>
DecodeVertexBlock(tcb::span<uint8_t> data, tcb::span<uint8_t> vertexData, int vertexCount, int vertexSize,
                  tcb::span<uint8_t> lastVertex) {
    if (vertexCount <= 0 || vertexCount > MeshOptimizerVertexDecoder::VertexBlockMaxSize)
        throw new std::invalid_argument{"Expected vertexCount to be between 0 and VertexMaxBlockSize"};

    std::vector<uint8_t> buffer{};
    buffer.resize(MeshOptimizerVertexDecoder::VertexBlockMaxSize);
    std::vector<uint8_t> transposed{};
    transposed.resize(MeshOptimizerVertexDecoder::VertexBlockSizeBytes);

    auto vertexCountAligned = (vertexCount + MeshOptimizerVertexDecoder::ByteGroupSize - 1) &
                              ~(MeshOptimizerVertexDecoder::ByteGroupSize - 1);

    for (auto k = 0; k < vertexSize; ++k) {
        data = DecodeBytes(data, slice(buffer, 0, vertexCountAligned));

        auto vertexOffset = k;

        auto p = lastVertex[k];

        for (auto i = 0; i < vertexCount; ++i) {
            auto v = (uint8_t) (MeshOptimizerVertexDecoder::Unzigzag8(buffer[i]) + p);

            transposed[vertexOffset] = v;
            p = v;

            vertexOffset += vertexSize;
        }
    }

    copy_to(slice(transposed, 0, vertexCount * vertexSize), vertexData);

    copy_to(slice(transposed, vertexSize * (vertexCount - 1), vertexSize), lastVertex);

    return data;
}

std::vector<uint8_t> MeshOptimizerVertexDecoder::DecodeVertexBuffer(int vertexCount, int vertexSize,
                                                                    const std::vector<uint8_t> &vertexBuffer) {
    if (vertexSize <= 0 || vertexSize > 256)
        throw std::invalid_argument{"Vertex size is expected to be between 1 and 256"};

    if (vertexSize % 4 != 0) {
        throw std::invalid_argument{"Vertex size is expected to be a multiple of 4."};
    }

    if (vertexBuffer.size() < 1 + vertexSize) {
        throw std::invalid_argument{"Vertex buffer is too short."};
    }

    auto vertexSpan = tcb::span<uint8_t>{const_cast<std::vector<uint8_t> &>(vertexBuffer)};

    auto header = vertexSpan[0];
    vertexSpan = slice(vertexSpan, 1);
    if (header != VertexHeader) {
        throw std::invalid_argument{
                "Invalid vertex buffer header, expected " + std::to_string(VertexHeader) + " but got " +
                std::to_string(header) + "."};
    }

    std::vector<uint8_t> lastVertex{};
    lastVertex.resize(vertexSize);

    copy_to(slice(vertexSpan, vertexBuffer.size() - 1 - vertexSize, vertexSize), lastVertex);

    auto vertexBlockSize = GetVertexBlockSize(vertexSize);

    auto vertexOffset = 0;

    std::vector<uint8_t> result{};
    result.resize(vertexCount * vertexSize);

    while (vertexOffset < vertexCount) {
        printf("Decoding vertex block %i/%i\r", vertexOffset, vertexCount);
        auto blockSize = vertexOffset + vertexBlockSize < vertexCount
                         ? vertexBlockSize
                         : vertexCount - vertexOffset;

        vertexSpan = DecodeVertexBlock(vertexSpan, slice(result, vertexOffset * vertexSize), blockSize, vertexSize,
                                       lastVertex);

        vertexOffset += blockSize;
    }
    printf("\n");
    fflush(stdout);
    return result;
}