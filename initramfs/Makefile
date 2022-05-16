CC=$(CROSS_COMPILE)gcc -Wall -Wextra -static -g

$(shell mkdir -p build/initramfs_root)

.PHONY: all clean
all: build/rootfs.gz

clean:
		rm -rf build
		find -name '*.o' -delete

build/rootfs.gz: build/initramfs_root/init build/initramfs_root/child
		cd build/initramfs_root ; \
		find -type f -print0 | cpio -0 -ov -H newc | gzip -9 > ../rootfs.gz

build/initramfs_root/init: init.o
		$(CC) -o $@ $^

build/initramfs_root/child: child.o
		$(CC) -o $@ $^
