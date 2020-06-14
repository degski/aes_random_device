
// BSD 3-Clause "Simplified" License
//
// Copyright (c) 2020 degski

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <random>
#include <string>

#include <randen.hpp>

#ifndef SAX_RANDEN_RANDOM_DEVICE_USE_TLS
#    define SAX_RANDEN_RANDOM_DEVICE_USE_TLS 1
#    define SAX_RANDEN_RANDOM_DEVICE_TLS thread_local
#else
#    define SAX_RANDEN_RANDOM_DEVICE_USE_TLS 0
#    define SAX_RANDEN_RANDOM_DEVICE_TLS
#endif

namespace sax {
namespace detail {

struct randen_device {

    static constexpr std::size_t buf_len                   = 4'096;
    static constexpr std::size_t std_rdev_result_type_size = sizeof ( std::random_device::result_type );

    using randen_state = RandenState;
    using result_type  = std::uint64_t;

    randen_device ( ) {
        std::random_device std_dev;
        unsigned char seed[ RANDEN_SEED_BYTES ];
        unsigned char * p = seed;
        for ( std::size_t n = ( RANDEN_SEED_BYTES / std_rdev_result_type_size ), i = 0; i < n; ++i ) {
            result_type s = std_dev ( );
            std::memcpy ( p, &s, std_rdev_result_type_size );
            p += std_rdev_result_type_size;
        }
        randen_reseed ( &state, seed );
    }

    [[nodiscard]] result_type operator( ) ( ) const noexcept {
        if ( e == p ) {
            randen_generate ( &state, buf, buf_len );
            p = buf;
        }
        result_type r;
        std::memcpy ( &r, p, sizeof ( result_type ) );
        p += sizeof ( result_type );
        return r;
    }

    void seed ( std::string const & token ) {
        unsigned char seed[ RANDEN_SEED_BYTES ] = { 0 };
        std::memcpy ( seed, token.data ( ), token.length ( ) );
        randen_reseed ( &state, seed );
    }

    alignas ( 64 ) mutable randen_state state;
    alignas ( 64 ) mutable unsigned char buf[ buf_len ];
    mutable unsigned char *e = buf + buf_len, *p = e;
};
} // namespace detail

class ran_random_device {

    detail::randen_device & thread_local_device ( ) const noexcept {
        static SAX_RANDEN_RANDOM_DEVICE_TLS detail::randen_device device;
        return device;
    }

    public:
    using result_type = detail::randen_device::result_type;

    ran_random_device ( ) = default;
    explicit ran_random_device ( std::string const & token ) { thread_local_device ( ).seed ( token ); }
    ran_random_device ( ran_random_device const & ) = delete;

    ran_random_device & operator= ( ran_random_device && ) = delete;
    ran_random_device & operator= ( ran_random_device const & ) = delete;

    [[nodiscard]] result_type operator( ) ( ) const noexcept { return thread_local_device ( ) ( ); }

    [[nodiscard]] constexpr double entropy ( ) const noexcept { return std::numeric_limits<result_type>::digits; }
    [[nodiscard]] static constexpr result_type min ( ) { return result_type{ 0x0000'0000'0000'0000 }; }
    [[nodiscard]] static constexpr result_type max ( ) { return result_type{ 0xFFFF'FFFF'FFFF'FFFF }; }
};

} // namespace sax
