#include <io.h>
#include <filesystem.h>

void __attribute__((cdecl)) c_main(uint32_t size, uint16_t space, uint8_t boot_drive)
{
    ClearScreen();
    SetColor(LIGHT_GREEN, BLACK);
    if (FsInit(boot_drive) == FS_OK)
	Printf("ok\n");

    Printf("test");

end:
    for (;;)
	continue;
}
