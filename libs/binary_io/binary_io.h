#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <memory.h>
#include <ios>

class MemoryBinaryIO {
    std::vector<uint8_t> memory;
public:

    MemoryBinaryIO(uint8_t* data, uint32_t size) {
        pos = 0;
        memory.resize(size);
        memcpy(memory.data(), data, size);
    }

    ~MemoryBinaryIO() {
        memory.clear();
    }

    uint8_t* get_data() {
        return memory.data();
    }

    uint64_t pos = 0;

    void open(const std::string &name) {
        memory.clear();
        pos = 0;
        memory.resize(0);
    }

    void open(uint8_t* data, uint64_t size) {
        pos = 0;
        memory.resize(size);
        memcpy(memory.data(), data, size);
    }

    void close() {
        memory.resize(0);
        memory.clear();
        pos = 0;
    }

    template<typename T>
    void read(T* ptr) {
        read((uint8_t*) ptr, sizeof(T));
        pos = tell();
    }

    template<typename T>
    T read() {
        T tmp;
        read((uint8_t*) &tmp, sizeof(T));
        pos = tell();
        return tmp;
    }

    template<typename T>
    void write(T data) {
        write((uint8_t*) &data, sizeof(T));
        pos = tell();
    }


    void read(uint8_t* buffer, uint64_t count) {
        for (int i = 0; i < count; i++) {
            buffer[i] = memory[pos + i];
        }
        pos += count;
    }

    void write(uint8_t* buffer, uint64_t count) {
        for (int i = 0; i < count; i++) {
            memory.push_back(buffer[i]);
        }
        pos = memory.size();
    }

    uint64_t size() { return memory.size(); }

    uint64_t tell() { return pos; }

    void seek(uint64_t offset, std::ios_base::seekdir dir = std::ios_base::beg) {
        switch (dir) {
            case (std::ios_base::beg): {
                pos = offset;
                break;
            };
            case (std::ios_base::cur): {
                pos = +offset;
                break;
            };
            case (std::ios_base::end): {
                pos = size() - offset;
                break;
            };
            default:
                break;
        }
    }

    uint8_t read_u8() {
        return read<uint8_t>();
    }

    int8_t read_i8() {
        return read<int8_t>();
    }

    uint16_t read_u16() {
        return read<uint16_t>();
    }

    int16_t read_i16() {
        return read<int16_t>();
    }

    uint32_t read_u32() {
        return read<uint32_t>();
    }

    int32_t read_i32() {
        return read<int32_t>();
    }

    std::string read_string(int len = -1) {
        std::string buffer;
        if (len == -1) {
            char c;
            do {
                c = read<char>();
                buffer += c;
            } while (c != 0);
            buffer.pop_back();
        } else {
            for (int i = 0; i < len; i++) {
                buffer += read<char>();
            }
        }
        return buffer;
    }

    void write_u8(uint8_t value) {
        write<uint8_t>(value);
    }

    void write_i8(int8_t value) {
        write<int8_t>(value);
    }

    void write_u16(uint16_t value) {
        write<uint16_t>(value);
    }

    void write_i16(int16_t value) {
        write<int16_t>(value);
    }

    void write_u32(uint32_t value) {
        write<uint32_t>(value);
    }

    void write_i32(int32_t value) {
        write<int32_t>(value);
    }

};