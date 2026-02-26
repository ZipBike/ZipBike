cmake_minimum_required(VERSION 4.1)
project(ZipBike)

set(CMAKE_CXX_STANDARD 14)

include_directories(cli)
include_directories(core)

add_executable(ZipBike
        cli/cli_parser.cpp
        cli/cli_parser.h
        cli/main.cpp
        core/bitstream.cpp
        core/bitstream.h
        core/compressor.cpp
        core/compressor.h
        core/container.cpp
        core/container.h
        core/huffman.cpp
        core/huffman.h
        core/lz77.cpp
        core/lz77.h
        core/rle.cpp
        core/rle.h)
