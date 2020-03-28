#ifndef FS_PROC_H
#define FS_PROC_H

#include "fs/stat.h"
#include "misc.h"

struct proc_entry {
    struct proc_dir_entry *meta;
    pid_t_ pid;
    sdword_t fd; // typedef might not have been read yet
};

struct proc_data {
    char *data;
    size_t size;
    size_t capacity;
};

struct proc_dir_entry {
    const char *name;
    mode_t_ mode;

    // file with dynamic name
    void (*getname)(struct proc_entry *entry, char *buf);

    // file with custom show data function
    int (*show)(struct proc_entry *entry, struct proc_data *data);

    // symlink
    int (*readlink)(struct proc_entry *entry, char *buf);

    // directory with static list
    struct proc_dir_entry *children;
    size_t children_sizeof;

    // directory with dynamic contents
    bool (*readdir)(struct proc_entry *entry, unsigned long *index, struct proc_entry *next_entry);

    struct proc_dir_entry *parent;
    int inode;
};

extern struct proc_dir_entry proc_root;
extern struct proc_dir_entry proc_pid;

mode_t_ proc_entry_mode(struct proc_entry *entry);
void proc_entry_getname(struct proc_entry *entry, char *buf);
int proc_entry_stat(struct proc_entry *entry, struct statbuf *stat);

bool proc_dir_read(struct proc_entry *entry, unsigned long *index, struct proc_entry *next_entry);

void proc_buf_write(struct proc_data *buf, const void *data, size_t size);
void proc_printf(struct proc_data *buf, const char *format, ...);

#endif
