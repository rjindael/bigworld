#ifndef IO_H
#define IO_H

// OPERATIONS //

int io_file_exists(const char* path);
char* io_resolve_path(const char* relative_path);
char* io_read_file(const char* filepath);

#endif // IO_H
