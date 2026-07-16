cmake_minimum_required(VERSION 3.20)
project(ZipBike CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(zb
        cli/cli_parser.cpp
        cli/cli_parser.h
        cli/main.cpp
        core/bitstream.cpp
        core/bitstream.h
        core/compressor.cpp
        core/compressor.h
        core/container.cpp
        core/container.h
        core/crc32.h
        core/filehandler.cpp
        core/filehandler.h
        core/huffman.cpp
        core/huffman.h
        core/lz77.cpp
        core/lz77.h
        core/rle.cpp
        core/rle.h)

target_include_directories(zb PRIVATE cli core)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(zb PRIVATE stdc++fs)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(zb PRIVATE c++fs)
endif()

if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(zb PRIVATE -static)
endif()
