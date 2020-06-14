
// BSD 3-Clause "Simplified" License
//
// Copyright (c) 2020 degski

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <iostream>
#include <random>

#include <plf/plf_nanotimer.h>
#include <sax/sfc.hpp>

#ifdef NDEBUG
#    define USE_AES_STREAM_LTO 1
#else
#    define USE_AES_STREAM_LTO 0
#endif

#include "include/sax/sodium_random_device.hpp"

int main ( ) {

    constexpr int N = 1'024'000;

    {
        sax::aes_random_device aes_rng;

        std::uint64_t r = 0;

        plf::nanotimer t;

        t.start ( );

        for ( int i = 0; i < N; ++i )
            r += aes_rng ( );

        std::uint64_t d = t.get_elapsed_ns ( ) * ( 1 / ( N * 0.001 ) );

        std::cout << "result sax::aes_random_device " << r << " in " << ( d / 1'000 ) << '.' << ( d % 1'000 ) << " ns/rn\n";
    }

    {
        sax::sfc64 sfc_rng;

        std::uint64_t r = 0;

        plf::nanotimer t;

        t.start ( );

        for ( int i = 0; i < N; ++i )
            r += sfc_rng ( );

        std::uint64_t d = t.get_elapsed_ns ( ) * ( 1 / ( N * 0.001 ) );

        std::cout << "result sax::sfc               " << r << " in " << ( d / 1'000 ) << '.' << ( d % 1'000 ) << " ns/rn\n";
    }

    {
        std::mt19937_64 mt_rng;

        std::uint64_t r = 0;

        plf::nanotimer t;

        t.start ( );

        for ( int i = 0; i < N; ++i )
            r += mt_rng ( );

        std::uint64_t d = t.get_elapsed_ns ( ) * ( 1 / ( N * 0.001 ) );

        std::cout << "result std::mt19937_64        " << r << " in " << ( d / 1'000 ) << '.' << ( d % 1'000 ) << " ns/rn\n";
    }

    return EXIT_SUCCESS;
}
