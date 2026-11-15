#include "memory.h"
#include <io.h>
#include <filesystem.h>

void __attribute__((cdecl)) c_main(uint32_t size, uint16_t space, uint8_t bootDrive)
{
    ClearScreen();

    FsErr err = FS_OK;
    FsInit(bootDrive, &err);
    if (err != FS_OK) goto end;

    void *file = FsReadFile("foo.txt", 500 , 0x1000);
    char buf[4];
    Memcpy(buf, file, 3);
    buf[3] = '\0';
    Printf("%s", buf);

    if (file == NULL)
	Printf("oh nein\n");

end:
    for (;;)
	continue;
}
