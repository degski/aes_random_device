
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
#include "include/sax/randen_random_device.hpp"

int main ( ) {

    constexpr std::uint64_t N = 2'048'000;

    {
        sax::aes_random_device aes_rng;

        std::uint64_t r = 0;

        plf::nanotimer t;

        for ( std::uint64_t i = 0; i < 10'000; ++i )
            r += aes_rng ( );

        t.start ( );

        for ( std::uint64_t i = 0; i < N; ++i )
            r += aes_rng ( );

        std::uint64_t d = t.get_elapsed_ns ( ) * ( 1 / ( N * 0.001 ) );

        std::cout << "result sax::aes_random_device " << r << " in " << ( d / 1'000 ) << '.' << ( d % 1'000 ) << " ns/rn\n";
    }

    {
        sax::aes_random_device aes_rng;
        sax::sfc64 sfc_rng ( aes_rng ( ), aes_rng ( ), aes_rng ( ), aes_rng ( ) );

        std::uint64_t r = 0;

        plf::nanotimer t;

        for ( std::uint64_t i = 0; i < 10'000; ++i )
            r += sfc_rng ( );

        t.start ( );

        for ( std::uint64_t i = 0; i < N; ++i )
            r += sfc_rng ( );

        std::uint64_t d = t.get_elapsed_ns ( ) * ( 1 / ( N * 0.001 ) );

        std::cout << "result sax::sfc               " << r << " in " << ( d / 1'000 ) << '.' << ( d % 1'000 ) << " ns/rn\n";
    }

    {
        sax::aes_random_device aes_rng;
        std::mt19937_64 mt_rng ( aes_rng ( ) );

        std::uint64_t r = 0;

        mt_rng.discard ( 10'000 );

        plf::nanotimer t;

        t.start ( );

        for ( std::uint64_t i = 0; i < N; ++i )
            r += mt_rng ( );

        std::uint64_t d = t.get_elapsed_ns ( ) * ( 1 / ( N * 0.001 ) );

        std::cout << "result std::mt19937_64        " << r << " in " << ( d / 1'000 ) << '.' << ( d % 1'000 ) << " ns/rn\n";
    }

    {
        sax::ran_random_device ran_rng;

        std::uint64_t r = 0;

        plf::nanotimer t;

        for ( std::uint64_t i = 0; i < 10'000; ++i )
            r += ran_rng ( );

        t.start ( );

        for ( std::uint64_t i = 0; i < N; ++i )
            r += ran_rng ( );

        std::uint64_t d = t.get_elapsed_ns ( ) * ( 1 / ( N * 0.001 ) );

        std::cout << "result sax::ran_random_device " << r << " in " << ( d / 1'000 ) << '.' << ( d % 1'000 ) << " ns/rn\n";
    }

    return EXIT_SUCCESS;
}
