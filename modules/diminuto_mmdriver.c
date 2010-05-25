/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This translation unit is the implementation of the Diminuto Generic Memory
 * Mapped Driver. It is compiled using the standard Linux 2.6 module build
 * process. By default it controls Programmed I/O device B (PIOB) which on
 * the AT91RM9200-EK board controls LEDs. But it can be trivially configured
 * to provide its read, write, set, and clear interface to any device with
 * memory mapped registers such as FPGAs, but also other PIO pins on the
 * AT91RM9200 processor core. Additional I/O control requests, or even
 * additional read and write entry points, can be easily added.
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

/*******************************************************************************
 * COMPILE TIME CONFIGURABLE PARAMETERS
 ******************************************************************************/

#if !defined(DIMINUTO_MMDRIVER_BEGIN) && !defined(DIMINUTO_MMDRIVER_END)
#include <asm/arch/at91rm9200.h>
#define DIMINUTO_MMDRIVER_BEGIN (AT91_BASE_SYS + AT91_PIOB)
#define DIMINUTO_MMDRIVER_END (AT91_BASE_SYS + AT91_PIOC)
#endif

#if !defined(DIMINUTO_MMDRIVER_EXCLUSIVE)
#define DIMINUTO_MMDRIVER_EXCLUSIVE (0)
#endif

#if !defined(DIMINUTO_MMDRIVER_MAJOR)
#define DIMINUTO_MMDRIVER_MAJOR (240)
#endif

#if !defined(DIMINUTO_MMDRIVER_NAME)
#define DIMINUTO_MMDRIVER_NAME ("mmdriver")
#endif

/*******************************************************************************
 * INSTALL TIME CONFIGURABLE PARAMETERS
 ******************************************************************************/

static unsigned long begin = DIMINUTO_MMDRIVER_BEGIN;
static unsigned long end = DIMINUTO_MMDRIVER_END;
static int exclusive = DIMINUTO_MMDRIVER_EXCLUSIVE;
static unsigned int major = DIMINUTO_MMDRIVER_MAJOR;
static char name[64] = DIMINUTO_MMDRIVER_NAME;

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

static unsigned long start = 0;
static unsigned long length = 0;
static struct resource * regionp = 0;
static void * basep = 0;
static void __iomem * pagep = 0;
static char proc[sizeof("driver/") + sizeof(name) - 1] = "driver/";
static int opens = 0;
static int closes = 0;
static int ioctls = 0;
static int procs = 0;
static int errors = 0;
static DECLARE_MUTEX(semaphore);

/*******************************************************************************
 * HELPER FUNCTIONS
 ******************************************************************************/

static int
mmdriver_operation_read(
    const void * pointer,
    diminuto_mmdriver_op * opp
) {
    int rc;

    rc = diminuto_kernel_get(pointer, opp->width, &(opp->datum));

    return rc;
}

static int
mmdriver_operation_write(
    void * pointer,
    diminuto_mmdriver_op * opp
) {
    int rc;
    diminuto_kernel_datum tmp;

    do {

        tmp = opp->datum;

        if ((rc = diminuto_kernel_get(pointer, opp->width, &(opp->datum)))) {
            break;
        }

        if ((rc = diminuto_kernel_put(pointer, opp->width, &tmp))) {
            break;
        }

    } while (0);

    return rc;
}

static int
mmdriver_operation_set(
    void * pointer,
    diminuto_mmdriver_op * opp
) {
    int rc;
    diminuto_kernel_datum tmp;

    do {

        tmp = opp->datum;

        if ((rc = diminuto_kernel_get(pointer, opp->width, &(opp->datum)))) {
            break;
        }

        if (opp->width == EIGHT) {
            tmp.eight = opp->datum.eight | tmp.eight;
        } else if (opp->width == SIXTEEN) {
            tmp.sixteen = opp->datum.sixteen | tmp.sixteen;
        } else if (opp->width == THIRTYTWO) {
            tmp.thirtytwo = opp->datum.thirtytwo | tmp.thirtytwo;
        } else if (opp->width == SIXTYFOUR) {
            tmp.sixtyfour = opp->datum.sixtyfour | tmp.sixtyfour;
        } else {
            rc = -EINVAL;
            break;
        }

        if ((rc = diminuto_kernel_put(pointer, opp->width, &tmp))) {
            break;
        }

    } while (0);

    return rc;
}

static int
mmdriver_operation_clear(
    void * pointer,
    diminuto_mmdriver_op * opp
) {
    int rc;
    diminuto_kernel_datum tmp;

    do {

        tmp = opp->datum;

        if ((rc = diminuto_kernel_get(pointer, opp->width, &(opp->datum)))) {
            break;
        }

        if (opp->width == EIGHT) {
            tmp.eight = opp->datum.eight & ~tmp.eight;
        } else if (opp->width == SIXTEEN) {
            tmp.sixteen = opp->datum.sixteen & ~tmp.sixteen;
        } else if (opp->width == THIRTYTWO) {
            tmp.thirtytwo = opp->datum.thirtytwo & ~tmp.thirtytwo;
        } else if (opp->width == SIXTYFOUR) {
            tmp.sixtyfour = opp->datum.sixtyfour & ~tmp.sixtyfour;
        } else {
            rc = -EINVAL;
            break;
        }

        if ((rc = diminuto_kernel_put(pointer, opp->width, &tmp))) {
            break;
        }

    } while (0);

    return rc;
}

/*******************************************************************************
 * DRIVER ENTRY POINTS
 ******************************************************************************/

static int
mmdriver_open(
    struct inode * inode,
    struct file * fp
) {
    pr_debug("mmdriver_open\n");
    ++opens;
    return 0;
}

static int
mmdriver_release(
    struct inode * inode,
    struct file * file
) {
    pr_debug("mmdriver_release\n");
    ++closes;
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
    char * pointer;

    pr_debug("mmdriver_ioctl(0x%08x,0x%08lx)\n", cmd, arg);

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

            if (op.offset > length) {
                ++errors;
                rc = -EINVAL;
                break;
            }

            pointer = (char *)basep + op.offset;

            if (cmd == DIMINUTO_MMDRIVER_READ) {
                rc = mmdriver_operation_read(pointer, &op);
            } else if (cmd == DIMINUTO_MMDRIVER_WRITE) {
                rc = mmdriver_operation_write(pointer, &op);
            } else if (cmd == DIMINUTO_MMDRIVER_SET) {
                rc = mmdriver_operation_set(pointer, &op);
            } else if (cmd == DIMINUTO_MMDRIVER_CLEAR) {
                rc = mmdriver_operation_clear(pointer, &op);
            } else {
                ++errors;
                rc = -EINVAL;
                break;
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

static struct file_operations fops = {
    .owner      = THIS_MODULE,
    .ioctl      = mmdriver_ioctl,
    .open       = mmdriver_open,
    .release    = mmdriver_release
};

/*******************************************************************************
 * PROC FILE SYSTEM FUNCTIONS
 ******************************************************************************/

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

    pr_debug("mmdriver_proc_read\n");

    ++procs;

    written = snprintf(bufferp + total, count, "name=\"%s\"\n", name);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "major=%u\n", major);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "exclusive=%d\n", exclusive);
    total += written;
    count -= written;

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

    if (regionp) {
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
    }

    written = snprintf(bufferp + total, count, "proc=\"%s\"\n", proc);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "opens=%d\n", opens);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "closes=%d\n", closes);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "ioctls=%d\n", ioctls);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "procs=%d\n", procs);
    total += written;
    count -= written;

    written = snprintf(bufferp + total, count, "errors=%d\n", errors);
    total += written;
    count -= written;

    return total;
}

/*******************************************************************************
 * LOADABLE MODULE INSERT AND REMOVE
 ******************************************************************************/

static int
__init mmdriver_init(
    void
) {
    int rc = 0;
    struct resource ** regionpp;

    do {

        pr_debug("mmdriver_init\n");

        pr_debug("begin=0x%lx\n", begin);
        pr_debug("end=0x%lx\n", end);
        pr_debug("exclusive=%d\n", exclusive);
        pr_debug("major=%u\n", major);
        pr_debug("name=\"%s\"\n", name);

        start = begin;
        length = end - begin;

        regionpp = exclusive ? &regionp : 0;

        if ((rc = diminuto_kernel_map(start, length, name, regionpp, &basep, &pagep))) {
            pr_err("mmdriver_init: diminuto_kernel_map failed!\n");
            break;
         }

        if (register_chrdev(major, name, &fops)) {
            pr_err("mmdriver_init: register_chrdev failed!\n");
            diminuto_kernel_unmap(&pagep, regionpp);
            rc = -ENODEV;
            break;
        }

        strncat(proc, name, sizeof(name));

        if (!create_proc_read_entry(proc, 0, NULL, mmdriver_proc_read, NULL)) {
            pr_err("mmdriver_init: create_proc_read_entry failed!\n");
            unregister_chrdev(major, name);
            diminuto_kernel_unmap(&pagep, regionpp);
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
    struct resource ** regionpp;

    pr_debug("mmdriver_exit\n");

    remove_proc_entry(proc, NULL);

    unregister_chrdev(major, name);

    regionpp = exclusive ? &regionp : 0;

    diminuto_kernel_unmap(&pagep, regionpp);
}

module_init(mmdriver_init);
module_exit(mmdriver_exit);

/*******************************************************************************
 * PARAMETERS
 ******************************************************************************/

module_param(begin, ulong, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(begin, "diminuto mmdriver beginning address");

module_param(end, ulong, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(end, "diminuto mmdriver ending address");

module_param(exclusive, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(exclusive, "diminuto mmdriver exclusive flag");

module_param(major, uint, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(major, "diminuto mmdriver major number");

module_param_string(name, name, sizeof(name), S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(name, "diminuto mmdriver module name");

MODULE_AUTHOR("coverclock@diag.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("diminuto generic memory mapped driver");
