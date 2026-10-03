#pragma once
#include <stddef.h>
#include <stdint.h>

typedef enum
{
    FS_OK = 0,
    FS_NO_PARTITION,
    FS_NO_FILESYSTEM,
    FS_NOT_SUPPORTED,
    FS_FILE_NOT_FOUND,
    FS_NO_BOOT_SIGNATURE
} FsErr;

FsErr FsInit(uint8_t drive);
FsErr FsReadFile(const char *path, size_t size, void *dst);
const char *FsErrStr(FsErr err);
