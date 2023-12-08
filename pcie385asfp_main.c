/*
  pcie385asfp_main.c
    - kernel module entry point
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include "chardev.h"
#include "pcie385asfp.h"
#include "info.h"
#include "log.h"

static struct pci_device_id pcie385asfp_id_table[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_NALLATECH, PCI_PRODUCT_ID_PCIE385asfp_BIST) },
	{0,}
};

MODULE_DEVICE_TABLE(pci, pcie385asfp_id_table);

static int pcie385asfp_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void pcie385asfp_remove(struct pci_dev *pdev);

static struct pci_driver pcie385asfp_driver = {
	.name = DRIVER_NAME,
	.id_table = pcie385asfp_id_table,
	.probe = pcie385asfp_probe,
	.remove = pcie385asfp_remove
};

static int __init pcie385asfp_init(void)
{
	LOG_INFO("Loading %s - version %s\n", DRIVER_DESCRIPTION, DRIVER_VERSION);
	LOG_INFO("%s\n", DRIVER_COPYRIGHT);

	return pci_register_driver(&pcie385asfp_driver);
}

static void __exit pcie385asfp_exit(void)
{
	pci_unregister_driver(&pcie385asfp_driver);
}

int read_device_config(struct pci_dev *pdev)
{
	u16 vendor, device, status_reg, command_reg;

	pci_read_config_word(pdev, PCI_VENDOR_ID, &vendor);
	pci_read_config_word(pdev, PCI_DEVICE_ID, &device);

	LOG_INFO("Device vid: 0x%X  pid: 0x%X\n", vendor, device);

	pci_read_config_word(pdev, PCI_STATUS, &status_reg);

	LOG_INFO("Device status reg: 0x%X\n", status_reg);


	pci_read_config_word(pdev, PCI_COMMAND, &command_reg);

	if (command_reg | PCI_COMMAND_MEMORY) {
		LOG_INFO("Device supports memory access\n");

		return 0;
	}

	LOG_ERROR("Device doesn't supports memory access!");

	return -EIO;
}

void release_device(struct pci_dev *pdev)
{
	pci_release_region(pdev, pci_select_bars(pdev, IORESOURCE_MEM));
	pci_disable_device(pdev);
}

static int pcie385asfp_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int bar, err;
	unsigned long mmio_start,mmio_len;
	struct pcie385asfp_driver *drv_priv;

	if (read_device_config(pdev) < 0) {
		return -EIO;
	}

	bar = pci_select_bars(pdev, IORESOURCE_MEM);

	LOG_INFO("Device availale MEM BAR are 0x%x\n", bar);

	err = pci_enable_device_mem(pdev);

	if (err) {
		LOG_ERROR("Failed to enable PCIE device memory, err: %i\n", err);
		return err;
	}

	err = pci_request_region(pdev, bar, DRIVER_NAME);

	if (err) {
		pci_disable_device(pdev);
		return err;
	}

	mmio_start = pci_resource_start(pdev, 0);
	mmio_len   = pci_resource_len(pdev, 0);

	LOG_INFO("PCIE device resource 0: start at 0x%lx with length %lu\n", mmio_start, mmio_len);

	drv_priv = kzalloc(sizeof(struct pcie385asfp_driver), GFP_KERNEL);

	if (!drv_priv) {
		release_device(pdev);
		return -ENOMEM;
	}

	drv_priv->hwmem = ioremap(mmio_start, mmio_len);

	if (!drv_priv->hwmem) {
		release_device(pdev);
		return -EIO;
	}

	LOG_INFO("PCIE device mapped resource 0x%lx to 0x%p\n", mmio_start, drv_priv->hwmem);

	create_char_devs(drv_priv);

	pci_set_drvdata(pdev, drv_priv);

	return 0;
}

static void pcie385asfp_remove(struct pci_dev *pdev)
{
	struct pcie385asfp_driver *drv_priv = pci_get_drvdata(pdev);

	destroy_char_devs();

	if (drv_priv) {
		if (drv_priv->hwmem) {
			iounmap(drv_priv->hwmem);
		}

		kfree(drv_priv);
	}

	release_device(pdev);

	LOG_INFO("Unloaded %s\n", DRIVER_DESCRIPTION);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stephen Arnold <nerdboy@gentoo.org>");
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
MODULE_VERSION(DRIVER_VERSION);

module_init(pcie385asfp_init);
module_exit(pcie385asfp_exit);

