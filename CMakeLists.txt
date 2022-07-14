# SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(cthrice)

add_executable(${PROJECT_NAME} src/main.cc)

# Create compile commands for the header files as well.
add_library(headers
    src/prelude/expect.cc
    src/prelude/range.cc
    src/prelude/scalar.cc
    src/prelude/view.cc
)

function(setup_target target)
    target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src)

    # Enable all warnings.
    if(MSVC)
        target_compile_options(${target} PRIVATE /Wall)
    else(MSVC)
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic
        -Wno-pragma-once-outside-header)
    endif(MSVC)
endfunction(setup_target target)

setup_target(${PROJECT_NAME})
setup_target(headers)
