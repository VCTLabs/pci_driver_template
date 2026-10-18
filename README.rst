pci_driver_template
===================

Quick-start
-----------

Make sure you have you have GNU toolchain installed, then clone this repo
and change into the project directory and run ``make`` to build the module
and ``sudo make install`` to install it into the kernel module tree.

.. code-block:: bash

    git clone https://github.com/VCTLabs/pci_driver_template.git
    cd pci_driver_template/
    make

What is this?
-------------

This is a template for a kernel char driver for the Nallatech 385A (an Arria
10 FPGA Network Accelerator Card). See the RocketBoards.org_ device page for
more info.

Supported make commands::

    make            # build the driver skeleton
    make clean      # clean the build objects
    make install    # install into kernel module tree
    make uninstall  # remove driver from kernel module tree


.. _RocketBoards.org: https://www.rocketboards.org/foswiki/Documentation/385AArria10FPGANetworkAcceleratorCard

