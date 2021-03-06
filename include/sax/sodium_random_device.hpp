
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

#ifdef SAX_AES_RANDOM_DEVICE_USE_TLS
#    define SAX_AES_RANDOM_DEVICE_USE_TLS 1
#    define SAX_AES_RANDOM_DEVICE_TLS thread_local
#else
#    define SAX_AES_RANDOM_DEVICE_USE_TLS 0
#    define SAX_AES_RANDOM_DEVICE_TLS
#endif

namespace sax {
namespace detail {

struct sodium_device {

    static constexpr std::size_t buf_len                   = 4'096;
    static constexpr std::size_t std_rdev_result_type_size = sizeof ( std::random_device::result_type );

    using result_type = std::uint64_t;

    sodium_device ( ) {
        // sodium_init ( );
        std::random_device std_dev;
        unsigned char seed[ AES_STREAM_SEEDBYTES ];
        unsigned char * p_ = seed;
        for ( std::size_t n = ( AES_STREAM_SEEDBYTES / std_rdev_result_type_size ), i = 0; i < n; ++i ) {
            result_type s = std_dev ( );
            std::memcpy ( p_, &s, std_rdev_result_type_size );
            p_ += std_rdev_result_type_size;
        }
        aes_stream_init ( &state, seed );
    }

    [[nodiscard]] result_type operator( ) ( ) const noexcept {
        if ( e == p ) {
            aes_stream ( &state, buf, buf_len );
            p = buf;
            perturbe ( state );
        }
        result_type r;
        std::memcpy ( &r, p, sizeof ( result_type ) );
        p += sizeof ( result_type );
        return r;
    }

    void seed ( std::string const & token ) {
        assert ( token.length ( ) );
        unsigned char seed[ AES_STREAM_SEEDBYTES ] = { 0 };
        std::memcpy ( seed, token.data ( ), token.length ( ) );
        aes_stream_init ( &state, seed );
    }

    // In 5% of all buffer-refills, the generation of PRN's will be followed by a random swap of 2 bytes in the state.
    // The source of entropy is the bits in the buffer.
    void perturbe ( aes_stream_state & state_ ) const noexcept {
        std::uint32_t seed;
        std::memcpy ( &seed, p, 4 );
        std::minstd_rand prng{ seed };
        if ( std::bernoulli_distribution{ 0.05 }( prng ) ) {
            std::uniform_int_distribution<std::size_t> sdis{ 0, ( sizeof ( aes_stream_state ) - 1 ) };
            std::size_t const s = sdis ( prng );
            std::size_t d       = sdis ( prng );
            while ( s == d )
                d = sdis ( prng );
            std::swap ( state.opaque[ s ], state.opaque[ d ] );
        }
    }

    alignas ( 64 ) mutable aes_stream_state state;
    alignas ( 64 ) mutable unsigned char buf[ buf_len ];
    mutable unsigned char *e = buf + buf_len, *p = e;
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
