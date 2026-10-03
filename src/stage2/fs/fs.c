#include <io.h>
#include <fat.h>
#include <filesystem.h>

/*
 * Filesystem dispatch layer.
 * The public, filesystem-agnostic API (fsInit, fsReadFile, ...) will live here,
 * selected through a small ops table so main.c never has to know the
 * underlying filesystem type.
 */

FsErr FsInit(uint8_t drive)
{
    char *errMsg = "";
    Fat fat = FatInit(drive, &errMsg);
    if (fat.err != FS_OK)
	PrintfAt(0, 0, LIGHT_RED, BLACK, "%s", errMsg);
    
    return fat.err;
}
