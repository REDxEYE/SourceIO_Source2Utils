//
// Created by i.getsman on 30.03.2020.
//

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <texture_decompressor.h>

#include <compressed_index_buffer.h>
#include <compressed_vertex_buffer.h>

#include <lz4/lib/lz4.h>

using namespace pybind11::literals;

PYBIND11_MODULE(PySourceIOUtils, m) {
    m.doc() = "SourceIO decompressor library"; // optional module docstring

    m.def("read_r8g8b8a8", [](const char* buffer, uint32_t width, uint32_t height, bool flip) {
              auto bytes = PyBytes_FromStringAndSize(nullptr, width * height * 4);
              auto bytes_raw = PyBytes_AsString(bytes);

              if (!read_r8g8b8a8(buffer, bytes_raw, width, height)) {
                  return pybind11::cast<pybind11::bytes>(pybind11::none());
              }

              if (flip) {
                  flip_image((uint32_t*) bytes_raw, width, height);
              }

              return pybind11::cast<pybind11::bytes>(bytes);
          }, pybind11::return_value_policy::automatic,
          "Read RGBA8888 image",
          "byte_buffer"_a, "width"_a, "height"_a, "flip"_a);

    m.def("read_bc7", [](const char* buffer, uint32_t width, uint32_t height, bool hemi_ocr_rb, bool flip) {
              auto bytes = PyBytes_FromStringAndSize(nullptr, width * height * 4);
              auto bytes_raw = PyBytes_AsString(bytes);

              if (!read_bc7(buffer, bytes_raw, width, height, hemi_ocr_rb)) {
                  return pybind11::cast<pybind11::bytes>(pybind11::none());
              }

              if (flip) {
                  flip_image((uint32_t*) bytes_raw, width, height);
              }

              return pybind11::cast<pybind11::bytes>(bytes);
          }, pybind11::return_value_policy::automatic,
          "Read BC7 (BPTC) image",
          "byte_buffer"_a, "width"_a, "height"_a, "hemi_oct_rb"_a, "flip"_a);

    m.def("read_ati1n", [](const char* buffer, uint32_t width, uint32_t height, bool flip) {
              auto bytes = PyBytes_FromStringAndSize(nullptr, width * height * 4);
              auto bytes_raw = PyBytes_AsString(bytes);

              if (!read_ATI1N(buffer, bytes_raw, width, height)) {
                  return pybind11::cast<pybind11::bytes>(pybind11::none());
              }

              if (flip) {
                  flip_image((uint32_t*) bytes_raw, width, height);
              }

              return pybind11::cast<pybind11::bytes>(bytes);
          }, pybind11::return_value_policy::automatic,
          "Read ATI1N (RGTC1, BC4) image",
          "byte_buffer"_a, "width"_a, "height"_a, "flip"_a);

    m.def("read_ati2n", [](const char* buffer, uint32_t width, uint32_t height, bool flip) {
              auto bytes = PyBytes_FromStringAndSize(nullptr, width * height * 4);
              auto bytes_raw = PyBytes_AsString(bytes);

              if (!read_ATI2N(buffer, bytes_raw, width, height)) {
                  return pybind11::cast<pybind11::bytes>(pybind11::none());
              }

              if (flip) {
                  flip_image((uint32_t*) bytes_raw, width, height);
              }

              return pybind11::cast<pybind11::bytes>(bytes);
          }, pybind11::return_value_policy::automatic,
          "Read ATI2N (RGTC2, BC5) image",
          "byte_buffer"_a, "width"_a, "height"_a, "flip"_a);

    m.def("read_dxt1", [](const char* buffer, uint32_t width, uint32_t height, bool flip) {
              auto bytes = PyBytes_FromStringAndSize(nullptr, width * height * 4);
              auto bytes_raw = PyBytes_AsString(bytes);

              if (!read_DXT1(buffer, bytes_raw, width, height)) {
                  return pybind11::cast<pybind11::bytes>(pybind11::none());
              }

              if (flip) {
                  flip_image((uint32_t*) bytes_raw, width, height);
              }

              return pybind11::cast<pybind11::bytes>(bytes);
          }, pybind11::return_value_policy::automatic,
          "Read dtx1 (bc1) image",
          "byte_buffer"_a, "width"_a, "height"_a, "flip"_a);

    m.def("read_dxt5", [](const char* buffer, uint32_t width, uint32_t height, bool flip) {
              auto bytes = PyBytes_FromStringAndSize(nullptr, width * height * 4);
              auto bytes_raw = PyBytes_AsString(bytes);

              if (!read_DXT5(buffer, bytes_raw, width, height)) {
                  return pybind11::cast<pybind11::bytes>(pybind11::none());
              }

              if (flip) {
                  flip_image((uint32_t*) bytes_raw, width, height);
              }

              return pybind11::cast<pybind11::bytes>(bytes);
          }, pybind11::return_value_policy::automatic,
          "Read dtx5 (bc3) image",
          "byte_buffer"_a, "width"_a, "height"_a, "flip"_a);

    m.def("lz4_decompress", [](const char* buffer, uint64_t compressed_size, uint64_t decompressed_size) {
              auto out_buffer = new char[decompressed_size];
              auto decompressed_actually = LZ4_decompress_safe(buffer, out_buffer, compressed_size, decompressed_size);
              if (decompressed_actually == decompressed_size) {
                  auto bytes = pybind11::bytes(out_buffer, decompressed_size);
                  free(out_buffer);
                  return bytes;
              } else {
                  free(out_buffer);
                  return pybind11::bytes();
              }
          }, pybind11::return_value_policy::automatic,
          "lz4 decompress",
          "compressed_data"_a, "compressed_size"_a, "decompressed_size"_a);

    m.def("decode_vertex_buffer", [](const char* data, uint64_t data_size, uint64_t size, uint64_t count) {
              std::vector<uint8_t> buffer;
              buffer.resize(data_size);
              memcpy(buffer.data(), data, data_size);
              std::vector<uint8_t> decoded = MeshOptimizerVertexDecoder::DecodeVertexBuffer(count, size, buffer);
              auto bytes = PyBytes_FromStringAndSize(nullptr, decoded.size() * sizeof(decoded.front()));
              memcpy(PyBytes_AsString(bytes), decoded.data(), decoded.size() * sizeof(decoded.front()));
              return pybind11::cast<pybind11::bytes>(bytes);
          }, pybind11::return_value_policy::automatic,
          "decode compressed vertex buffer", "data"_a, "data_size"_a, "size"_a, "count"_a);

    m.def("decode_index_buffer", [](const char* data, uint64_t data_size, uint64_t size, uint64_t count) {
              std::vector<uint8_t> buffer;
              buffer.resize(data_size);
              memcpy(buffer.data(), data, data_size);
              std::vector<uint8_t> decoded = MeshOptimizerIndexDecoder::DecodeIndexBuffer(count, size, buffer);
              auto bytes = PyBytes_FromStringAndSize(nullptr, decoded.size() * sizeof(decoded.front()));
              memcpy(PyBytes_AsString(bytes), decoded.data(), decoded.size() * sizeof(decoded.front()));
              return pybind11::cast<pybind11::bytes>(bytes);
          }, pybind11::return_value_policy::automatic,
          "decode compressed index buffer", "data"_a, "data_size"_a, "size"_a, "count"_a);

    m.def("extract_alpha", [](const char* buffer, uint64_t size, float threshold) {
              pybind11::array_t<float> color{size};
              std::optional<pybind11::array_t<float>> alpha{};

              for (auto i = 0; i < size; i += 4) {
                  float r = (float) (uint8_t) buffer[i + 0] / 255.0f;
                  float g = (float) (uint8_t) buffer[i + 1] / 255.0f;
                  float b = (float) (uint8_t) buffer[i + 2] / 255.0f;
                  float a = (float) (uint8_t) buffer[i + 3] / 255.0f;

                  color.mutable_at(i + 0) = r;
                  color.mutable_at(i + 1) = g;
                  color.mutable_at(i + 2) = b;
                  color.mutable_at(i + 3) = 1.0f;

                  if (a < threshold) {
                      if (!alpha.has_value()) {
                          alpha = pybind11::array_t<float>(size);

                          for (auto x = 0; x < i - 4; x += 4) {
                              alpha->mutable_at(x + 0) = (float) (uint8_t) buffer[x + 3] / 255.0f;
                              alpha->mutable_at(x + 1) = (float) (uint8_t) buffer[x + 3] / 255.0f;
                              alpha->mutable_at(x + 2) = (float) (uint8_t) buffer[x + 3] / 255.0f;
                              alpha->mutable_at(x + 3) = 1.0f;
                          }
                      }

                      alpha->mutable_at(i + 0) = a;
                      alpha->mutable_at(i + 1) = a;
                      alpha->mutable_at(i + 2) = a;
                      alpha->mutable_at(i + 3) = 1.0f;
                  }
              }
              if (alpha.has_value()) {
                  return pybind11::make_tuple(color, *alpha);
              } else {
                  return pybind11::make_tuple(color, pybind11::none());
              }
          }, pybind11::return_value_policy::automatic,
          "Separate RGB and A",
          "buffer"_a, "size"_a, "threshold"_a = 0.98f);
}
