#include <io.h>
#include <filesystem.h>

void __attribute__((cdecl)) c_main(uint32_t size, uint16_t space, uint8_t bootDrive)
{
    ClearScreen();
    FsInit(bootDrive);

    Printf("the loader is %d bytes big\n", size);

    void *kernel = FsReadFile("path/to/kernel", 5000, 0x80000);

    if (!kernel)
	Printf("oh nein\n");

    for (;;)
	continue;
}
