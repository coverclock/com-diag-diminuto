/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This translation unit is the implementation of the Diminuto Generic Memory
 * Mapped Driver. It is compiled using the standard Linux 2.6 module build
 * process.
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/proc_fs.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>
#include "diminuto_mmdriver.h"
#include "diminuto_kernel_map.h"
#include "diminuto_log.h"

#if !defined(DIMINUTO_MMDRIVER_BEGIN)
#define DIMINUTO_MMDRIVER_BEGIN (0xfffff400UL) /* Address of PIOA */
#endif

#if !defined(DIMINUTO_MMDRIVER_END)
#define DIMINUTO_MMDRIVER_END (0xfffffc00UL) /* Address past PIOD */
#endif

static int beginning = -1;
static int ending = -1;
static unsigned long begin = DIMINUTO_MMDRIVER_BEGIN;
static unsigned long end = DIMINUTO_MMDRIVER_END;
static unsigned long start = 0;
static unsigned long length = 0;
static struct resource * regionp = 0;
static void * basep = 0;
static void __iomem * pagep = 0;
static int opened = 0;
static int ioctls = 0;
static int errors = 0;

static DECLARE_MUTEX(semaphore);

static int
mmdriver_ioctl_read(
    const void * address,
    diminuto_mmdriver_op * opp
) {
    int rc;

    rc = diminuto_kernel_get(address, opp->width, &(opp->datum));

    return rc;
}

static int
mmdriver_ioctl_write(
    void * address,
    diminuto_mmdriver_op * opp
) {
    int rc;
    diminuto_kernel_datum tmp;

    do {

        tmp = opp->datum;

        if ((rc = diminuto_kernel_get(address, opp->width, &(opp->datum)))) {
            break;
        }

        if ((rc = diminuto_kernel_put(address, opp->width, &tmp))) {
            break;
        }

    } while (0);

    return rc;
}

static int
mmdriver_ioctl_set(
    void * address,
    diminuto_mmdriver_op * opp
) {
    int rc;
    diminuto_kernel_datum tmp;

    do {

        tmp = opp->datum;

        if ((rc = diminuto_kernel_get(address, opp->width, &(opp->datum)))) {
            break;
        }

        if (opp->width == EIGHT) {
            tmp.eight = opp->datum.eight | tmp.eight;
        } else if (opp->width == SIXTEEN) {
            tmp.sixteen = opp->datum.sixteen | tmp.sixteen;
        } else if (opp->width == THIRTYTWO) {
            tmp.thirtytwo = opp->datum.thirtytwo | tmp.thirtytwo;
        } else {
            rc = -EINVAL;
            break;
        }

        if ((rc = diminuto_kernel_put(address, opp->width, &tmp))) {
            break;
        }

    } while (0);

    return rc;
}

static int
mmdriver_ioctl_clear(
    void * address,
    diminuto_mmdriver_op * opp
) {
    int rc;
    diminuto_kernel_datum tmp;

    do {

        tmp = opp->datum;

        if ((rc = diminuto_kernel_get(address, opp->width, &(opp->datum)))) {
            break;
        }

        if (opp->width == EIGHT) {
            tmp.eight = opp->datum.eight & ~tmp.eight;
        } else if (opp->width == SIXTEEN) {
            tmp.sixteen = opp->datum.sixteen & ~tmp.sixteen;
        } else if (opp->width == THIRTYTWO) {
            tmp.thirtytwo = opp->datum.thirtytwo & ~tmp.thirtytwo;
        } else {
            rc = -EINVAL;
            break;
        }

        if ((rc = diminuto_kernel_put(address, opp->width, &tmp))) {
            break;
        }

    } while (0);

    return rc;
}

static int
mmdriver_open(
    struct inode * inode,
    struct file * fp
) {
    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "mmdriver_open\n");
    ++opened;
    return 0;
}

static int
mmdriver_release(
    struct inode * inode,
    struct file * file
) {
    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "mmdriver_release\n");
    --opened;
    return 0;
}

static int
mmdriver_ioctl(
    struct inode * inode,
    struct file * file,
    unsigned int cmd,
    unsigned long arg
) {
    int rc;
    diminuto_mmdriver_op op;
    void * address;

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "mmdriver_ioctl(0x%08x,0x%08lx)\n", cmd, arg);

    do {

        if (down_interruptible(&semaphore)) {
            ++errors;
            rc = -EINTR;
            break;
        }

        ++ioctls;

        do {

            if ((_IOC_DIR(cmd) & _IOC_WRITE) && copy_from_user(&op, (diminuto_mmdriver_op *)arg, sizeof(op))) {
                ++errors;
                rc = -EFAULT;
                break;
            }

            address = (char *)basep + (op.address - start);

            if (cmd == DIMINUTO_MMDRIVER_READ) {
                rc = mmdriver_ioctl_read(address, &op);
            } else if (cmd == DIMINUTO_MMDRIVER_WRITE) {
                rc = mmdriver_ioctl_write(address, &op);
            } else if (cmd == DIMINUTO_MMDRIVER_SET) {
                rc = mmdriver_ioctl_set(address, &op);
            } else if (cmd == DIMINUTO_MMDRIVER_CLEAR) {
                rc = mmdriver_ioctl_clear(address, &op);
            } else {
                ++errors;
                rc = -EINVAL;
            }

            if ((_IOC_DIR(cmd) & _IOC_READ) && copy_to_user((diminuto_mmdriver_op *)arg, &op, sizeof(op))) {
                ++errors;
                rc = -EFAULT;
                break;
            }

        } while (0);

        up(&semaphore);

    } while (0);


    return rc;
}

static int
mmdriver_proc_read(
    char * bufferp,
    char ** startp,
    off_t offset,
    int count,
    int * eofp,
    void * datap
) {
    int written;
    int total = 0;

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "mmdriver_proc_read\n");

    written = snprintf(bufferp + total, count, "start=0x%08lx\n", start);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "length=%lu\n", length);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "base=0x%08lx\n", (unsigned long)basep);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "page=0x%08lx\n", (unsigned long)pagep);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "region.start=0x%08x\n", regionp->start);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "region.end=0x%08x\n", regionp->end);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "region.name=\"%s\"\n", regionp->name);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "region.flags=0x%08lx\n", regionp->flags);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "opened=%d\n", opened);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "ioctls=%d\n", ioctls);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "errors=%d\n", errors);
    total += written;
    count -= written;

    return total;
}

static struct file_operations fops = {
    .owner      = THIS_MODULE,
    .ioctl      = mmdriver_ioctl,
    .open       = mmdriver_open,
    .release    = mmdriver_release
};

static int
__init mmdriver_init(
    void
) {
    int rc = 0;

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "mmdriver_init\n");

    do {

        if (beginning != -1) {
            begin = beginning;
        }
        if (ending != -1) {
            end = ending;
        }

        start = begin;
        length = end - begin;

        if ((rc = diminuto_kernel_map(start, length, DIMINUTO_MMDRIVER_NAME, &regionp, &basep, &pagep))) {
            rc = -ENODEV;
            break;
         }

        if (register_chrdev(DIMINUTO_MMDRIVER_MAJOR, DIMINUTO_MMDRIVER_NAME, &fops)) {
            diminuto_kernel_unmap(&pagep, &regionp);
            rc = -EIO;
            break;
        }

        if (!create_proc_read_entry(DIMINUTO_MMDRIVER_PROC, 0, NULL, mmdriver_proc_read, NULL)) {
            unregister_chrdev(DIMINUTO_MMDRIVER_MAJOR, DIMINUTO_MMDRIVER_NAME);
            diminuto_kernel_unmap(&pagep, &regionp);
            rc = -EIO;
            break;
        }

    } while (0);

    return rc;
}

static void __exit
mmdriver_exit(
    void
) {
    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "mmdriver_exit\n");

    remove_proc_entry(DIMINUTO_MMDRIVER_PROC, NULL);

    unregister_chrdev(DIMINUTO_MMDRIVER_MAJOR, DIMINUTO_MMDRIVER_NAME);

    diminuto_kernel_unmap(&pagep, &regionp);
}

module_init(mmdriver_init);
module_exit(mmdriver_exit);

module_param(beginning, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(beginning, "diminuto mmdriver beginning address");

module_param(ending, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(ending, "diminuto mmdriver ending address");

MODULE_AUTHOR("coverclock@diag.com");
MODULE_LICENSE("LGPL");
MODULE_DESCRIPTION("diminuto generic memory mapped driver");
