#!/usr/bin/env python3
"""
Install the bootloader (stage 1 + stage 2) onto a block device.

Layout written (512-byte sectors, LBA):

    LBA 0        stage1.bin   - MBR boot code (exactly 512 bytes)
    LBA 1..16    stage2.bin   - padded to 16 sectors, matching .count in
                                src/stage1/mbr.asm

stage 2 is 6+ KiB, so it occupies the 16 sectors after the MBR - that is what
stage1 reads at boot.

This script only installs the loader. It does not create or modify partitions.
The existing MBR bytes 440..509 (disk id, reserved, partition table) are read
from the device and patched into stage1, because stage2 finds its filesystem
through the active partition entry (GetActivePartition()).

The device must already be partitioned with an active partition containing a
FAT filesystem; this script does not touch the filesystem either.

Usage:

    sudo ./install.py /dev/sdb
    sudo ./install.py /dev/sdb --stage1 build/stage1.bin --stage2 build/stage2.bin -y
"""

import argparse
import errno
import fcntl
import os
import stat
import sys

SECTOR = 512
BLKSSZGET = 0x1268

MBR_META_OFF = 440
MBR_META_LEN = 70          # 4 disk id + 2 reserved + 64 table
BOOT_SIG = b"\x55\xaa"

STAGE1_SIZE = 512
STAGE2_SECTORS = 16        # must match .count in src/stage1/mbr.asm


def die(msg):
    print(f"install.py: error: {msg}", file=sys.stderr)
    sys.exit(1)


def read_file(path):
    try:
        with open(path, "rb") as f:
            return f.read()
    except OSError as e:
        die(f"cannot read {path}: {e.strerror}")


def read_all(fd, size, offset):
    chunks = []
    while size > 0:
        data = os.pread(fd, size, offset)
        if not data:
            break
        chunks.append(data)
        size -= len(data)
        offset += len(data)
    return b"".join(chunks)


def write_all(fd, data, offset):
    view = memoryview(data)
    while len(view) > 0:
        n = os.pwrite(fd, view, offset)
        if n <= 0:
            raise OSError(errno.EIO, "short write")
        view = view[n:]
        offset += n


def get_sector_size(fd):
    return int.from_bytes(fcntl.ioctl(fd, BLKSSZGET, b"\x00" * 4), "little")


def mounted_by(device):
    """Return sources if any partition of device is mounted or used as swap."""
    dev = os.path.realpath(device)
    used = []

    for proc in ("/proc/mounts", "/proc/swaps"):
        try:
            with open(proc, "r") as f:
                lines = f.read().splitlines()
        except OSError:
            continue
        for line in lines:
            fields = line.split()
            if not fields or not fields[0].startswith("/"):
                continue
            src = os.path.realpath(fields[0])
            if src == dev or src.startswith(dev):
                used.append(fields[0])
    return used


def open_device(device, force):
    try:
        st = os.stat(device)
    except OSError as e:
        die(f"cannot stat {device}: {e.strerror}")

    if not stat.S_ISBLK(st.st_mode) and not force:
        die(f"{device} is not a block device (use --force to override)")

    used = mounted_by(device)
    if used and not force:
        die(f"{device} is in use ({', '.join(sorted(set(used)))}); "
            "unmount it first")

    try:
        return os.open(device, os.O_RDWR | os.O_EXCL)
    except OSError as e:
        if e.errno == errno.EBUSY:
            die(f"{device} is busy (mounted or otherwise in use)")
        raise


def human_size(n):
    for unit in ("B", "KiB", "MiB", "GiB", "TiB"):
        if n < 1024 or unit == "TiB":
            return f"{n} B" if unit == "B" else f"{n:.1f} {unit}"
        n /= 1024.0


def main(argv=None):
    parser = argparse.ArgumentParser(
        description="Install the bootloader (stage1 + stage2) onto a block device.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("device", help="target block device, e.g. /dev/sdb")
    parser.add_argument("--stage1", default="build/stage1.bin")
    parser.add_argument("--stage2", default="build/stage2.bin")
    parser.add_argument("--force", action="store_true",
                        help="allow non-block devices and devices that are in use")
    parser.add_argument("-y", "--yes", action="store_true",
                        help="skip the confirmation prompt")
    args = parser.parse_args(argv)

    stage1 = read_file(args.stage1)
    stage2 = read_file(args.stage2)

    if len(stage1) != STAGE1_SIZE:
        die(f"{args.stage1} is {len(stage1)} bytes, expected {STAGE1_SIZE}")
    if stage1[510:512] != BOOT_SIG:
        die(f"{args.stage1} does not end in the 0xAA55 signature")
    if any(stage1[MBR_META_OFF:510]):
        die(f"{args.stage1} uses bytes {MBR_META_OFF}..509, "
            "cannot preserve the MBR metadata")
    if not stage2:
        die(f"{args.stage2} is empty")
    if len(stage2) > STAGE2_SECTORS * SECTOR:
        die(f"{args.stage2} is {len(stage2)} bytes but stage1 only loads "
            f"{STAGE2_SECTORS * SECTOR}; update .count in mbr.asm and "
            "STAGE2_SECTORS here")

    fd = open_device(args.device, args.force)
    try:
        sector_size = get_sector_size(fd)
        if sector_size != SECTOR:
            die(f"{args.device} has {sector_size}-byte logical sectors; "
                "this bootloader only supports 512-byte sectors")

        total_sectors = os.lseek(fd, 0, os.SEEK_END) // SECTOR
        if total_sectors < 1 + STAGE2_SECTORS:
            die(f"{args.device} is too small ({total_sectors} sectors)")

        meta = read_all(fd, MBR_META_LEN, MBR_META_OFF)
        if len(meta) != MBR_META_LEN:
            die(f"could not read MBR metadata from {args.device}")

        stage1_patched = bytearray(stage1)
        stage1_patched[MBR_META_OFF:MBR_META_OFF + MBR_META_LEN] = meta
        stage2_blob = stage2 + bytes(STAGE2_SECTORS * SECTOR - len(stage2))

        print(f"Device:    {args.device} "
              f"({human_size(total_sectors * SECTOR)}, {total_sectors} sectors)")
        print(f"Stage 1:   {args.stage1} ({len(stage1)} B) -> LBA 0")
        print(f"Stage 2:   {args.stage2} ({len(stage2)} B, "
              f"{(len(stage2) + SECTOR - 1) // SECTOR} sectors) -> "
              f"LBA 1..{STAGE2_SECTORS}")
        print("Partition table: preserved")
        print()

        if not args.yes:
            print("This overwrites the boot area of the device.")
            if input(f"Type 'yes' to continue on {args.device}: ").strip().lower() != "yes":
                print("aborted")
                return 1

        write_all(fd, stage1_patched, 0)
        write_all(fd, stage2_blob, SECTOR)
        os.fsync(fd)
    finally:
        os.close(fd)

    print("done.")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("\naborted")
        sys.exit(130)
