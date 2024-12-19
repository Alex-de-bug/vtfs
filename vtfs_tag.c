#include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <fcntl.h>
 #include <sys/ioctl.h>
 #include <errno.h> 

 #define VTFS_IOCTL_ADD_TAG _IOW('V', 1, char*)
 #define VTFS_IOCTL_GET_TAGS _IOR('V', 2, char*)

 int main(int argc, char *argv[]) {
     if (argc < 2) {
         printf("Usage: %s <filename> [tag]\n", argv[0]);
         printf("Usage: %s <filename> -g\n", argv[0]);
         return 1;
     }
     int fd = open(argv[1], O_RDONLY);
     if (fd == -1) {
         perror("open");
         return 1;
     }
     if (argc == 3) {
         if (ioctl(fd, VTFS_IOCTL_ADD_TAG, argv[2]) == -1) {
            perror("ioctl (add tag)");
            return 1;
          }
         printf("Tag '%s' added to '%s'\n", argv[2], argv[1]);
     } else if (argc == 2 && strcmp(argv[1],"-g") != 0) {
          char buf[4096];
          ssize_t len;
          
             len = ioctl(fd, VTFS_IOCTL_GET_TAGS, buf);
          if (len == -1) {
             perror("ioctl (get tags)");
             close(fd);
             return 1;
         }
         if (len == 0) {
              printf("No tags for %s\n",argv[1]);
         } else {
            printf("Tags for %s:\n",argv[1]);
              char *current = buf;
              while (current < buf + len){
                 printf(" - %s\n", current);
                 current += strlen(current) + 1;
                 
              }
        }
    } else {
      printf("Incorrect arguments\n");
        close(fd);
        return 1;
     }
   close(fd);
   return 0;
 }
