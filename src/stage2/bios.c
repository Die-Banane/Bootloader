#include <console.h>
#include <memory.h>
#include <stdio.h>
#include <bios.h>

#define LinearToSegOff(dst) \
    ((dst) / 16 > 0xffff \
	? (0xffff << 16) | ((dst) - (0xffff * 0x10)) \
	: (((dst) / 16) << 16) | ((dst) % 16))

static const char *IoErrStr(IoErr err)
{
    switch (err)
    {
	case IO_INVALID_FUNCTION:		return "invalid function or parameter";
	case IO_ADDRESS_MARK_NOT_FOUND:		return "address mark not found";
	case IO_WRITE_PROTECTED:		return "disk write protected";
	case IO_SECTOR_NOT_FOUND:		return "sector not found";
	case IO_RESET_FAILED:			return "reset failed";
	case IO_DISK_CHANGED:			return "disk changed";
	case IO_DRIVE_PARAMS_FAILED:		return "drive parameters failed";
	case IO_DMA_OVERRUN:			return "dma overrun";
	case IO_DATA_BOUNDARY_ERROR:		return "data boundary error";
	case IO_BAD_SECTOR:			return "bad sector";
	case IO_BAD_TRACK:			return "bad track";
	case IO_UNSUPPORTED_TRACK:		return "unsupported track";
	case IO_INVALID_SECTOR_COUNT:		return "invalid number of sectors";
	case IO_ADDRESS_MARK_DETECTED:		return "address mark detected";
	case IO_DMA_OUT_OF_RANGE:		return "dma out of range";
	case IO_CRC_ERROR:			return "crc error";
	case IO_ECC_CORRECTED:			return "data ecc corrected";
	case IO_CONTROLLER_FAILURE:		return "controller failure";
	case IO_NO_MEDIA:			return "no media in drive";
	case IO_INVALID_DRIVE_TYPE:		return "invalid drive type";
	case IO_SEEK_FAILED:			return "seek failed";
	case IO_TIMEOUT:			return "timeout";
	case IO_DRIVE_NOT_READY:		return "drive not ready";
	case IO_VOLUME_NOT_LOCKED:		return "volume not locked";
	case IO_VOLUME_LOCKED:			return "volume locked";
	case IO_VOLUME_NOT_REMOVABLE:		return "volume not removable";
	case IO_VOLUME_IN_USE:			return "volume in use";
	case IO_LOCK_COUNT_EXCEEDED:		return "lock count exceeded";
	case IO_EJECT_FAILED:			return "eject failed";
	case IO_READ_PROTECTED:			return "volume read protected";
	case IO_UNDEFINED_ERROR:		return "undefined error";
	case IO_WRITE_FAULT:			return "write fault";
	case IO_STATUS_REGISTER_ERROR:		return "status register error";
	case IO_SENSE_FAILED:			return "sense operation failed";
	case IO_OK:				return "no error";
	default:				return "unknown bios error";
    }
}


void* ReadDisk(uint8_t drive, uint16_t count, uint64_t srcSector, uintptr_t dst)
{
    if (count == 0) return (void*)dst;
    if (dst > RM_MAX || count > (RM_MAX - dst) / SECTOR_SIZE) return NULL;

    //IMPORTANT: NEVER PLACE DAP IN HIGH MEMORY 
    //BiosReadDisk WILL FAIL!!!!!
    static volatile struct Dap *DAP = (struct Dap*)DAP_ADR;

    uintptr_t cur = dst;
    uint16_t left = count;

    while (left)
    {
	uint16_t chunk = left < MAX_READ_SECTORS
	    ? left
	    : MAX_READ_SECTORS;
	
	DAP->size 	= 16;
	DAP->reserved	= 0;
	DAP->count 	= chunk;
	DAP->dest 	= LinearToSegOff(cur);
	DAP->sector 	= srcSector;

	IoErr err = BiosReadDisk(drive, DAP);

	if (err != IO_OK)
	{
	    const char *errStr = IoErrStr(err);

	    SetColor(LIGHT_RED, BLACK);
	    printf("%s\n", errStr);
	    SetColor(WHITE, BLACK);
	    return NULL;
	}

	left -= chunk;
	srcSector += chunk;
	cur += (uintptr_t)chunk * SECTOR_SIZE;
    }

    return (void*)dst;
}
