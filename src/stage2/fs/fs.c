#include <io.h>
#include <fat.h>
#include <filesystem.h>
#include <stddef.h>
#include <stdint.h>

static struct Fs fs;

static const char *FsErrStr(FsErr err)
{
    switch (err)
    {
	case FS_OK:			return "no error";
	case FS_NO_PARTITION:		return "no active partition on boot device";
	case FS_NO_FILESYSTEM:		return "no filesystem on active partition";
	case FS_NOT_SUPPORTED:		return "filesystem not supported";
	case FS_FILE_NOT_FOUND:		return "file not found";
	case FS_NO_BOOT_SIGNATURE:	return "active partition does not contain a valid boot signature";
	default:			return "unknown filesystem error";
    }
}

void FsInit(uint8_t drive)
{
    FsErr err = FS_OK;

    FatInit(drive, &fs, &err);

    if (err != FS_OK)
    {
	const char *errStr = FsErrStr(err);

	SetColor(LIGHT_RED, BLACK);
	Printf("%s", errStr);
	SetColor(WHITE, BLACK);
    }
}

void *FsReadFile(const char *path, size_t size, uintptr_t dst)
{
    FsErr err = FS_OK;

    void *ptr = fs.ReadFile(path, size, dst, &err);

    if (err != FS_OK)
    {
	const char *errStr = FsErrStr(err);

	SetColor(LIGHT_RED, BLACK);
	Printf("an error occurred while reading %s: %s\n", path, errStr);
	SetColor(WHITE, BLACK);
    }

    return ptr;
}
