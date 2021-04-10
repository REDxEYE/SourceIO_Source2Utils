cmake_minimum_required(VERSION 3.15)
project(SourceIOUtils)

set(python_version 3.7)

set(CMAKE_CXX_STANDARD 20)
if (UNIX)
    set(PYTHON_MODULE_EXTENSION ".so")
endif (UNIX)
if (WIN32)
    set(PYTHON_MODULE_EXTENSION ".pyd")
endif (WIN32)
set(PYBIND11_PYTHON_VERSION ${python_version})
find_package(Python3 ${python_version})
#set(PYTHON_INCLUDE_DIR "D:/PROGRAMS/Python${python_version}/include")
#set(PYTHON_LIBRARY "D:/PROGRAMS/Python${python_version}/python*.dll")

find_package(Python3 COMPONENTS Development)


add_subdirectory(libs/pybind11)

include(additional_cmake/detex/CmakeLists.txt)
include(additional_cmake/lz4/CmakeLists.txt)

include_directories(libs)

add_library(PySourceIOUtils SHARED src/texture_decompressor.cpp src/bindings.cpp src/compressed_formats.cpp src/compressed_vertex_buffer.cpp src/compressed_index_buffer.cpp)
add_library(SourceIOUtils SHARED src/texture_decompressor.cpp src/compressed_formats.cpp src/compressed_vertex_buffer.cpp src/compressed_index_buffer.cpp)
add_executable(SourceIOUtilsTest src/main.cpp src/texture_decompressor.cpp src/compressed_formats.cpp src/compressed_vertex_buffer.cpp src/compressed_index_buffer.cpp)

target_include_directories(PySourceIOUtils PUBLIC include/)
target_include_directories(SourceIOUtils PUBLIC include/)
target_include_directories(SourceIOUtilsTest PUBLIC include/)

set_target_properties(PySourceIOUtils PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
        SUFFIX "${PYTHON_MODULE_EXTENSION}")

target_compile_options(PySourceIOUtils PRIVATE -O3 -fPIC)
target_compile_options(SourceIOUtils PRIVATE -O3 -fPIC)
target_compile_options(SourceIOUtilsTest PRIVATE -O3 -fPIC)
if (WIN32)
    target_link_libraries(SourceIOUtils PUBLIC Detex lz4 -static-libgcc -static-libstdc++
            -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)

    target_link_libraries(PySourceIOUtils PRIVATE pybind11::module PUBLIC Detex lz4 -static-libgcc -static-libstdc++
            -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)

    target_link_libraries(SourceIOUtilsTest PUBLIC Detex lz4 -static-libgcc -static-libstdc++
            -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)

endif (WIN32)
if (UNIX)
    target_link_libraries(SourceIOUtils PUBLIC -Bstatic Detex lz4 -Wl,-Bdynamic)

    target_link_libraries(PySourceIOUtils PRIVATE pybind11::module PUBLIC -Bstatic Detex lz4 -Bdynamic -static-libgcc)

    target_link_libraries(SourceIOUtilsTest PUBLIC -Bstatic Detex lz4 -Bdynamic)

endif (UNIX)