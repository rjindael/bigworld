// SPDX-License-Identifier: GPL-2.0-or-later
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int io_file_exists(const char* path)
{
    return access(path, F_OK) == 0;
}

// resolve file path by searching in multiple directories
char* io_resolve_path(const char* relative_path)
{
    const char* search_paths[] = { "", "../", "../../", NULL };
    static char full_path[512];

    for (int i = 0; search_paths[i] != NULL; i++) {
        snprintf(full_path, sizeof(full_path), "%s%s", search_paths[i], relative_path);

        if (io_file_exists(full_path)) {
            return full_path;
        }
    }

    return NULL;
}

char* io_read_file(const char* filepath)
{
    char* resolved_path = io_resolve_path(filepath);

    if (!resolved_path) {
        fprintf(stderr, "Could not find file: %s\n", filepath);
        return NULL;
    }

    FILE* file = fopen(resolved_path, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", resolved_path);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // allocate memory and read
    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Failed to allocate memory for file\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);

    printf("Loaded file: %s\n", resolved_path);

    return buffer;
}