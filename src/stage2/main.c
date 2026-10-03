#include <io.h>
#include <filesystem.h>

void __attribute__((cdecl)) c_main(uint32_t size, uint16_t space, uint8_t bootDrive)
{
    ClearScreen();

    FsErr err = FsInit(bootDrive);

    if (err != FS_OK)
    {
	SetColor(LIGHT_RED, BLACK);
	const char *errStr = FsErrStr(err);
	Printf("%s\n", errStr);
	SetColor(WHITE, BLACK);
    }

    Printf("test\n");
end:
    for (;;)
	continue;
}
