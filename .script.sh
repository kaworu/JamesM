#!/bin/sh

D="$(/usr/bin/dirname "$0")"
if [ "$(/usr/bin/id -u)" -ne 0 ]; then
    echo "root woot ?" > /dev/stderr
    exit 1
fi

case "$(/usr/bin/basename "$0")" in
    run_bochs.sh)
        # run_bochs.sh
        # mounts the correct loopback device, runs bochs, then unmounts.
        /sbin/losetup /dev/loop0 "$D/floppy.img"
        bochs -f "$D/bochsrc.txt"
        /sbin/losetup -d /dev/loop0
    ;;
    update_image.sh)
        /sbin/losetup /dev/loop0 "$D/floppy.img"
        /bin/mount /dev/loop0 "$D/mnt"
        /bin/cp src/kernel "$D/mnt/kernel"
        /bin/cp initrd.tar "$D/mnt/initrd"
        /bin/sync
        /bin/umount /dev/loop0
        /sbin/losetup -d /dev/loop0
    ;;
    *)
        echo "This incredibly cool script should not be called directly, because too much awesomeness could result. For your own safety, it will exit now." > /dev/stderr
        exit 1
    ;;
esac
