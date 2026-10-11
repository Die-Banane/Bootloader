#include <filesystem.h>
#include <io.h>

void *Fat16ReadFile(const char *path, size_t size, uintptr_t dst, FsErr *err)
{
    Printf("reading %d bytes from %s...\n", size, path);
    *err = FS_FILE_NOT_FOUND;

    return NULL;
}
