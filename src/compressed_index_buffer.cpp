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

#include <compressed_index_buffer.h>

////////////////

void MeshOptimizerIndexDecoder::PushEdgeFifo(std::array<std::pair<uint32_t, uint32_t>,16> &fifo, int &offset, uint32_t a, uint32_t b)
{
    fifo[offset] = {a,b};
    offset = (offset + 1) & 15;
}

void MeshOptimizerIndexDecoder::PushVertexFifo(std::array<uint32_t,16> &fifo, int &offset, uint32_t v, bool cond)
{
    fifo[offset] = v;
    offset = (offset + (cond ? 1 : 0)) & 15;
}

uint32_t MeshOptimizerIndexDecoder::DecodeVByte(MemoryBinaryIO &data)
{
    auto lead = static_cast<uint32_t>(data.read_u8());

    if (lead < 128)
    {
        return lead;
    }

    auto result = lead & 127;
    auto shift = 7;

    for (auto i = 0; i < 4; i++)
    {
        auto group = static_cast<uint32_t>(data.read_u8());
        result |= (group & 127) << shift;
        shift += 7;

        if (group < 128)
        {
            break;
        }
    }

    return result;
}

uint32_t MeshOptimizerIndexDecoder::DecodeIndex(MemoryBinaryIO &data, uint32_t next, uint32_t last)
{
    auto v = DecodeVByte(data);
    auto d = (uint32_t)((v >> 1) ^ -(v & 1));

    return last + d;
}
void MeshOptimizerIndexDecoder::WriteTriangle(std::vector<uint8_t> &destination, int offset, int indexSize, uint32_t a, uint32_t b, uint32_t c)
{
    offset *= indexSize;
    if (indexSize == 2)
    {
        std::array<uint16_t,3> values = {
                static_cast<uint16_t>(a),
                static_cast<uint16_t>(b),
                static_cast<uint16_t>(c)
        };
        memcpy(destination.data() +offset,values.data(),values.size() *sizeof(values.front()));
    }
    else
    {
        std::array<uint32_t,3> values = {a,b,c};
        memcpy(destination.data() +offset,values.data(),values.size() *sizeof(values.front()));
    }
}

std::vector<uint8_t> MeshOptimizerIndexDecoder::DecodeIndexBuffer(int indexCount, int indexSize, std::vector<uint8_t> &buffer)
{
    if (indexCount % 3 != 0)
        throw std::invalid_argument{"Expected indexCount to be a multiple of 3."};

    if (indexSize != 2 && indexSize != 4)
        throw std::invalid_argument{"Expected indexSize to be either 2 or 4"};

    auto dataOffset = 1 + (indexCount / 3);

    // the minimum valid encoding is header, 1 byte per triangle and a 16-byte codeaux table
    if (buffer.size() < dataOffset + 16)
        throw std::invalid_argument{"Index buffer is too short."};

    if (buffer[0] != IndexHeader)
        throw std::invalid_argument{"Incorrect index buffer header."};

    std::array<uint32_t,16> vertexFifo {};
    std::array<std::pair<uint32_t, uint32_t>,16> edgeFifo {};
    auto edgeFifoOffset = 0;
    auto vertexFifoOffset = 0;

    auto next = 0u;
    auto last = 0u;

    auto bufferIndex = 1;
    auto data = slice(buffer,dataOffset, buffer.size() - 16 - dataOffset);

    auto codeauxTable = slice(buffer,buffer.size() - 16);

    std::vector<uint8_t> destination {};
    destination.resize(indexCount * indexSize);

    MemoryBinaryIO ds {data.data(),static_cast<uint32_t>(data.size() *sizeof(data.front()))};
    ds.seek(0);
    for (auto i = 0; i < indexCount; i += 3)
    {
        auto codetri = buffer[bufferIndex++];

        if (codetri < 0xf0)
        {
            auto fe = codetri >> 4;

            auto &ab = edgeFifo[(edgeFifoOffset - 1 - fe) & 15];

            auto fec = codetri & 15;

            if (fec != 15)
            {
                auto c = fec == 0 ? next : vertexFifo[(vertexFifoOffset - 1 - fec) & 15];

                auto fec0 = fec == 0;
                next += fec0 ? 1u : 0u;

                WriteTriangle(destination, i, indexSize, ab.first, ab.second, c);

                PushVertexFifo(vertexFifo, vertexFifoOffset, c, fec0);

                PushEdgeFifo(edgeFifo, edgeFifoOffset, c, ab.second);
                PushEdgeFifo(edgeFifo, edgeFifoOffset, ab.first, c);
            }
            else
            {
                auto c = last = DecodeIndex(ds, next, last);

                WriteTriangle(destination, i, indexSize, ab.first, ab.second, c);

                PushVertexFifo(vertexFifo, vertexFifoOffset, c);

                PushEdgeFifo(edgeFifo, edgeFifoOffset, c, ab.second);
                PushEdgeFifo(edgeFifo, edgeFifoOffset, ab.first, c);
            }
        }
        else
        {
            if (codetri < 0xfe)
            {
                auto codeaux = codeauxTable[codetri & 15];

                auto feb = codeaux >> 4;
                auto fec = codeaux & 15;

                auto a = next++;

                auto b = (feb == 0) ? next : vertexFifo[(vertexFifoOffset - feb) & 15];

                auto feb0 = feb == 0 ? 1u : 0u;
                next += feb0;

                auto c = (fec == 0) ? next : vertexFifo[(vertexFifoOffset - fec) & 15];

                auto fec0 = fec == 0 ? 1u : 0u;
                next += fec0;

                WriteTriangle(destination, i, indexSize, a, b, c);

                PushVertexFifo(vertexFifo, vertexFifoOffset, a);
                PushVertexFifo(vertexFifo, vertexFifoOffset, b, feb0 == 1u);
                PushVertexFifo(vertexFifo, vertexFifoOffset, c, fec0 == 1u);

                PushEdgeFifo(edgeFifo, edgeFifoOffset, b, a);
                PushEdgeFifo(edgeFifo, edgeFifoOffset, c, b);
                PushEdgeFifo(edgeFifo, edgeFifoOffset, a, c);
            }
            else
            {
                auto codeaux = static_cast<uint32_t>(ds.read_u8());

                auto fea = codetri == 0xfe ? 0 : 15;
                auto feb = codeaux >> 4;
                auto fec = codeaux & 15;

                auto a = (fea == 0) ? next++ : 0;
                auto b = (feb == 0) ? next++ : vertexFifo[(vertexFifoOffset - feb) & 15];
                auto c = (fec == 0) ? next++ : vertexFifo[(vertexFifoOffset - fec) & 15];

                if (fea == 15)
                {
                    last = a = DecodeIndex(ds, next, last);
                }

                if (feb == 15)
                {
                    last = b = DecodeIndex(ds, next, last);
                }

                if (fec == 15)
                {
                    last = c = DecodeIndex(ds, next, last);
                }

                WriteTriangle(destination, i, indexSize, a, b, c);

                PushVertexFifo(vertexFifo, vertexFifoOffset, a);
                PushVertexFifo(vertexFifo, vertexFifoOffset, b, (feb == 0) || (feb == 15));
                PushVertexFifo(vertexFifo, vertexFifoOffset, c, (fec == 0) || (fec == 15));

                PushEdgeFifo(edgeFifo, edgeFifoOffset, b, a);
                PushEdgeFifo(edgeFifo, edgeFifoOffset, c, b);
                PushEdgeFifo(edgeFifo, edgeFifoOffset, a, c);
            }
        }
    }
    if(ds.tell() != ds.size())
        throw std::runtime_error{"we didn't read all data bytes and stopped before the boundary between data and codeaux table"};

    return destination;
}