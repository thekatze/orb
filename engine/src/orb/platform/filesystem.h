#pragma once

#include "../core/types.h"

typedef struct orb_file_handle {
    void *handle;
    usize file_bytes;
} orb_file_handle;

typedef enum orb_file_mode_flags {
    FILE_MODE_READ_FLAG = 0x01,
    FILE_MODE_WRITE_FLAG = 0x02,
} orb_file_mode_flags;

ORB_API b8 orb_filesystem_file_exists(const char *path);

ORB_API b8 orb_filesystem_file_open(const char *path, orb_file_mode_flags flags, b8 binary,
                                    orb_file_handle *out_handle);
ORB_API void orb_filesystem_file_close(orb_file_handle *handle);

// reads all text into out_text buffer. out_text must be at least handle->file_bytes big.
ORB_API b8 orb_filesystem_file_read_all_text(orb_file_handle *handle, char *out_text);

// reads all bytes into out_bytes buffer. out_bytes must be at least handle->file_bytes big.
ORB_API b8 orb_filesystem_file_read_all_bytes(orb_file_handle *handle, void *out_bytes);
ORB_API b8 orb_filesystem_file_read_bytes(orb_file_handle *handle, usize bytes_to_read,
                                          void *out_bytes, usize *out_bytes_read);

ORB_API b8 orb_filesystem_file_append_line(orb_file_handle *handle, const char *text);

ORB_API b8 orb_filesystem_file_append_bytes(orb_file_handle *handle, usize bytes_to_write,
                                           const void *bytes, usize *out_bytes_written);
