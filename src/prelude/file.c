// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/buffer.c"
#include "prelude/expect.c"
#include "prelude/scalar.c"
#include "prelude/split.c"
#include "prelude/string.c"

#include <stdio.h>

/* Size of chunks the files are read by. */
#define CT_FILE_CHUNK 1024

/* Load the contents of the file at the path to the buffer. Returns a view to
 * the contents of the file. */
CTString ct_file_load(CTBuffer* buffer, char const* path)
{
    FILE* file = fopen(path, "r");
    ct_expect(file != NULL, "Could not open the file!");

    CTIndex begining = ct_buffer_size(buffer);
    CTIndex written  = 0;

    do {
        ct_buffer_reserve(buffer, CT_FILE_CHUNK);
        written = (CTIndex)fread(buffer->last, 1, CT_FILE_CHUNK, file);
        buffer->last += written;
    } while (written == CT_FILE_CHUNK);

    ct_expect(feof(file) != 0, "Problem while reading file!");
    ct_expect(fclose(file) != -1, "Could not close the file!");

    CTString whole = ct_buffer_view(buffer);
    return ct_split(&whole, begining).after;
}
