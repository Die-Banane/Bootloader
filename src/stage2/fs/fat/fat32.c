#include <filesystem.h>
#include <io.h>

void *Fat32ReadFile(const char *path, size_t size, uintptr_t dst, FsErr *err)
{
    Printf("reading from %d bytes from %s...", size, path);
    *err = FS_FILE_NOT_FOUND;

    return NULL;
}
