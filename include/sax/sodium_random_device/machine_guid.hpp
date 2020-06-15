
// BSD 3-Clause "Simplified" License
//
// Copyright (c) 2020 degski

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#if ( UINTPTR_MAX == 0xFFFF'FFFF'FFFF'FFFF )
#    define _AMD64_
#else
#    define _X86_
#endif

#ifdef _WIN32
#    ifndef WINDOWS_LEAN_AND_MEAN
#        define WINDOWS_LEAN_AND_MEAN
#        include <winreg.h>
#        include "utf8conv.hpp"
#    endif
#else // nix

#endif

#include <random>
#include <string>

#pragma once

namespace sax {

#ifdef _WIN32
std::string machine_guid ( ) {
    std::wstring key  = L"SOFTWARE\\Microsoft\\Cryptography";
    std::wstring name = L"MachineGuid";
    HKEY hKey;
    if ( RegOpenKeyEx ( HKEY_LOCAL_MACHINE, key.c_str ( ), 0, KEY_READ | KEY_WOW64_64KEY, &hKey ) )
        throw std::runtime_error ( "Could not open registry key" );
    DWORD type;
    DWORD cbData;
    if ( RegQueryValueEx ( hKey, name.c_str ( ), NULL, &type, NULL, &cbData ) ) {
        RegCloseKey ( hKey );
        throw std::runtime_error ( "Could not read registry value" );
    }
    if ( type != REG_SZ ) {
        RegCloseKey ( hKey );
        throw "Incorrect registry value type";
    }
    std::wstring value ( cbData / sizeof ( wchar_t ), L'\0' );
    if ( RegQueryValueEx ( hKey, name.c_str ( ), NULL, NULL, reinterpret_cast<LPBYTE> ( &value[ 0 ] ), &cbData ) ) {
        RegCloseKey ( hKey );
        throw "Could not read registry value";
    }
    RegCloseKey ( hKey );
    return utf16_to_utf8 ( value );
}
#else

#endif

} // namespace sax
