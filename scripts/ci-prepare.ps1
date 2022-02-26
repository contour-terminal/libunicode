#! /usr/bin/env pwsh

cmake `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_TOOLCHAIN_FILE="${Env:WORKSPACE}\vcpkg\scripts\buildsystems\vcpkg.cmake" `
    -DVCPKG_TARGET_TRIPLET="x64-windows" `
    -DPEDANTIC_COMPILER="ON" `
    -DPEDANTIC_COMPILER_WERROR="ON" `
    -B build `
    -S .
