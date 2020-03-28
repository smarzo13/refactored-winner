#ifndef FS_INODE_H
#define FS_INODE_H
#include <sys/types.h>
#include "misc.h"
#include "util/list.h"
#include "util/sync.h"
struct mount;
struct fd;

struct inode_data {
    unsigned refcount;
    ino_t number;
    struct mount *mount;
    struct list chain;

    struct list posix_locks;
    cond_t posix_unlock;

    uint32_t socket_id;

    lock_t lock;
};

struct inode_data *inode_get(struct mount *mount, ino_t inode);
void inode_retain(struct inode_data *inode);
void inode_release(struct inode_data *inode);

// generic_open must lock out anything trying to destroy an inode between
// opening the file and acquiring a reference to its inode. For this purpose
// only, the inodes_lock and inode_get_unlocked are made available. Think
// carefully before using them for anything else.
// mount->lock nests inside this.
// To quote @dril: i despise this lock. id love nothing more than to kick it
// through the wall and shatter it into 100 deadlocks. But i need it
extern lock_t inodes_lock;
struct inode_data *inode_get_unlocked(struct mount *mount, ino_t inode);

// calls mount->fs->inode_orphaned if this inode is orphaned, while holding indoes_lock
void inode_check_orphaned(struct mount *mount, ino_t ino);

// file locking stuff (maybe should go in kernel/calls.h?)

#define F_RDLCK_ 0
#define F_WRLCK_ 1
#define F_UNLCK_ 2

struct file_lock {
    off_t_ start;
    off_t_ end;
    int type;
    pid_t_ pid;
    void *owner;
    struct list locks;
};

struct flock_ {
    word_t type;
    word_t whence;
    off_t_ start;
    off_t_ len;
    pid_t_ pid;
} __attribute__((packed));
struct flock32_ {
    word_t type;
    word_t whence;
    dword_t start;
    dword_t len;
    pid_t_ pid;
} __attribute__((packed));

int fcntl_getlk(struct fd *fd, struct flock_ *flock);
// cmd should be either F_SETLK or F_SETLKW
int fcntl_setlk(struct fd *fd, struct flock_ *flock, bool block);

// locks the inode internally
void file_lock_remove_owned_by(struct fd *fd, void *owner);

#endif
