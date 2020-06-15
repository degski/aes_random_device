
// BSD 3-Clause "Simplified" License
//
// Copyright (c) 2020 degski

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <random>
#include <string>

#include "sodium_random_device/aes-stream.hpp" // ensure aes-stream is compiled with the same parameters!

extern "C" int sodium_init ( void );

#ifndef SAX_AES_RANDOM_DEVICE_USE_TLS
#    define SAX_AES_RANDOM_DEVICE_USE_TLS 1
#    define SAX_AES_RANDOM_DEVICE_TLS thread_local
#else
#    define SAX_AES_RANDOM_DEVICE_USE_TLS 0
#    define SAX_AES_RANDOM_DEVICE_TLS
#endif

namespace sax {
namespace detail {

struct sodium_device {

    static constexpr std::size_t buf_len                   = 8'192;
    static constexpr std::size_t std_rdev_result_type_size = sizeof ( std::random_device::result_type );

    using result_type = std::uint64_t;

    sodium_device ( ) {
        sodium_init ( );
        result_type seed[ ( AES_STREAM_SEEDBYTES / sizeof ( result_type ) ) ];
        for ( auto it = std::begin ( seed ), end = it + ( AES_STREAM_SEEDBYTES / sizeof ( result_type ) ); it != end; ++it )
            *it = std_dev ( );
        aes_stream_init ( &state, reinterpret_cast<unsigned char *> ( seed ) );
    }

    [[nodiscard]] result_type operator( ) ( ) const noexcept {
        if ( e == p ) {
            aes_stream ( &state, reinterpret_cast<unsigned char *> ( buf ), buf_len );
            p = buf;
            perturbe ( state );
        }
        return *p++;
    }

    void seed ( std::string const & token ) {
        assert ( token.length ( ) );
        unsigned char seed[ AES_STREAM_SEEDBYTES ] = { 0 };
        std::memcpy ( seed, token.data ( ), token.length ( ) );
        aes_stream_init ( &state, seed );
    }

    void perturbe ( aes_stream_state & state_ ) const noexcept {
        std::minstd_rand prng{ static_cast<std::uint32_t> ( p[ 0 ] ) };
        if ( std::bernoulli_distribution{ 0.05 }( prng ) ) {
            std::uniform_int_distribution<std::size_t> sdis{ 0, ( sizeof ( aes_stream_state ) - 1 ) };
            std::size_t const s = sdis ( prng ), d = sdis ( prng );
            if ( s != d )
                std::swap ( state.opaque[ s ], state.opaque[ d ] );
        }
    }

    alignas ( 64 ) mutable aes_stream_state state;
    alignas ( 64 ) mutable result_type buf[ ( buf_len / ( sizeof ( result_type ) ) ) ];
    mutable result_type *e = buf + ( buf_len / ( sizeof ( result_type ) ) ), *p = e;
    mutable std::random_device std_dev;
};
} // namespace detail

class aes_random_device {

    detail::sodium_device & thread_local_device ( ) const noexcept {
        static SAX_AES_RANDOM_DEVICE_TLS detail::sodium_device device;
        return device;
    }

    public:
    using result_type = detail::sodium_device::result_type;

    aes_random_device ( ) = default;
    explicit aes_random_device ( std::string const & token ) { thread_local_device ( ).seed ( token ); }
    aes_random_device ( aes_random_device const & ) = delete;

    aes_random_device & operator= ( aes_random_device && ) = delete;
    aes_random_device & operator= ( aes_random_device const & ) = delete;

    [[nodiscard]] result_type operator( ) ( ) const noexcept { return thread_local_device ( ) ( ); }

    [[nodiscard]] constexpr double entropy ( ) const noexcept { return std::numeric_limits<result_type>::digits; }
    [[nodiscard]] static constexpr result_type min ( ) { return result_type{ 0x0000'0000'0000'0000 }; }
    [[nodiscard]] static constexpr result_type max ( ) { return result_type{ 0xFFFF'FFFF'FFFF'FFFF }; }
};
} // namespace sax
