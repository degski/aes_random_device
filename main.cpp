
// BSD 3-Clause "Simplified" License
//
// Copyright (c) 2020 degski

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <array>
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

using crypto_stream_chacha20_ietf_key_type    = std::array<unsigned char, crypto_stream_chacha20_ietf_KEYBYTES>;
using crypto_stream_chacha20_ietf_nonce_type  = std::array<unsigned char, crypto_stream_chacha20_NONCEBYTES>;
using crypto_stream_chacha20_ietf_stream_type = std::array<unsigned char, 4'096>;

crypto_stream_chacha20_ietf_key_type crypto_stream_chacha20_ietf_rekey ( ) noexcept {
    crypto_stream_chacha20_ietf_key_type k;
    crypto_stream_chacha20_ietf_keygen ( k.data ( ) );
    return k;
}

int main ( ) {

    crypto_stream_chacha20_ietf_stream_type stream;

    crypto_stream_chacha20_ietf_key_type key     = crypto_stream_chacha20_ietf_rekey ( );
    crypto_stream_chacha20_ietf_nonce_type nonce = { '*', 'd', 'e', 'g', 's', 'k', 'i', '*' };

    crypto_stream_chacha20_ietf ( stream.data ( ), stream.size ( ), nonce.data ( ), key.data ( ) );

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
