rm -r /mnt/vt

make

insmod source/vtfs.ko

mkdir /mnt/vt 

mount -t vtfs "" /mnt/vt 


ls /mnt/vt

touch /mnt/vt/1

umount /mnt/vt

rmmod vtfs


dmesg



