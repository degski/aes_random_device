# aes_random_device

cryptographically-secure stl-style `random_device`, a wrapper around `aes-stream` [(Frank Denis)](https://github.com/jedisct1/aes-stream) and `libsodium` [(Frank Denis)](https://github.com/jedisct1/libsodium)

prng is default-fully-seeded with `std::random_device`.

### limitations

* requires a modern Intel or AMD CPU with AES-NI support
* aes_random_device() can be called indefinitely without having to reseed the generator

### concurrency

* thread-safe, using tls, define SAX_AES_RANDOM_DEVICE_USE_TLS=0 to disable, tls is the default

### dependencies

* [libsodium](https://doc.libsodium.org/)
* [aes-stream](https://github.com/jedisct1/aes-stream)
* C++17 and a corresponding STL-library

### license

* BSD 3-Clause "Simplified" License
