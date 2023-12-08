/*
  chardev.c
    - kernel module character devices implementation
 */

#ifndef CHARDEV_H
#define CHARDEV_H

#include "pcie385asfp.h"

int create_char_devs(struct pcie385asfp_driver* drv);
int destroy_char_devs(void);

#endif
