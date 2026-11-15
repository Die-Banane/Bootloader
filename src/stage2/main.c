#include <string.h>
#include <stdio.h>
#include <console.h>
#include <filesystem.h>

void __attribute__((cdecl)) c_main(uint32_t size, uint16_t space, uint8_t bootDrive)
{
    ClearScreen();

    FsErr err = FS_OK;
    FsInit(bootDrive, &err);
    if (err != FS_OK) goto end;

    void *file = FsReadFile("foo.txt", 500 , 0x1000);
    char buf[4];
    memcpy(buf, file, 3);
    buf[3] = '\0';
    printf("%s", buf);

    if (file == NULL)
	printf("oh nein\n");

end:
    for (;;)
	continue;
}
