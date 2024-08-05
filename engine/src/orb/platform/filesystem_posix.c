#include "filesystem.h"

#include "../core/asserts.h"
#include "../core/logger.h"

#include <stdio.h>
#include <sys/stat.h>

b8 orb_filesystem_file_exists(const char *path) {
    struct stat s;
    return stat(path, &s) == 0;
}

b8 orb_filesystem_file_open(const char *path, orb_file_mode_flags flags, b8 binary,
                            orb_file_handle *out_handle) {
    out_handle->handle = nullptr;

    const char *mode_str = "";

    if ((flags & FILE_MODE_READ_FLAG) != 0 && (flags & FILE_MODE_WRITE_FLAG) != 0) {
        mode_str = binary ? "w+b" : "w+";
    } else if ((flags & FILE_MODE_READ_FLAG) != 0 && (flags & FILE_MODE_WRITE_FLAG) == 0) {
        mode_str = binary ? "rb" : "r";
    } else if ((flags & FILE_MODE_READ_FLAG) == 0 && (flags & FILE_MODE_WRITE_FLAG) != 0) {
        mode_str = binary ? "wb" : "w";
    } else {
        ORB_ASSERT(false, "invalid file mode flags");
        return false;
    }

    FILE *file = fopen(path, mode_str);
    if (!file) {
        ORB_ERROR("Error opening file: %s", path);
        return false;
    }

    i32 descriptor = fileno(file);

    struct stat s;
    if (fstat(descriptor, &s) != 0) {
        ORB_ERROR("Error getting file info: %s", path);
        return false;
    }

    out_handle->handle = file;
    out_handle->file_bytes = (usize)s.st_size;

    // TODO: remove this, im just not sure which approach is better or if both even return the same
    // value
    fseek(file, 0, SEEK_END);
    usize size = (usize)ftell(file);
    rewind(file);

    ORB_ASSERT(out_handle->file_bytes == size, "ftell differs from stat.st_size");

    return true;
}
void orb_filesystem_file_close(orb_file_handle *handle) {
    ORB_ASSERT(handle->handle != nullptr, "can not close uninitialized file");

    fclose((FILE *)handle->handle);
    handle->handle = nullptr;
}

b8 orb_filesystem_file_read_all_text(orb_file_handle *handle, char *out_text) {
    ORB_ASSERT(handle->handle != nullptr, "can not read from uninitialized file");

    if (!fgets(out_text, (int)handle->file_bytes, handle->handle)) {
        ORB_ERROR("Could not read from file");
        return false;
    }

    return true;
}

b8 orb_filesystem_file_read_all_bytes(orb_file_handle *handle, void *out_bytes) {
    ORB_ASSERT(handle->handle != nullptr, "can not read from uninitialized file");
    ORB_ASSERT(out_bytes != nullptr, "can not read into nullptr buffer");

    usize bytes_read = fread(out_bytes, 1, handle->file_bytes, (FILE *)handle->handle);
    if (bytes_read != handle->file_bytes) {
        ORB_ERROR("Could not read entire file");
        return false;
    }

    return true;
}

b8 orb_filesystem_file_read_bytes(orb_file_handle *handle, usize bytes_to_read, void *out_bytes,
                                  usize *out_bytes_read) {
    ORB_ASSERT(handle->handle != nullptr, "can not read from uninitialized file");
    ORB_ASSERT(out_bytes != nullptr, "can not read into nullptr buffer");

    *out_bytes_read = fread(out_bytes, 1, bytes_to_read, (FILE *)handle->handle);

    return *out_bytes_read == bytes_to_read;
}

b8 orb_filesystem_file_append_line(orb_file_handle *handle, const char *text) {
    ORB_ASSERT(handle->handle != nullptr, "can not read from uninitialized file");
    ORB_ASSERT(text != nullptr, "can not read from uninitialized file");

    i32 result = fputs(text, (FILE *)handle->handle);
    if (result != EOF) {
        result = fputc('\n', (FILE *)handle->handle);
    }

    fflush((FILE *)handle->handle);
    return result != EOF;
}

b8 orb_filesystem_file_append_bytes(orb_file_handle *handle, usize bytes_to_write,
                                    const void *bytes, usize *out_bytes_written) {
    ORB_ASSERT(handle->handle != nullptr, "can not read from uninitialized file");
    ORB_ASSERT(bytes != nullptr, "can not read from nullptr buffer");

    *out_bytes_written = fwrite(bytes, 1, bytes_to_write, (FILE *)handle->handle);

    fflush((FILE *)handle->handle);

    return *out_bytes_written == bytes_to_write;
}
