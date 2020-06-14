
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
// #include "include/sax/randen_random_device.hpp"
#include <randen.hpp>

#include <sodium.h>

// void crypto_stream_xchacha20_keygen ( unsigned char k[ crypto_stream_xchacha20_KEYBYTES ] );
// int crypto_stream_xchacha20(unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *k);

// int crypto_stream_xchacha20_xor ( unsigned char * c, const unsigned char * m, unsigned long long mlen, const unsigned char * n,
//                                  const unsigned char * k );

int main ( ) {

    unsigned char message[] = { 'd', 'e', 'g', 's', 'k', 'i', '@', 'g', 'm', 'a', 'i', 'l', '.', 'c', 'o', 'm' };
    unsigned char encrypt[ 1'024 ];

    sax::aes_random_device aes_rng;

    uint64_t initial_counter = aes_rng ( );

    unsigned char key[ crypto_stream_xchacha20_KEYBYTES ];
    crypto_stream_xchacha20_keygen ( key );

    unsigned char nounce[ crypto_stream_xchacha20_NONCEBYTES ] = { };

    crypto_stream_xchacha20_xor_ic ( encrypt, message, sizeof ( message ), nounce, aes_rng ( ), key );

    /*

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
        randen::Randen<std::uint64_t> ran_rng;

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

    */

    return EXIT_SUCCESS;
}
