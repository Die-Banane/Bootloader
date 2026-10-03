#pragma once
#include <stdint.h>

struct Dap
{
    uint8_t	size;
    uint8_t	reserved;
    uint16_t	count;
    uint32_t	dest; //format: seg:off
    uint64_t	sector;
} __attribute__((packed));

//status codes an int 13h service returns in ah (ralf browns interrupt list,
//table 00234). the values are the raw codes of the bios, BiosReadDisk
//returns them unmodified
typedef enum
{
    IO_OK			= 0x00,
    IO_INVALID_FUNCTION		= 0x01,
    IO_ADDRESS_MARK_NOT_FOUND	= 0x02,
    IO_WRITE_PROTECTED		= 0x03,
    IO_SECTOR_NOT_FOUND		= 0x04,
    IO_RESET_FAILED		= 0x05,
    IO_DISK_CHANGED		= 0x06,
    IO_DRIVE_PARAMS_FAILED	= 0x07,
    IO_DMA_OVERRUN		= 0x08,
    IO_DATA_BOUNDARY_ERROR	= 0x09,
    IO_BAD_SECTOR		= 0x0a,
    IO_BAD_TRACK		= 0x0b,
    IO_UNSUPPORTED_TRACK	= 0x0c,
    IO_INVALID_SECTOR_COUNT	= 0x0d,
    IO_ADDRESS_MARK_DETECTED	= 0x0e,
    IO_DMA_OUT_OF_RANGE		= 0x0f,
    IO_CRC_ERROR		= 0x10,
    IO_ECC_CORRECTED		= 0x11,
    IO_CONTROLLER_FAILURE	= 0x20,
    IO_NO_MEDIA			= 0x31,
    IO_INVALID_DRIVE_TYPE	= 0x32,
    IO_SEEK_FAILED		= 0x40,
    IO_TIMEOUT			= 0x80,
    IO_DRIVE_NOT_READY		= 0xaa,
    IO_VOLUME_NOT_LOCKED	= 0xb0,
    IO_VOLUME_LOCKED		= 0xb1,
    IO_VOLUME_NOT_REMOVABLE	= 0xb2,
    IO_VOLUME_IN_USE		= 0xb3,
    IO_LOCK_COUNT_EXCEEDED	= 0xb4,
    IO_EJECT_FAILED		= 0xb5,
    IO_READ_PROTECTED		= 0xb6,
    IO_UNDEFINED_ERROR		= 0xbb,
    IO_WRITE_FAULT		= 0xcc,
    IO_STATUS_REGISTER_ERROR	= 0xe0,
    IO_SENSE_FAILED		= 0xff,
} IoErr;

void* ReadDisk(uint8_t drive, uint16_t count, uint64_t sector, uintptr_t dst);
extern IoErr __attribute__((cdecl)) BiosReadDisk(uint8_t drive, volatile struct Dap *dap);
const char *IoErrStr(IoErr err);
