/*
 *  Copyright (C) 2005 SAN People
 *  Copyright (C) 2008 Atmel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/at73c213.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/clk.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>
#include <linux/regulator/consumer.h>

#include <mach/hardware.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/irq.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/at91sam9_smc.h>

#include "sam9_smc.h"
#include "generic.h"

static void __init sf9g_map_io(void)
{
	/* Initialize processor: 18.432 MHz crystal */
	at91sam9260_initialize(18432000);

	/* DBGU on ttyS0. (Rx & Tx only) */
	at91_register_uart(0, 0, 0);

	/* USART0 on ttyS1. (Rx, Tx, CTS, RTS, DTR, DSR, DCD, RI) */
	at91_register_uart(AT91SAM9260_ID_US0, 1, ATMEL_UART_CTS | ATMEL_UART_RTS
			   | ATMEL_UART_DTR | ATMEL_UART_DSR | ATMEL_UART_DCD
			   | ATMEL_UART_RI);

	/* USART1 on ttyS2. (Rx, Tx, RTS, CTS) */
	at91_register_uart(AT91SAM9260_ID_US1, 2, ATMEL_UART_CTS | ATMEL_UART_RTS);

	/* set serial console to ttyS0 (ie, DBGU) */
	at91_set_serial_console(0);
}

static void __init sf9g_init_irq(void)
{
	at91sam9260_init_interrupts(NULL);
}


/*
 * USB Host port
 */
static struct at91_usbh_data __initdata sf9g_usbh_data = {
	.ports		= 2,
};

/*
 * USB Device port
 */
static struct at91_udc_data __initdata sf9g_udc_data = {
	.vbus_pin	= AT91_PIN_PC5,
	.pullup_pin	= 0,		/* pull-up driven by UDC */
};


/*
 * SPI devices.
 */
static struct spi_board_info sf9g_spi_devices[] = {
#if !(defined(CONFIG_MMC_ATMELMCI) || defined(CONFIG_MMC_AT91))
	{	/* DataFlash chip */
		.modalias	= "mtd_dataflash",
		.chip_select	= 1,
		.max_speed_hz	= 15 * 1000 * 1000,
		.bus_num	= 0,
	},
#endif
};


/*
 * MACB Ethernet device
 */
static struct at91_eth_data __initdata sf9g_macb_data = {
	.phy_irq_pin	= AT91_PIN_PA7,
	.is_rmii	= 1,
};

static void __init sf9g_add_device_macb(void)
{
	at91_add_device_eth(&sf9g_macb_data);
}

/*
 * NAND flash
 */
static struct mtd_partition __initdata sf9g_nand_partition[] = {
    {
        .name   = "Kernel Partition",
        .offset = 0,
        .size   = 4 * SZ_1M,
    },
    {
        .name   = "Image Partition",
        .offset = MTDPART_OFS_NXTBLK,
        .size   = MTDPART_SIZ_FULL,
    },
};

static struct mtd_partition * __init nand_partitions(int size, int *num_partitions)
{
	*num_partitions = ARRAY_SIZE(sf9g_nand_partition);
	return sf9g_nand_partition;
}

/* det_pin is not connected */
static struct atmel_nand_data __initdata sf9g_nand_data = {
	.ale		= 21,
	.cle		= 22,
	.rdy_pin	= AT91_PIN_PC13,
	.enable_pin	= AT91_PIN_PC14,
	.partition_info	= nand_partitions,
#if defined(CONFIG_MTD_NAND_ATMEL_BUSWIDTH_16)
	.bus_width_16	= 1,
#else
	.bus_width_16	= 0,
#endif
};

static struct sam9_smc_config __initdata sf9g_nand_smc_config = {
	.ncs_read_setup		= 0,
	.nrd_setup		= 2,
	.ncs_write_setup	= 0,
	.nwe_setup		= 2,

	.ncs_read_pulse		= 4,
	.nrd_pulse		= 4,
	.ncs_write_pulse	= 4,
	.nwe_pulse		= 4,

	.read_cycle		= 7,
	.write_cycle		= 7,

	.mode			= AT91_SMC_READMODE | AT91_SMC_WRITEMODE | AT91_SMC_EXNWMODE_DISABLE,
	.tdf_cycles		= 3,
};

static void __init sf9g_add_device_nand(void)
{
	/* setup bus-width (8 or 16) */
	if (sf9g_nand_data.bus_width_16)
		sf9g_nand_smc_config.mode |= AT91_SMC_DBW_16;
	else
		sf9g_nand_smc_config.mode |= AT91_SMC_DBW_8;

	/* configure chip-select 3 (NAND) */
	sam9_smc_configure(3, &sf9g_nand_smc_config);

	at91_add_device_nand(&sf9g_nand_data);
}


/*
 * MCI (SD/MMC)
 * wp_pin and vcc_pin are not connected
 */
#if defined(CONFIG_MMC_ATMELMCI) || defined(CONFIG_MMC_ATMELMCI_MODULE)
static struct mci_platform_data __initdata sf9g_mmc_data = {
	.slot[1] = {
		.bus_width	= 4,
		.detect_pin	= AT91_PIN_PA6,
	},

};
#else
static struct at91_mmc_data __initdata sf9g_mmc_data = {
	.slot_b		= 1,	/* Only one slot so use slot B */
	.wire4		= 1,
	.det_pin	= AT91_PIN_PA6,
};
#endif

static void __init sf9g_add_device_mmc(void)
{
#if defined(CONFIG_MMC_ATMELMCI) || defined(CONFIG_MMC_ATMELMCI_MODULE)
	at91_add_device_mci(0, &sf9g_mmc_data);
#else
	at91_add_device_mmc(0, &sf9g_mmc_data);
#endif
}


static void __init sf9g_board_init(void)
{
	/* Serial */
	at91_add_device_serial();
	/* USB Host */
	at91_add_device_usbh(&sf9g_usbh_data);
	/* USB Device */
	at91_add_device_udc(&sf9g_udc_data);
	/* SPI */
	at91_add_device_spi(sf9g_spi_devices, ARRAY_SIZE(sf9g_spi_devices));
	/* NAND */
	sf9g_add_device_nand();
	/* Ethernet */
	sf9g_add_device_macb();
	/* MMC */
	sf9g_add_device_mmc();
	/* PCK0 provides MCLK to the WM8731 */
	at91_set_B_periph(AT91_PIN_PC1, 0);
	/* SSC (for WM8731) */
	at91_add_device_ssc(AT91SAM9260_ID_SSC, ATMEL_SSC_TX);
}

MACHINE_START(LUX_SF9G, "Evo-Teh LUX-SF9G")
	/* Maintainer: Atmel */
	.boot_params	= AT91_SDRAM_BASE + 0x100,
	.timer		= &at91sam926x_timer,
	.map_io		= sf9g_map_io,
	.init_irq	= sf9g_init_irq,
	.init_machine	= sf9g_board_init,
MACHINE_END
