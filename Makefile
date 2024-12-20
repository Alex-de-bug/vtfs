obj-m += source/vtfs.o

PWD := $(CURDIR)
KDIR = /lib/modules/`uname -r`/build
EXTRA_CFLAGS = -Wall -g

all: modules vtfs_tag

modules:
	make -C $(KDIR) M=$(PWD) modules
	insmod source/vtfs.ko
	mkdir /mnt/vt
	mount -t vtfs "" /mnt/vt

vtfs_tag:
	gcc vtfs_tag.c -o vtfs_tag

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf .cache
	umount /mnt/vt
	rmmod vtfs
	rm -r /mnt/vt
