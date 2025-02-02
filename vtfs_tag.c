#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>

#define VTFS_IOCTL_ADD_TAG _IOW('V', 1, char*)
#define VTFS_IOCTL_GET_TAGS _IOR('V', 2, char*)
#define VTFS_IOCTL_REMOVE_TAG _IOW('V', 3, char*)
#define VTFS_IOCTL_FIND_BY_TAG _IOW('V', 4, char*)

#define MAX_PATH 256

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename> [tag]\n", argv[0]);
        printf("Usage: %s <filename> -g\n", argv[0]);
        printf("Usage: %s <filename> -r <tag>\n", argv[0]);
        printf("Usage: %s -f <dir> <tag>\n", argv[0]);
        return 1;
    }
    
  if (argc == 4 && strcmp(argv[1], "-f") == 0) {
        char *dir_path = argv[2];
        char *tag_name = argv[3];
        DIR *dir;
        struct dirent *ent;

        dir = opendir(dir_path);
        if (dir == NULL) {
            perror("opendir");
             return 1;
        }
        printf("Files with tag '%s' in '%s':\n", tag_name, dir_path);
        while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            char full_path[MAX_PATH];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, ent->d_name);

            int fd = open(full_path, O_RDONLY);
            if (fd == -1) {
                continue;
            }
             char buf[4096];
                ssize_t len;
            len = ioctl(fd, VTFS_IOCTL_GET_TAGS, buf);
            close(fd);
            if (len > 0) {
                char *current = buf;
                  while (current < buf + len) {
                      if (strcmp(current, tag_name) == 0)
                        {
                            printf(" - %s\n", full_path);
                         }
                       current += strlen(current) + 1;
                 }
            }
        }
    }
        closedir(dir);
        return 0;
  }
   int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    
    if (argc == 3 && strcmp(argv[2],"-g") != 0 && strcmp(argv[2],"-r") != 0) {
        if (ioctl(fd, VTFS_IOCTL_ADD_TAG, argv[2]) == -1) {
            perror("ioctl (add tag)");
            close(fd);
            return 1;
        }
        printf("Tag '%s' added to '%s'\n", argv[2], argv[1]);
    } else if (argc == 2 && strcmp(argv[1],"-g") != 0 ) {
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
    } else if (argc == 4 && strcmp(argv[2],"-r") == 0)
    {
       if (ioctl(fd,VTFS_IOCTL_REMOVE_TAG, argv[3]) == -1) {
           perror("ioctl (remove tag)");
           close(fd);
            return 1;
        }
         printf("Tag '%s' removed from '%s'\n", argv[3], argv[1]);
    } else {
        printf("Incorrect arguments\n");
      close(fd);
      return 1;
    }
  close(fd);
   return 0;
}
