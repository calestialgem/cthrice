# SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

function(setup_target target)
    target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src)

    # Enable all warnings.
    if(MSVC)
        target_compile_options(${target} PRIVATE /Wall)
    else(MSVC)
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic
        -Wno-pragma-once-outside-header -Wno-empty-translation-unit)
    endif(MSVC)
endfunction(setup_target target)

project(cthrice)

add_executable(${PROJECT_NAME} src/main.c)
setup_target(${PROJECT_NAME})

# Create compile commands for the header files as well.
add_library(headers OBJECT
    src/prelude/buffer.c
    src/prelude/expect.c
    src/prelude/file.c
    src/prelude/scalar.c
    src/prelude/split.c
    src/prelude/string.c

    src/patlak/code.c
    src/patlak/context.c
    src/patlak/decode.c
    src/patlak/lexer.c
    src/patlak/pattern.c
    src/patlak/printer.c
    src/patlak/state.c
    src/patlak/token.c
)
setup_target(headers)
