#include "fat.h"
#include <io.h>
#include <filesystem.h>

void __attribute__((cdecl)) c_main(uint32_t size, uint16_t space, uint8_t bootDrive)
{
    FsErr err = FS_OK;
    ClearScreen();
    FsInit(bootDrive, &err);
    
    if (err != FS_OK) goto end;

    void *kernel = FsReadFile("path/to/kernel", 5000, 0x80000);

    if (kernel == NULL)
	Printf("oh nein\n");

end:
    for (;;)
	continue;
}
