/*
  pcie385asfp.h
    - defines for pcie device
*/

#ifndef PCIE_385_A_SFP_H
#define PCIE_385_A_SFP_H

#include <linux/types.h>

#ifndef PCI_VENDOR_ID_NALLATECH
#define PCI_VENDOR_ID_NALLATECH   0x198a
#endif

#ifndef PCI_PRODUCT_ID_PCIE385asfp_BIST
#define PCI_PRODUCT_ID_PCIE385asfp_BIST   0x385f
#define PCI_PRODUCT_ID_PCIE385asfp_40GE   0x0100
#endif

struct pcie385asfp_driver {
	u8 __iomem *hwmem;
};

#endif
