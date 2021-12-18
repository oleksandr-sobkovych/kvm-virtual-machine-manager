#include "kvm/disk-image.h"

#include <linux/err.h>
#include <mntent.h>


class Disk : public Storage {
public:

    struct disk_image {
        int        fd;
        u64        size;
        struct disk_image_operations  *ops;
        void        *priv;
        void        *disk_req_cb_param;
        void        (*disk_req_cb)(void *param, long len);
        bool        readonly;
        bool        async;
    #ifdef CONFIG_HAS_AIO
        io_context_t      ctx;
        int        evt;
        pthread_t      thread;
        u64        aio_inflight;
    #endif /* CONFIG_HAS_AIO */
        const char      *wwpn;
        const char      *tpgt;
        int        debug_iodelay;
    };

    static struct disk_image_operations blk_dev_ops = {
        .read	= raw_image__read,
        .write	= raw_image__write,
        .wait	= raw_image__wait,
        .async	= true,
    };

private:
    static bool is_mounted(struct stat *st)
    {
        struct stat st_buf;
        struct mntent *mnt;
        FILE *f;

        f = setmntent("/proc/mounts", "r");
        if (!f)
            return false;

        while ((mnt = getmntent(f)) != NULL) {
            if (stat(mnt->mnt_fsname, &st_buf) == 0 &&
                S_ISBLK(st_buf.st_mode) && st->st_rdev == st_buf.st_rdev) {
                fclose(f);
                return true;
            }
        }

        fclose(f);
        return false;
    }

    struct disk_image *blkdev__probe(const char *filename, int flags, struct stat *st)
    {
        int fd, r;
        u64 size;

        if (!S_ISBLK(st->st_mode))
            return ERR_PTR(-EINVAL);

        if (is_mounted(st)) {
            pr_err("Block device %s is already mounted! Unmount before use.",
                   filename);
            return ERR_PTR(-EINVAL);
        }

        /*
         * Be careful! We are opening host block device!
         * Open it readonly since we do not want to break user's data on disk.
         */
        fd = open(filename, flags);
        if (fd < 0)
            return ERR_PTR(fd);

        if (ioctl(fd, BLKGETSIZE64, &size) < 0) {
            r = -errno;
            close(fd);
            return ERR_PTR(r);
        }

        /*
         * FIXME: This will not work on 32-bit host because we can not
         * mmap large disk. There is not enough virtual address space
         * in 32-bit host. However, this works on 64-bit host.
         */
        return disk_image__new(fd, size, &blk_dev_ops, DISK_IMAGE_REGULAR);
    }
};



