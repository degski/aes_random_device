
// BSD 3-Clause "Simplified" License
//
// Copyright (c) 2020 degski

/*
    C:\Program Files\LLVM\lib\clang\10.0.0\lib\windows\clang_rt.asan_cxx-x86_64.lib
    C:\Program Files\LLVM\lib\clang\10.0.0\lib\windows\clang_rt.asan-preinit-x86_64.lib
    C:\Program Files\LLVM\lib\clang\10.0.0\lib\windows\clang_rt.asan-x86_64.lib
*/

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <array>
#include <iostream>

#ifdef NDEBUG
#    define USE_AES_STREAM_LTO 1
#else
#    define USE_AES_STREAM_LTO 0
#endif

#include "include/sax/sodium_random_device.hpp"
#include "include/sax/sodium_random_device/machine_guid.hpp"

int main ( ) {

    std::cout << sax::machine_guid ( ) << '\n';

    sax::aes_random_device aes_rng;

    for ( int i = 0; i < 8'384; ++i )
        std::cout << aes_rng ( ) << ' ';
    std::cout << '\n';

    return EXIT_SUCCESS;
}
