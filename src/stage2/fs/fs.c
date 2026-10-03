#include <io.h>
#include <fat.h>
#include <filesystem.h>

FsErr FsInit(uint8_t drive)
{
    FatType type;

    FsErr err = FatInit(drive, &type);
    return err;
}

const char *FsErrStr(FsErr err)
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
