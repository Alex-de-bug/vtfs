#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/ioctl.h>
#include <linux/syscalls.h> 

#define MODULE_NAME "vtfs"
#define VTFS_IOCTL_ADD_TAG _IOW('V', 1, char*)
#define VTFS_IOCTL_GET_TAGS _IOR('V', 2, char*)
#define VTFS_IOCTL_REMOVE_TAG _IOW('V', 3, char*)


MODULE_LICENSE("GPL");
MODULE_AUTHOR("secs-dev");
MODULE_DESCRIPTION("A simple FS kernel module");


#define LOG(fmt, ...) pr_info("[" MODULE_NAME "]: " fmt, ##__VA_ARGS__)

unsigned long next_ino = 1;

struct inode* vtfs_get_inode(struct super_block*, const struct inode*, umode_t, int);

struct vtfs_tag {
    char *name;
    struct list_head list; 
};

struct vtfs_inode {
  struct inode *i_inode;
  size_t i_size;
  void *i_data;
  struct list_head tags;
};

struct vtfs_dentry {
  struct dentry *d_dentry;
  char *d_name;
  struct inode *d_parent_inode;
  struct vtfs_inode *d_inode;
  struct list_head list;
};

struct {
  struct super_block *sb;
  struct list_head dentries;
} vtfs_sb;

struct dentry* vtfs_lookup(
  struct inode *parent_inode,
  struct dentry *child_dentry,
  unsigned int flag
) {
  struct vtfs_dentry *dentry;
  struct list_head *pos;
  struct inode *inode;
  
  list_for_each(pos, &vtfs_sb.dentries) {
    dentry = list_entry(pos, struct vtfs_dentry, list);

    if (dentry->d_parent_inode == parent_inode && strcmp(dentry->d_name, child_dentry->d_name.name) == 0) {
      inode = vtfs_get_inode(vtfs_sb.sb, parent_inode, dentry->d_inode->i_inode->i_mode, dentry->d_inode->i_inode->i_ino);
      d_add(child_dentry, inode);
    }
  }

  return NULL;
};

struct inode *test_inode;

int vtfs_create(
    struct user_namespace *mnt_userns,
    struct inode *parent_inode, 
    struct dentry *child_dentry, 
    umode_t mode, 
    bool b
) {
  struct inode *inode;
  struct vtfs_dentry *new_dentry;
  struct vtfs_inode *new_inode;

  inode = vtfs_get_inode(vtfs_sb.sb, parent_inode, mode, next_ino++);
  if (!inode)
    return -ENOMEM;

  new_dentry = kmalloc(sizeof(struct vtfs_dentry), GFP_KERNEL);
  if (!new_dentry) {
    iput(inode);
    return -ENOMEM;
  }

  new_inode = kmalloc(sizeof(struct vtfs_inode), GFP_KERNEL);
  if (!new_inode) {
    kfree(new_dentry);
    iput(inode);
    return -ENOMEM;
  }

  INIT_LIST_HEAD(&new_inode->tags);
  new_dentry->d_dentry = child_dentry;
  new_dentry->d_name = child_dentry->d_name.name;
  new_dentry->d_parent_inode = parent_inode;
  new_dentry->d_inode = new_inode;
  new_dentry->d_inode->i_inode = inode;
  new_dentry->d_inode->i_size = 0;

  list_add(&new_dentry->list, &vtfs_sb.dentries);

  d_add(child_dentry, inode);
  test_inode = inode;

  printk(KERN_INFO "File %s added successfully\n", child_dentry->d_name.name);  
  return 0;
}

int vtfs_unlink(struct inode *parent_inode, struct dentry *child_dentry) {
  struct vtfs_dentry *found_dentry = NULL;
  struct inode *file_inode;
  struct list_head *pos;
  
  list_for_each(pos, &vtfs_sb.dentries) {
    found_dentry = list_entry(pos, struct vtfs_dentry, list);

    if (strcmp(found_dentry->d_name, child_dentry->d_name.name) == 0 && found_dentry->d_parent_inode == parent_inode) {
      file_inode = found_dentry->d_inode->i_inode;

      list_del(&found_dentry->list);

      kfree(found_dentry);

      printk(KERN_INFO "File %s deleted successfully\n", child_dentry->d_name.name);
      return 0;
    }
  }

  return -ENOENT;
}

int vtfs_mkdir(
    struct user_namespace *mnt_userns,
    struct inode *parent_inode,
    struct dentry *child_dentry,
    umode_t mode
) {
    struct inode *inode;
    struct vtfs_dentry *new_dentry;

    INIT_LIST_HEAD(&new_dentry->d_inode->tags);

    inode = vtfs_get_inode(vtfs_sb.sb, parent_inode, mode | S_IFDIR, next_ino++);
    if (!inode) {
        return -ENOMEM;
    }

    new_dentry = kmalloc(sizeof(struct vtfs_dentry), GFP_KERNEL);
    if (!new_dentry) {
        iput(inode);
        return -ENOMEM;
    }

    new_dentry->d_inode = kmalloc(sizeof(struct vtfs_inode), GFP_KERNEL);
    if (!new_dentry->d_inode) {
        kfree(new_dentry);
        iput(inode);
        return -ENOMEM;
    }

    new_dentry->d_dentry = child_dentry;
    new_dentry->d_name = child_dentry->d_name.name;
    new_dentry->d_parent_inode = parent_inode;
    new_dentry->d_inode->i_inode = inode;
    new_dentry->d_inode->i_size = 0;

    list_add(&new_dentry->list, &vtfs_sb.dentries);

    d_add(child_dentry, inode);

    printk(KERN_INFO "Directory %s created successfully\n", child_dentry->d_name.name);

    return 0;
}

int vtfs_rmdir(struct inode *parent_inode, struct dentry *child_dentry) {
  struct vtfs_dentry *found_dentry = NULL;
  struct inode *file_inode;
  struct list_head *pos;
  
  list_for_each(pos, &vtfs_sb.dentries) {
    found_dentry = list_entry(pos, struct vtfs_dentry, list);

    if (strcmp(found_dentry->d_name, child_dentry->d_name.name) == 0 && found_dentry->d_parent_inode == parent_inode) {
      file_inode = found_dentry->d_inode->i_inode;

      list_del(&found_dentry->list);
      kfree(found_dentry);

      printk(KERN_INFO "File %s deleted successfully\n", child_dentry->d_name.name);
      return 0;
    }
  }

  return -ENOENT;
}

int vtfs_link(struct dentry *old_dentry, struct inode *parent_inode, struct dentry *new_dentry) {
    struct vtfs_dentry *existing_dentry = NULL;
    struct list_head *pos;
    struct vtfs_dentry *new_link_dentry;

    list_for_each(pos, &vtfs_sb.dentries) {
        existing_dentry = list_entry(pos, struct vtfs_dentry, list);

        if (strcmp(existing_dentry->d_name, old_dentry->d_name.name) == 0 &&
            existing_dentry->d_parent_inode == old_dentry->d_parent->d_inode) {
            
            new_link_dentry = kmalloc(sizeof(struct vtfs_dentry), GFP_KERNEL);
            if (!new_link_dentry)
                return -ENOMEM;

            new_link_dentry->d_dentry = new_dentry;
            new_link_dentry->d_name = new_dentry->d_name.name;
            new_link_dentry->d_parent_inode = parent_inode;
            new_link_dentry->d_inode = existing_dentry->d_inode;

            list_add(&new_link_dentry->list, &vtfs_sb.dentries);

            d_add(new_dentry, existing_dentry->d_inode->i_inode);

            printk(KERN_INFO "File %s linked successfully\n", new_dentry->d_name.name);
            return 0;
        }
    }

    return -ENOENT;
}

struct inode_operations vtfs_inode_ops = {
  .lookup = vtfs_lookup,
  .create = vtfs_create,
  .unlink = vtfs_unlink,
  .mkdir = vtfs_mkdir,
  .rmdir = vtfs_rmdir,
  .link = vtfs_link,
};

int vtfs_iterate(struct file *file, struct dir_context *ctx) {
    struct vtfs_dentry *dentry;
    struct list_head *pos;
    struct inode *dir_inode = file->f_path.dentry->d_inode;
    unsigned char type;
    loff_t counter = 2;

    if (!dir_emit_dots(file, ctx))
        return 0;

    list_for_each(pos, &vtfs_sb.dentries) {
        dentry = list_entry(pos, struct vtfs_dentry, list);

        if (counter++ < ctx->pos)
            continue;

        if (dentry->d_parent_inode != dir_inode)
            continue;

        if (S_ISDIR(dentry->d_inode->i_inode->i_mode))
            type = DT_DIR;
        else if (S_ISREG(dentry->d_inode->i_inode->i_mode))
            type = DT_REG;
        else
            type = DT_UNKNOWN;

        if (!dir_emit(ctx, dentry->d_name, strlen(dentry->d_name),
                     dentry->d_inode->i_inode->i_ino, type))
            return 0;

        ctx->pos = counter;
    }

    return 0;
}

ssize_t vtfs_read(
  struct file *file,
  char *buffer,
  size_t len,
  loff_t *offset
) {
  struct vtfs_inode *found_inode;
  struct vtfs_dentry *found_dentry;
  struct inode *file_inode = file->f_inode;
  struct list_head *pos;
  ssize_t to_read;

  list_for_each(pos, &vtfs_sb.dentries) {
    found_dentry = list_entry(pos, struct vtfs_dentry, list);
    found_inode = found_dentry->d_inode;

    if (found_dentry->d_inode->i_inode->i_ino == file_inode->i_ino) {
      if (*offset > found_inode->i_size)
        return 0;
    
      to_read = min(len, found_inode->i_size - *offset);
      if (copy_to_user(buffer, found_inode->i_data + *offset, to_read))
        return -EFAULT;

      *offset += to_read;
      return to_read;
    }
  }


  return -ENOENT;
}

ssize_t vtfs_write(
  struct file *file,
  const char *buffer,
  size_t len,
  loff_t *offset
) {
  struct vtfs_inode *found_inode;
  struct vtfs_dentry *found_dentry;
  struct inode *file_inode = file->f_inode;
  struct list_head *pos;
  void *new_data;
  ssize_t new_size;
  int ret;


  list_for_each(pos, &vtfs_sb.dentries) {
    found_dentry = list_entry(pos, struct vtfs_dentry, list);
    found_inode = found_dentry->d_inode;

    if (found_dentry->d_inode->i_inode->i_ino == file_inode->i_ino) {
      new_size = max(found_inode->i_size, *offset + len);

      if (*offset + len > found_inode->i_size) {
        new_data = krealloc(found_inode->i_data, new_size, GFP_KERNEL);
        if (!new_data) {
          return -ENOMEM;
        }

        found_inode->i_data = new_data;
        memset(found_inode->i_data + found_inode->i_size, 0, new_size - found_inode->i_size);
        found_inode->i_size = new_size;
      }

      ret = copy_from_user(found_inode->i_data + *offset, buffer, len);
      if (ret) {
        return -EFAULT;
      }


      *offset += len;
        file->f_inode->i_size = found_inode->i_size; 
      return len;
    }
  }

  return -ENOENT;
}


long vtfs_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct vtfs_inode *inode;
    struct vtfs_dentry *dentry;
    struct vtfs_tag *tag, *temp_tag;
    char *tag_name;
    struct list_head *pos, *q;
    int ret;

    // Найти inode по file
    inode = NULL;
    list_for_each(pos, &vtfs_sb.dentries) {
        dentry = list_entry(pos, struct vtfs_dentry, list);
        if (dentry->d_inode->i_inode == file->f_inode) {
            inode = dentry->d_inode;
            break;
        }
    }

    if (!inode) {
        return -ENOENT;
    }


    switch(cmd) {
        case VTFS_IOCTL_ADD_TAG:
            tag_name = kmalloc(strlen((char*)arg) + 1, GFP_KERNEL);
            if (!tag_name) {
                return -ENOMEM;
            }
            ret = copy_from_user(tag_name, (char*)arg, strlen((char*)arg) + 1);
            if (ret != 0) {
                kfree(tag_name);
                return -EFAULT;
            }


            // Проверка на дубликат тега
            list_for_each(pos, &inode->tags) {
                tag = list_entry(pos, struct vtfs_tag, list);
                if (strcmp(tag->name, tag_name) == 0) {
                    kfree(tag_name);
                    return -EEXIST; // Тег уже существует
                }
            }



            tag = kmalloc(sizeof(*tag), GFP_KERNEL);
            if (!tag) {
                kfree(tag_name);
                return -ENOMEM;
            }

            tag->name = tag_name;
            list_add_tail(&tag->list, &inode->tags);

            return 0;

        case VTFS_IOCTL_GET_TAGS: {
            char *buf = (char *)arg;
            size_t buf_size = PAGE_SIZE;
            size_t offset = 0;

            list_for_each(pos, &inode->tags) {
                tag = list_entry(pos, struct vtfs_tag, list);
                size_t len = strlen(tag->name);

                if (offset + len + 1 > buf_size) {
                    return -ENOBUFS; // Буфер слишком мал
                }

                ret = copy_to_user(buf + offset, tag->name, len + 1);
                 if (ret) return -EFAULT;

                offset += len + 1;
            }
            return offset; // Возвращает общее количество записанных байт
        }

        case VTFS_IOCTL_REMOVE_TAG: {
          tag_name = kmalloc(strlen((char*)arg) + 1, GFP_KERNEL);
            if (!tag_name) {
                return -ENOMEM;
            }
            ret = copy_from_user(tag_name, (char*)arg, strlen((char*)arg) + 1);
            if (ret) {
                kfree(tag_name);
                return -EFAULT;
            }

             list_for_each_safe(pos, q, &inode->tags) {
                tag = list_entry(pos, struct vtfs_tag, list);
                if (strcmp(tag->name, tag_name) == 0) {
                  list_del(pos);
                  kfree(tag->name);
                  kfree(tag);
                    kfree(tag_name);
                    return 0; // Тег удален
                }
            }
             kfree(tag_name);
              return -ENOENT; // Тег не найден
        }

        
        default:
            return -ENOTTY;
    }
}

struct file_operations vtfs_dir_ops = {
    .iterate_shared = vtfs_iterate,
    .read = vtfs_read,
    .write = vtfs_write,
    .unlocked_ioctl = vtfs_ioctl,
};



struct inode* vtfs_get_inode(
  struct super_block* sb, 
  const struct inode* dir, 
  umode_t mode, 
  int i_ino
) {
  struct inode *inode = new_inode(sb);
  if (inode != NULL) {
    inode_init_owner(sb->s_user_ns, inode, dir, mode);
  }

  inode->i_ino = i_ino;
  inode->i_op = &vtfs_inode_ops;
  inode->i_fop = &vtfs_dir_ops;

  inc_nlink(inode);

  return inode;
}

int vtfs_fill_super(struct super_block *sb, void *data, int silent) {
  umode_t mode = S_IFDIR | 0777;

  struct inode *inode = vtfs_get_inode(sb, NULL, mode, next_ino++);
  if (!inode) {
    printk(KERN_ERR "Failed to create a root inode");
    return -ENOMEM;
  }

  sb->s_root = d_make_root(inode);
  if (sb->s_root == NULL) {
    printk(KERN_ERR "Failed to create a root dentry");
    return -ENOMEM;
  }

  vtfs_sb.sb = sb;
  INIT_LIST_HEAD(&vtfs_sb.dentries);

  printk(KERN_INFO "return 0\n");
  return 0;
}

struct dentry* vtfs_mount(
  struct file_system_type* fs_type,
  int flags,
  const char* token,
  void* data
) {
  struct dentry* ret = mount_nodev(fs_type, flags, data, vtfs_fill_super);
  if (ret == NULL) {
    printk(KERN_ERR "Can't mount file system");
  } else {
    printk(KERN_INFO "Mounted successfuly");
  }
  return ret;
}

void vtfs_kill_sb(struct super_block* sb) {
  printk(KERN_INFO "vtfs super block is destroyed. Unmount successfully.\n");
}

struct file_system_type vtfs_fs_type = {
  .name = "vtfs",
  .mount = vtfs_mount,
  .kill_sb = vtfs_kill_sb,
};

static int __init vtfs_init(void) {
  register_filesystem(&vtfs_fs_type);
  LOG("VTFS joined the kernel\n");
  return 0;
}

static void __exit vtfs_exit(void) {
  unregister_filesystem(&vtfs_fs_type);
  LOG("VTFS left the kernel\n");
}

module_init(vtfs_init);
module_exit(vtfs_exit);
