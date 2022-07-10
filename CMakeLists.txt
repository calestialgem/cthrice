# SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(cthrice)

add_executable(${PROJECT_NAME} src/main.cc)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_precompile_headers(${PROJECT_NAME} PRIVATE
    src/prelude/error.cc
    src/prelude/types.cc
    <cmath>
    <cstddef>
    <cstdlib>
    <cstdint>
    <cstdio>
    <cstring>
    <algorithm>
    <exception>
)

# Enable all warnings.
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Wall)
else(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-deprecated-declarations -Wno-gnu-anonymous-struct -Wno-nested-anon-types)
endif(MSVC)
