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


./vtfs_tag -f /mnt/vt/ tag1

./vtfs_tag /mnt/vt/file tag1

./vtfs_tag /mnt/vt/file -r tag1

./vtfs_tag /mnt/vt/file




