
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

using chacha20_key_type   = std::array<std::uint64_t, crypto_stream_chacha20_KEYBYTES / 8>;
using chacha20_nonce_type = std::array<std::uint8_t, crypto_stream_chacha20_NONCEBYTES>;

template<std::size_t BufferSize>
using chacha20_stream_type = std::array<std::uint64_t, BufferSize / 8>;

template<typename StreamType>
[[nodiscard]] StreamType chacha20_stream_rekey ( ) noexcept {
    StreamType stream;
    crypto_stream_chacha20_keygen ( reinterpret_cast<unsigned char *> ( stream.data ( ) ) );
    return stream;
}

int main ( ) {

    // static constexpr std::size_t

    constexpr std::uint64_t N = 2'048'000 / ( 8'192 / 8 );

    chacha20_stream_type<8'192> stream = chacha20_stream_rekey<chacha20_stream_type<8'192>> ( );

    unsigned char k[ 32 ] = { 'd', 'e', 'g', 's', 'k', 'i', '@', 'g', 'm', 'a', 'i', 'l', '.', 'c', 'o', 'm',
                              'd', 'e', 'g', 's', 'k', 'i', '@', 'g', 'm', 'a', 'i', 'l', '.', 'c', 'o', 'm' };

    chacha20_key_type key;

    std::memcpy ( key.data ( ), k, key.size ( ) / 2 );
    std::memcpy ( key.data ( ) + key.size ( ) / 2, k + key.size ( ) / 2, key.size ( ) / 2 );

    chacha20_nonce_type nonce = { '*', 'd', 'e', 'g', 's', 'k', 'i', '*' };

    std::uint64_t c = N, r = 0, *e = stream.data ( ) + stream.size ( ), *p = e;

    plf::nanotimer t;
    t.start ( );

    while ( c-- ) {
        while ( e != p )
            r += *p++;
        crypto_stream_chacha20 ( reinterpret_cast<unsigned char *> ( stream.data ( ) ), stream.size ( ) * 8, nonce.data ( ),
                                 reinterpret_cast<unsigned char *> ( key.data ( ) ) );
        p = stream.data ( );
    }

    std::uint64_t d = t.get_elapsed_ns ( ) * ( 1 / ( N * ( ( 8'192 / 8 ) * 0.001 ) ) );

    std::cout << "result sax::chacha20_random_device " << r << " in " << ( d / 1'000 ) << '.' << ( d % 1'000 ) << " ns/rn\n";

    /*

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
