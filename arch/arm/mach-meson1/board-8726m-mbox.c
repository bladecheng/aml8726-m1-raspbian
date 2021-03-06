/*
 * arch/arm/mach-meson/board-8726m-dvbc.c
 *
 *  Copyright (C) 2010 AMLOGIC, INC.
 *
 * License terms: GNU General Public License (GPL) version 2
 * Platform machine definition.
 */

#include <mach/io.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>
#include <linux/device.h>
#include <linux/spi/flash.h>
#include <mach/hardware.h>
#include <mach/platform.h>
#include <mach/memory.h>
#include <mach/clock.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/setup.h>
#include <mach/lm.h>
#include <asm/memory.h>
#include <asm/mach/map.h>
#include <mach/am_eth_pinmux.h>
//#include <linux/aml_eth.h>
#include <mach/aml_eth.h>
// treeyan

#include <mach/nand.h>
#include <mach/card_io.h>
#include <linux/i2c.h>
// treeyan
//#include <linux/i2c-aml.h>
#include <mach/i2c-aml.h>
#include <mach/power_gate.h>

// ddd
//#ifdef CONFIG_AM_UART_WITH_S_CORE 
//#include <linux/uart-aml.h>
#ifdef CONFIG_SERIAL_MESON_ONE
#include <mach/uart-aml.h>
#endif

#include <mach/pinmux.h>
#include <mach/gpio.h>
#include <linux/delay.h>
#include <mach/clk_set.h>
#include "board-8726m-mbox.h"

#ifdef CONFIG_ANDROID_PMEM
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/android_pmem.h>
#endif

#ifdef CONFIG_USB_ANDROID
#include <linux/usb/android_composite.h>
#endif
#ifdef CONFIG_SUSPEND
#include <mach/pm.h>
#endif

#ifdef CONFIG_EFUSE
#include <linux/efuse.h>
#endif

#include <mach/system.h>

// treeyan
#define  _outl(_v,_p)       writel((_v),(void*)(_p))
#define  _inl(_p)           readl((void*)(_p))


#if defined(CONFIG_JPEGLOGO)
static struct resource jpeglogo_resources[] = {
    [0] = {
        .start = CONFIG_JPEGLOGO_ADDR,
        .end   = CONFIG_JPEGLOGO_ADDR + CONFIG_JPEGLOGO_SIZE - 1,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = CODEC_ADDR_START,
        .end   = CODEC_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device jpeglogo_device = {
	.name = "jpeglogo-dev",
	.id   = 0,
    .num_resources = ARRAY_SIZE(jpeglogo_resources),
    .resource      = jpeglogo_resources,
};
#endif

#if defined(CONFIG_KEYPADS_AM)||defined(CONFIG_KEYPADS_AM_MODULE) 
static struct resource intput_resources[] = {
	{
		.start = 0x0,
		.end = 0x0,
		.name="8726",
		.flags = IORESOURCE_IO,
	},
};

static struct platform_device input_device = {
	.name = "m1-kp",
	.id = 0,
	.num_resources = ARRAY_SIZE(intput_resources),
	.resource = intput_resources,
	
};
#endif

#if defined(CONFIG_FB_AM)
static struct resource fb_device_resources[] = {
    [0] = {
        .start = OSD1_ADDR_START,
        .end   = OSD1_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
#if defined(CONFIG_FB_OSD2_ENABLE)
    [1] = {
        .start = OSD2_ADDR_START,
        .end   = OSD2_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
#endif

};

static struct platform_device fb_device = {
    .name       = "mesonfb",
    .id         = 0,
    .num_resources = ARRAY_SIZE(fb_device_resources),
    .resource      = fb_device_resources,
};
#endif

#if  defined(CONFIG_AM_TV_OUTPUT)||defined(CONFIG_AM_TCON_OUTPUT)
static struct resource vout_device_resources[] = {
    [0] = {
        .start = 0,
        .end   = 0,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device vout_device = {
    .name       = "mesonvout",
    .id         = 0,
    .num_resources = ARRAY_SIZE(vout_device_resources),
    .resource      = vout_device_resources,
};
#endif

#ifdef CONFIG_USB_ANDROID
#ifdef CONFIG_USB_ANDROID_MASS_STORAGE
static struct usb_mass_storage_platform_data mass_storage_pdata = {
       .nluns = 2,
       .vendor = "AMLOGIC",
       .product = "Android MBX",
       .release = 0x0100,
#ifdef CONFIG_AML_NFTL
       .unremovableflag=0,
#else
       .unremovableflag=1,
#endif
};
static struct platform_device usb_mass_storage_device = {
       .name = "usb_mass_storage",
       .id = -1,
       .dev = {
               .platform_data = &mass_storage_pdata,
               },
};
#endif
static char *usb_functions[] = { "usb_mass_storage" };
static char *usb_functions_adb[] = { 
#ifdef CONFIG_USB_ANDROID_MASS_STORAGE
"usb_mass_storage", 
#endif

#ifdef CONFIG_USB_ANDROID_ADB
"adb" 
#endif
};

static struct android_usb_product usb_products[] = {
       {
               .product_id     = 0x0c01,
               .num_functions  = ARRAY_SIZE(usb_functions),
               .functions      = usb_functions,
       },
       {
               .product_id     = 0x0c02,
               .num_functions  = ARRAY_SIZE(usb_functions_adb),
               .functions      = usb_functions_adb,
       },
};

static struct android_usb_platform_data android_usb_pdata = {
       .vendor_id      = 0x0bb4,
       .product_id     = 0x0c01,
       .version        = 0x0100,
       .product_name   = "Android MBX",
       .manufacturer_name = "AMLOGIC",
       .num_products = ARRAY_SIZE(usb_products),
       .products = usb_products,
       .num_functions = ARRAY_SIZE(usb_functions_adb),
       .functions = usb_functions_adb,
};

static struct platform_device android_usb_device = {
       .name   = "android_usb",
       .id             = -1,
       .dev            = {
               .platform_data = &android_usb_pdata,
       },
};
#endif

#if defined(CONFIG_AMLOGIC_SPI_NOR)
static struct mtd_partition spi_partition_info[] = {
/* Hide uboot partition
        {
                .name = "uboot",
                .offset = 0,
                .size = 0x3e000,
        },
*/
	{
		.name = "ubootenv",
		.offset = 0x3e000,
		.size = 0x2000,
	},
/* Hide recovery partition
        {
                .name = "recovery",
                .offset = 0x40000,
                .size = 0x1c0000,
        },
*/
// Add a partition for uboot update 
        {
                .name = "ubootwhole",
                .offset = 0,
                .size = 0x400000,//Joe 111226
        },
};

static struct flash_platform_data amlogic_spi_platform = {
	.parts = spi_partition_info,
	.nr_parts = ARRAY_SIZE(spi_partition_info),
};

static struct resource amlogic_spi_nor_resources[] = {
	{
		.start = 0xc1800000,
		.end = 0xc1ffffff,
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device amlogic_spi_nor_device = {
	.name = "AMLOGIC_SPI_NOR",
	.id = -1,
	.num_resources = ARRAY_SIZE(amlogic_spi_nor_resources),
	.resource = amlogic_spi_nor_resources,
	.dev = {
		.platform_data = &amlogic_spi_platform,
	},
};

#endif

#ifdef CONFIG_USB_DWC_OTG_HCD
#ifdef CONFIG_USB_DPLINE_PULLUP_DISABLE
static set_vbus_valid_ext_fun(unsigned int id,char val)
{
	unsigned int  reg = (PREI_USB_PHY_A_REG1 + id);
	if(val == 1)
	{
		SET_CBUS_REG_MASK(reg,1<<0);
	}
	else
	{
		CLEAR_CBUS_REG_MASK(reg,1<<0);
	}
}
#endif
static void set_usb_a_vbus_power(char is_power_on)
{
#define USB_A_POW_GPIO_BIT	20
	if(is_power_on){
		printk(KERN_INFO "set usb port power on (board gpio %d)!\n",USB_A_POW_GPIO_BIT);
		set_gpio_mode(PREG_HGPIO,USB_A_POW_GPIO_BIT,GPIO_OUTPUT_MODE);
		set_gpio_val(PREG_HGPIO,USB_A_POW_GPIO_BIT,1);
	}
	else	{
		printk(KERN_INFO "set usb port power off (board gpio %d)!\n",USB_A_POW_GPIO_BIT);		
		set_gpio_mode(PREG_HGPIO,USB_A_POW_GPIO_BIT,GPIO_OUTPUT_MODE);
		set_gpio_val(PREG_HGPIO,USB_A_POW_GPIO_BIT,0);		
	}
}
//usb_a is OTG port
static struct lm_device usb_ld_a = {
	.type = LM_DEVICE_TYPE_USB,
	.id = 0,
	.irq = INT_USB_A,
	.resource.start = IO_USB_A_BASE,
	.resource.end = -1,
	.dma_mask_room = DMA_BIT_MASK(32),
	.port_type = USB_PORT_TYPE_OTG,//USB_PORT_TYPE_OTG,
	.port_speed = USB_PORT_SPEED_DEFAULT,
    .dma_config = USB_DMA_BURST_INCR16,
	.set_vbus_power = set_usb_a_vbus_power,
#ifdef CONFIG_USB_DPLINE_PULLUP_DISABLE	
	.set_vbus_valid_ext = set_vbus_valid_ext_fun,
#endif
};
static struct lm_device usb_ld_b = {
	.type = LM_DEVICE_TYPE_USB,
	.id = 1,
	.irq = INT_USB_B,
	.resource.start = IO_USB_B_BASE,
	.resource.end = -1,
	.dma_mask_room = DMA_BIT_MASK(32),
	.port_type = USB_PORT_TYPE_HOST,
	.port_speed = USB_PORT_SPEED_DEFAULT,
    .dma_config = USB_DMA_BURST_INCR16, //   USB_DMA_DISABLE,
	.set_vbus_power = 0,
#ifdef CONFIG_USB_DPLINE_PULLUP_DISABLE	
	.set_vbus_valid_ext = set_vbus_valid_ext_fun,
#endif	
};
#endif
#ifdef CONFIG_SATA_DWC_AHCI
static struct lm_device sata_ld = {
	.type = LM_DEVICE_TYPE_SATA,
	.id = 2,
	.irq = INT_SATA,
	.dma_mask_room = DMA_BIT_MASK(32),
	.resource.start = IO_SATA_BASE,
	.resource.end = -1,
};
#endif

#if defined(CONFIG_AM_STREAMING)
static struct resource codec_resources[] = {
    [0] = {
        .start =  CODEC_ADDR_START,
        .end   = CODEC_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = STREAMBUF_ADDR_START,
	 .end = STREAMBUF_ADDR_END,
	 .flags = IORESOURCE_MEM,
    },
};

static struct platform_device codec_device = {
    .name       = "amstream",
    .id         = 0,
    .num_resources = ARRAY_SIZE(codec_resources),
    .resource      = codec_resources,
};
#endif

#if defined(CONFIG_AM_VIDEO)
static struct resource deinterlace_resources[] = {
    [0] = {
        .start =  DI_ADDR_START,
        .end   = DI_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device deinterlace_device = {
    .name       = "deinterlace",
    .id         = 0,
    .num_resources = ARRAY_SIZE(deinterlace_resources),
    .resource      = deinterlace_resources,
};
#endif

#if defined(CONFIG_TVIN_VDIN)
static struct resource vdin_resources[] = {
    [0] = {
        .start =  VDIN_ADDR_START,		//pbufAddr
        .end   = VDIN_ADDR_END,				//pbufAddr + size
        .flags = IORESOURCE_MEM,
    },


};

static struct platform_device vdin_device = {
    .name       = "vdin",
    .id         = -1,
    .num_resources = ARRAY_SIZE(vdin_resources),
    .resource      = vdin_resources,
};

//add pin mux info for bt656 input
static struct resource bt656in_resources[] = {
    [0] = {
        .start =  VDIN_ADDR_START,		//pbufAddr
        .end   = VDIN_ADDR_END,				//pbufAddr + size
        .flags = IORESOURCE_MEM,
    },
    [1] = {		//bt656/camera/bt601 input resource pin mux setting
        .start =  0x3000,		//mask--mux gpioD 15 to bt656 clk;  mux gpioD 16:23 to be bt656 dt_in
        .end   = PERIPHS_PIN_MUX_5 + 0x3000,	  
        .flags = IORESOURCE_MEM,
    },

    [2] = {			//camera/bt601 input resource pin mux setting
        .start =  0x1c000,		//mask--mux gpioD 12 to bt601 FIQ; mux gpioD 13 to bt601HS; mux gpioD 14 to bt601 VS;
        .end   = PERIPHS_PIN_MUX_5 + 0x1c000,				
        .flags = IORESOURCE_MEM,
    },

    [3] = {			//bt601 input resource pin mux setting
        .start =  0x800,		//mask--mux gpioD 24 to bt601 IDQ;;
        .end   = PERIPHS_PIN_MUX_5 + 0x800,			
        .flags = IORESOURCE_MEM,
    },

};

static struct platform_device bt656in_device = {
    .name       = "amvdec_656in",
    .id         = -1,
    .num_resources = ARRAY_SIZE(bt656in_resources),
    .resource      = bt656in_resources,
};
#endif

#ifdef CONFIG_BT_DEVICE
#include <linux/bt-device.h>

static struct platform_device bt_device = {
	.name             = "bt-dev",
	.id               = -1,
};

static void bt_device_init(void)
{

}

static void bt_device_on(void)
{

}

static void bt_device_off(void)
{

}

struct bt_dev_data bt_dev = {
    .bt_dev_init    = bt_device_init,
    .bt_dev_on      = bt_device_on,
    .bt_dev_off     = bt_device_off,
};
#endif

#if defined(CONFIG_MESON_ONE_CARDREADER)
static struct resource amlogic_card_resource[]  = {
	[0] = {
		.start = 0x1200230,   //physical address
		.end   = 0x120024c,
		.flags = 0x200,
	}
};

static struct aml_card_info  amlogic_card_info[] = {
	[0] = {
		.name = "sd_card",
		.work_mode = CARD_HW_MODE,
		.io_pad_type = SDIO_GPIOA_9_14,
		.card_ins_en_reg = EGPIO_GPIOC_ENABLE,
		.card_ins_en_mask = PREG_IO_25_MASK,
		.card_ins_input_reg = EGPIO_GPIOC_INPUT,
		.card_ins_input_mask = PREG_IO_25_MASK,
		.card_power_en_reg = EGPIO_GPIOC_ENABLE,
		.card_power_en_mask = PREG_IO_26_MASK,
		.card_power_output_reg = EGPIO_GPIOC_OUTPUT,
		.card_power_output_mask = PREG_IO_26_MASK,
		.card_power_en_lev = 0,
		.card_wp_en_reg = EGPIO_GPIOC_ENABLE,
		.card_wp_en_mask = PREG_IO_23_MASK,
		.card_wp_input_reg = EGPIO_GPIOC_INPUT,
		.card_wp_input_mask = PREG_IO_23_MASK,
		.card_extern_init = 0,
	},

#ifdef CONFIG_MS_MSPRO
	[1] = {
		.name = "ms_card",
		.work_mode = CARD_HW_MODE,
		.io_pad_type = SDIO_GPIOA_9_14,
		.card_ins_en_reg = EGPIO_GPIOC_ENABLE,
		.card_ins_en_mask = PREG_IO_25_MASK,
		.card_ins_input_reg = EGPIO_GPIOC_INPUT,
		.card_ins_input_mask = PREG_IO_25_MASK,
		.card_power_en_reg = EGPIO_GPIOC_ENABLE,
		.card_power_en_mask = PREG_IO_26_MASK,
		.card_power_output_reg = EGPIO_GPIOC_OUTPUT,
		.card_power_output_mask = PREG_IO_26_MASK,
		.card_power_en_lev = 0,
		.card_wp_en_reg = EGPIO_GPIOC_ENABLE,
		.card_wp_en_mask = PREG_IO_23_MASK,
		.card_wp_input_reg = EGPIO_GPIOC_INPUT,
		.card_wp_input_mask = PREG_IO_23_MASK,
		.card_extern_init = 0,
	},
#endif
};

static struct aml_card_platform amlogic_card_platform = {
	.card_num = ARRAY_SIZE(amlogic_card_info),
	.card_info = amlogic_card_info,
};

static struct platform_device amlogic_card_device = { 
	.name = "AMLOGIC_CARD", 
	.id    = -1,
	.num_resources = ARRAY_SIZE(amlogic_card_resource),
	.resource = amlogic_card_resource,
	.dev = {
		.platform_data = &amlogic_card_platform,
	},
};
#endif

#if defined(CONFIG_AML_AUDIO_DSP)
static struct resource audiodsp_resources[] = {
    [0] = {
        .start = AUDIODSP_ADDR_START,
        .end   = AUDIODSP_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device audiodsp_device = {
    .name       = "audiodsp",
    .id         = 0,
    .num_resources = ARRAY_SIZE(audiodsp_resources),
    .resource      = audiodsp_resources,
};
#endif

static struct resource aml_m1_audio_resource[]={
		[0]	=	{
				.start 	=	0,
				.end		=	0,
				.flags	=	IORESOURCE_MEM,
		},
};
static struct platform_device aml_sound_card={
		.name 				= "aml_m1_audio",//"aml_m1_audio_wm8900",
		.id 					= -1,
		.resource 		=	aml_m1_audio_resource,
		.num_resources	=	ARRAY_SIZE(aml_m1_audio_resource),
};

#ifdef CONFIG_AM_NAND
static struct mtd_partition normal_partition_info_256M[] = 
{
#ifndef CONFIG_AMLOGIC_SPI_NOR
	/* Hide uboot partition
    {
        .name = "uboot",
        .offset = 0,
        .size = 4*1024*1024,
    },
	*/
    {
        .name = "ubootenv",
        .offset = 4*1024*1024,
        .size = 0x20000,
    },
#endif
  	{
		.name = "aml_logo",
		.offset = 8*1024*1024,
		.size=8*1024*1024,
	},
    {
        .name = "recovery",
        .offset = 16*1024*1024,
        .size = 8*1024*1024,
    },
    {
        .name = "boot",
        .offset = 24*1024*1024,
        .size = 8*1024*1024,
    },
    {
		.name = "system",
		.offset = 32*1024*1024,
		.size = 156*1024*1024,
    },
    {
        .name = "cache",
        .offset = 188*1024*1024,
        .size = 16*1024*1024,
    },
    {
        .name = "userdata",
        .offset=MTDPART_OFS_APPEND,
        .size=MTDPART_SIZ_FULL,
    },
};
static struct mtd_partition normal_partition_info_512M[] = 
{
#ifndef CONFIG_AMLOGIC_SPI_NOR
	/* Hide uboot partition
    {
        .name = "uboot",
        .offset = 0,
        .size = 4*1024*1024,
    },
	*/
    {
        .name = "ubootenv",
        .offset = 4*1024*1024,
        .size = 0x20000,
    },
#endif
  	{
		.name = "aml_logo",
		.offset = 8*1024*1024,
		.size=8*1024*1024,
	},
    {
        .name = "recovery",
        .offset = 16*1024*1024,
        .size = 8*1024*1024,
    },
    {
        .name = "boot",
        .offset = 24*1024*1024,
        .size = 8*1024*1024,
    },
	{
        .name = "system",
        .offset = 32*1024*1024,
#if (defined CONFIG_MACH_MESON_8726M_REFC03_ICS)
        .size = 300*1024*1024,
#else
        .size = 200*1024*1024,
#endif
    },
    {
        .name = "cache",
#if (defined CONFIG_MACH_MESON_8726M_REFC03_ICS)
        .offset = 332*1024*1024,
#else
        .offset = 232*1024*1024,
#endif
        .size = 16*1024*1024,
    },
#ifdef CONFIG_AML_NFTL
   {
        .name = "userdata",
#if (defined CONFIG_MACH_MESON_8726M_REFC03_ICS)
        .offset=348*1024*1024,
        .size=112*1024*1024,
#else
        .offset=248*1024*1024,
        .size=200*1024*1024,
#endif
    },
    {
		.name = "NFTL_Part",
		.offset = MTDPART_OFS_APPEND,
		.size = MTDPART_SIZ_FULL,
	},
#else
    {
        .name = "userdata",
        .offset=MTDPART_OFS_APPEND,
        .size=MTDPART_SIZ_FULL,
    },
#endif
};
static struct mtd_partition normal_partition_info_1G_OR_MORE[] = 
{
#ifndef CONFIG_AMLOGIC_SPI_NOR
	/* Hide uboot partition
    {
        .name = "uboot",
        .offset = 0,
        .size = 4*1024*1024,
    },
	*/
    {
        .name = "ubootenv",
        .offset = 4*1024*1024,
        .size = 4*1024*1024,
    },
#endif
    {
		.name = "aml_logo",
		.offset = 8*1024*1024,
		.size=8*1024*1024,
    },
    {
        .name = "recovery",
        .offset = 16*1024*1024,
        .size = 8*1024*1024,
    },
    {
        .name = "boot",
        .offset = 24*1024*1024,
        .size = 8*1024*1024,
    },
	{
        .name = "system",
        .offset = 32*1024*1024,
        // .size = 200*1024*1024,
        .size = 384*1024*1024,
        
    },
    {
        .name = "cache",
        // .offset = 232*1024*1024,
        .offset = 416*1024*1024,
        .size = 64*1024*1024,
    },
#ifdef CONFIG_AML_NFTL
    {
        .name = "userdata",
        //.offset=296*1024*1024,
        //.size=300*1024*1024,
        .offset=480*1024*1024,
        .size=512*1024*1024, //MTC:Liuese 2013-01-31 change the size for energy

    },
       {
		.name = "NFTL_Part",
		.offset = MTDPART_OFS_APPEND,
		.size = MTDPART_SIZ_FULL,
	},
#else
    {
        .name = "userdata",
        .offset=MTDPART_OFS_APPEND,
        .size=MTDPART_SIZ_FULL,
    },
#endif
};

static struct mtd_partition normal_partition_info[] = 
{
#ifndef CONFIG_AMLOGIC_SPI_NOR
	/* Hide uboot partition
    {
        .name = "uboot",
        .offset = 0,
        .size = 4*1024*1024,
    },
	*/
    {
        .name = "ubootenv",
        .offset = 4*1024*1024,
        .size = 0x20000,
    },
#endif
    {
		.name = "aml_logo",
		.offset = 8*1024*1024,
		.size=8*1024*1024,
    },
    {
        .name = "recovery",
        .offset = 16*1024*1024,
        .size = 8*1024*1024,
    },
    {
        .name = "boot",
        .offset = 24*1024*1024,
        .size = 8*1024*1024,
    },
	{
        .name = "system",
        .offset = 32*1024*1024,
        .size = 156*1024*1024,
    },
    {
        .name = "cache",
        .offset = 188*1024*1024,
        .size = 16*1024*1024,
    },
    {
        .name = "userdata",
        .offset=MTDPART_OFS_APPEND,
        .size=MTDPART_SIZ_FULL,
    },
};

#if 0
#if defined(CONFIG_PM)
static void __init eth_pinmux_init(void);
static void eth_clock_enable(int enable);
static struct resource aml_eth_pm_ops[] = {
    [0] = {
        .start = eth_clock_enable,
        .end   = eth_pinmux_init,
        .flags = IORESOURCE_READONLY,
    },
};
static struct platform_device aml_eth_pm = {
    .name          = "ethernet_pm_driver",
    .num_resources = ARRAY_SIZE(aml_eth_pm_ops),
    .resource      = aml_eth_pm_ops,
    };
#endif
#endif

static void nand_set_parts(uint64_t size, struct platform_nand_chip *chip)
{
	printk("set nand parts for chip %lldMB\n", (size/(1024*1024)));

	if (size/(1024*1024) == 256) {
		chip->partitions = normal_partition_info_256M;
		chip->nr_partitions = ARRAY_SIZE(normal_partition_info_256M);
	}
	else if (size/(1024*1024) == 512) {
		chip->partitions = normal_partition_info_512M;
		chip->nr_partitions = ARRAY_SIZE(normal_partition_info_512M);
	}
	else if (size/(1024*1024) >= 1024) {
		chip->partitions = normal_partition_info_1G_OR_MORE;
		chip->nr_partitions = ARRAY_SIZE(normal_partition_info_1G_OR_MORE);
	}
	else {
		chip->partitions = normal_partition_info;
		chip->nr_partitions = ARRAY_SIZE(normal_partition_info);
	}

	return;
}

static struct aml_nand_platform aml_nand_platform[] = {
#ifndef CONFIG_AMLOGIC_SPI_NOR
	{
		.name = NAND_BOOT_NAME,
		.chip_enable_pad = AML_NAND_CE0,
		.ready_busy_pad = AML_NAND_CE0,
		.platform_nand_data = {
			.chip =  {
				.nr_chips = 1,
				.options = (NAND_TIMING_MODE5 | NAND_ECC_BCH16_MODE),
			},
    	},
		.T_REA = 20,
		.T_RHOH = 15,
	},
#endif
	{
		.name = NAND_NORMAL_NAME,
		.chip_enable_pad = (AML_NAND_CE0 | (AML_NAND_CE1 << 4) | (AML_NAND_CE2 << 8) | (AML_NAND_CE3 << 12)),
		.ready_busy_pad = (AML_NAND_CE0 | (AML_NAND_CE0 << 4) | (AML_NAND_CE1 << 8) | (AML_NAND_CE1 << 12)),
		.platform_nand_data = {
			.chip =  {
				.nr_chips = 4,
				.nr_partitions = ARRAY_SIZE(normal_partition_info),
				.partitions = normal_partition_info,
				.set_parts = nand_set_parts,
				.options = (NAND_TIMING_MODE5 | NAND_ECC_BCH16_MODE | NAND_TWO_PLANE_MODE),
			},
    	},
		.T_REA = 20,
		.T_RHOH = 15,
	}
};

struct aml_nand_device aml_m1_nand_device = {
	.aml_nand_platform = aml_nand_platform,
	.dev_num = ARRAY_SIZE(aml_nand_platform),
};

static struct resource aml_nand_resources[] = {
	{
		.start = 0xc1108600,
		.end = 0xc1108624,
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device aml_nand_device = {
	.name = "aml_m1_nand",
	.id = 0,
	.num_resources = ARRAY_SIZE(aml_nand_resources),
	.resource = aml_nand_resources,
	.dev = {
		.platform_data = &aml_m1_nand_device,
	},
};

#endif



#if defined(CONFIG_PM)

unsigned int eth_clk_src_index = 1;

static void aml_eth_clock_enable(int enable)
{

    if( enable ) {

        demod_apll_setting(0,1200*CLK_1M);//reset demod to enure demod have enabled
        //      udelay(100);
        if ((eth_clk_src_index>=0) && (eth_clk_src_index < 4 )) {
            unsigned int ethernet_clk_measure_index[4] = {
                SYS_PLL_DIV3, DEMOD_PLL_CLK400,
                DDR_PLL_CLK, OTHER_PLL_CLK
            };
            int freq = clk_util_clk_msr_rl(ethernet_clk_measure_index[eth_clk_src_index]);
            if(freq % 50) {  //in MHz
                printk("******************%s: clk [%d] freq = %dMHz!! can not use this clock source!!we will set clk 1!\n", __func__, eth_clk_src_index, freq);
                eth_clk_set(1, 400*CLK_1M, 50*CLK_1M);
            } else {
                eth_clk_set(eth_clk_src_index, freq*CLK_1M, 50*CLK_1M);
                printk("******************%s: freq = %dMHz!! can not use this clock source!!\n", __func__, freq);
            }
        } else {
            printk("******************%s: eth_clk_src_index = %d is invalide(0-3)!!\n", __func__, eth_clk_src_index);
            eth_clk_set(1, 400*CLK_1M, 50*CLK_1M);
        }
        //        eth_clk_set(ETH_CLKSRC_APLL_CLK,400*CLK_1M,50*CLK_1M);
    }else if((eth_clk_src_index >= 0) && (eth_clk_src_index < 4)){
        eth_clk_set(eth_clk_src_index, 0 ,0);
    }
}

/*
 * the code in this function is copied from eth_pinmux_init() in this file.
 */
#define DELAY_TIME 500
static void aml_eth_reset(void)
{
    int i;

    eth_set_pinmux(ETH_BANK2_GPIOD15_D23,ETH_CLK_OUT_GPIOD24_REG5_1,0);
    CLEAR_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, 1);
    SET_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, (1 << 1));
    SET_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, 1);
    for (i = 0; i < DELAY_TIME; i++)
        udelay(100);
    /*reset*/
    ///GPIOC19/NA   nRst;
    set_gpio_mode(PREG_GGPIO,12,GPIO_OUTPUT_MODE);
    set_gpio_val(PREG_GGPIO,12,0);
    udelay(100);    //waiting reset end;
    set_gpio_val(PREG_GGPIO,12,1);
    udelay(10);     //waiting reset end;
}

static struct aml_eth_platform_data aml_eth_pdata = {
    .clock_enable = aml_eth_clock_enable,
    .reset = aml_eth_reset,
};

static struct platform_device aml_eth_pm = {
    .name          = "ethernet_pm_driver",
    .id            = -1,
    .dev = {
        .platform_data = &aml_eth_pdata,
    }

};
#endif



#if defined(CONFIG_I2C_SW_AML)

static struct aml_sw_i2c_platform aml_sw_i2c_plat_fe1 = {
	.sw_pins = {
		.scl_reg_out	= MESON_I2C_PREG_GPIOE_OUTLVL,
		.scl_reg_in		= MESON_I2C_PREG_GPIOE_INLVL,
		.scl_bit		= 20,	/*MESON_GPIOE_20*/
		.scl_oe			= MESON_I2C_PREG_GPIOE_OE,
		.sda_reg_out	= MESON_I2C_PREG_GPIOE_OUTLVL,
		.sda_reg_in		= MESON_I2C_PREG_GPIOE_INLVL,
		.sda_bit		= 21,	/*MESON_GPIOE_21*/
		.sda_oe			= MESON_I2C_PREG_GPIOE_OE,
	},	
	.udelay			= 30,
	.timeout			= 100,
};

static struct platform_device aml_sw_i2c_device_fe1 = {
	.name		  = "aml-sw-i2c",
	.id		  = 0,
	.dev = {
		.platform_data = &aml_sw_i2c_plat_fe1,
	},
};

static struct aml_sw_i2c_platform aml_sw_i2c_plat_fe2 = {
	.sw_pins = {
		.scl_reg_out		= MESON_I2C_PREG_GPIOC_OUTLVL,
		.scl_reg_in		= MESON_I2C_PREG_GPIOC_INLVL,
		.scl_bit			= 21,	/*MESON_GPIOC_13*/
		.scl_oe			= MESON_I2C_PREG_GPIOC_OE,
		.sda_reg_out		= MESON_I2C_PREG_GPIOC_OUTLVL,
		.sda_reg_in		= MESON_I2C_PREG_GPIOC_INLVL,
		.sda_bit			= 22,	/*MESON_GPIOC_14*/
		.sda_oe			= MESON_I2C_PREG_GPIOC_OE,
	},	
	.udelay			= 2,
	.timeout			= 100,
};

static struct platform_device aml_sw_i2c_device_fe2 = {
	.name		  = "aml-sw-i2c",
	.id		  = 1,
	.dev = {
		.platform_data = &aml_sw_i2c_plat_fe2,
	},
};

#endif

#if defined(CONFIG_I2C_AML)
static struct aml_i2c_platform aml_i2c_plat = {
	.wait_count		= 1000000,
	.wait_ack_interval	= 5,
	.wait_read_interval	= 5,
	.wait_xfer_interval	= 5,
	.master_no		= AML_I2C_MASTER_B,
	.use_pio			= 0,
	.master_i2c_speed	= AML_I2C_SPPED_100K,

	.master_b_pinmux = {
		.scl_reg	= MESON_I2C_MASTER_B_GPIOC_13_REG,
		.scl_bit	= MESON_I2C_MASTER_B_GPIOC_13_BIT,
		.sda_reg	= MESON_I2C_MASTER_B_GPIOC_14_REG,
		.sda_bit	= MESON_I2C_MASTER_B_GPIOC_14_BIT,
	}
};

static struct resource aml_i2c_resource[] = {
	[0] = {/*master a*/
		.start = 	MESON_I2C_MASTER_A_START,
		.end   = 	MESON_I2C_MASTER_A_END,
		.flags = 	IORESOURCE_MEM,
	},
	[1] = {/*master b*/
		.start = 	MESON_I2C_MASTER_B_START,
		.end   = 	MESON_I2C_MASTER_B_END,
		.flags = 	IORESOURCE_MEM,
	},
	[2] = {/*slave*/
		.start = 	MESON_I2C_SLAVE_START,
		.end   = 	MESON_I2C_SLAVE_END,
		.flags = 	IORESOURCE_MEM,
	},
};

static struct platform_device aml_i2c_device = {
	.name		  = "aml-i2c",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(aml_i2c_resource),
	.resource	  = aml_i2c_resource,
	.dev = {
		.platform_data = &aml_i2c_plat,
	},
};
#endif

//#if defined(CONFIG_AM_UART_WITH_S_CORE)
#if defined(CONFIG_SERIAL_MESON_ONE)

#if defined(CONFIG_AM_UART0_SET_PORT_B)
    #define UART_0_PORT		UART_B
    #define UART_1_PORT		UART_A
#else // CONFIG_AM_UART0_SET_PORT_B
    #define UART_0_PORT		UART_A
    #define UART_1_PORT		UART_B
#endif // CONFIG_AM_UART0_SET_PORT_B

static struct aml_uart_platform aml_uart_plat = {
    .line[0]		=	UART_0_PORT,
    .line[1]		=	UART_1_PORT
};

static struct platform_device aml_uart_device = {	
    .name           = "am_uart",  
    .id             = -1, 
    .num_resources  = 0,  
    .resource       = NULL,   
    .dev = {        
                .platform_data = &aml_uart_plat,  
           },
};

//#define UART_BASEADDR0    (IO_CBUS_PHY_BASE + (UART0_WFIFO << 2))
//#define UART_BASEADDR1    (IO_CBUS_PHY_BASE + (UART1_WFIFO << 2))
//
//#ifndef CONFIG_AM_UART0_SET_PORT_B
//
//    #define UART_0_PORT		UART_BASEADDR0
//    #define UART_1_PORT		UART_BASEADDR1
//    #define UART_IRQ0       INT_UART
//    #define UART_IRQ1       INT_UART_1
//
//#else
//
//    #define UART_0_PORT		UART_BASEADDR1
//    #define UART_1_PORT		UART_BASEADDR0
//    #define UART_IRQ0       INT_UART_1
//    #define UART_IRQ1       INT_UART
//
//#endif
//
//static struct resource struc_am_uart_0 [] = {
//    [0] = {
//        .start  = UART_0_PORT,
//        .end    = UART_0_PORT + 20,
//        .flags  = IORESOURCE_MEM,
//    },
//    [1] = {
//        .start  = UART_IRQ0,
//        .end    = -1,
//        .flags  = IORESOURCE_IRQ,
//    },
//};
//
//static struct platform_device aml_uart_device = {	
//    .name           = "meson_uart",  
//    .id             = 0, 
//    .num_resources  = 2,  
//    .resource       = struc_am_uart_0,   
//};


#endif // CONFIG_AM_UART_WITH_S_CORE

#ifdef CONFIG_EFUSE
static struct platform_device aml_efuse_device = {
    .name	= "efuse",
    .id	= -1,
};
#endif
#ifdef CONFIG_ANDROID_PMEM
static struct android_pmem_platform_data pmem_data =
{
	.name = "pmem",
	.start = PMEM_START,
	.size = PMEM_SIZE,
	.no_allocator = 1,
	.cached = 0,
};

static struct platform_device android_pmem_device =
{
	.name = "android_pmem",
	.id = 0,
	.dev = {
		.platform_data = &pmem_data,
	},
};
#endif

#if defined(CONFIG_AML_RTC)
static	struct platform_device aml_rtc_device = {
      		.name            = "aml_rtc",
      		.id               = -1,
	};
#endif

#if defined(CONFIG_AM_DVB)
static struct resource gx1001_resource[]  = {
	[0] = {
		.start = ((GPIOA_bank_bit(13)<<16) | GPIOA_bit_bit0_14(13)),                           //frontend 0 reset pin
		.end   = ((GPIOA_bank_bit(13)<<16) | GPIOA_bit_bit0_14(13)),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset"
	},
	[1] = {
		.start = 0,                                    //frontend 0 i2c adapter id
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[2] = {
		.start = 0xC0,                                 //frontend 0 tuner address
		.end   = 0xC0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_tuner_addr"
	},
	[3] = {
		.start = 0x18,                                 //frontend 0 demod address
		.end   = 0x18,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_demod_addr"
	},
};

static  struct platform_device gx1001_device = {
	.name             = "gx1001",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(gx1001_resource),
	.resource         = gx1001_resource,
};

static struct resource amlfe_resource[]  = {

	[0] = {
		.start = 1,                                    //frontend  i2c adapter id
		.end   = 1,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[1] = {
	       // 0xC0 - Mxl5007, 0xC2 - RDA5880E
		.start = 0xC2,                                 //frontend  tuner address
		.end   = 0xC2,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_tuner_addr"
	},
	[2] = {
		.start = 1,                   //frontend   mode 0-dvbc 1-dvbt
		.end   = 1,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_mode"
	},
	[3] = {
		.start =  5,                  //frontend  tuner 0-NULL, 1-DCT7070, 2-Maxliner, 3-FJ2207, 4-TD1316, 5-RDA5880E, 6-NMI120
		.end   = 5,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_tuner"
	},
	[4] = {
		.start = 0,                   //tuner power gpio
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "tuner_power_pin"
	},
};

static  struct platform_device amlfe_device = {
	.name             = "amlfe",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(amlfe_resource),
	.resource         = amlfe_resource,
};

static struct resource amlogic_dvb_resource[]  = {
	[0] = {
		.start = ((GPIOD_bank_bit2_24(9)<<16) | GPIOD_bit_bit2_24(9)),                           //frontend 0 reset pin
		.end   = ((GPIOD_bank_bit2_24(9)<<16) | GPIOD_bit_bit2_24(9)),
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_reset"
	},
	[1] = {
		.start = 0,                                    //frontend 0 i2c adapter id
		.end   = 0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_i2c"
	},
	[2] = {
		.start = 0xC0,                                 //frontend 0 tuner address
		.end   = 0xC0,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_tuner_addr"
	},
	[3] = {
		.start = 0x18,                                 //frontend 0 demod address
		.end   = 0x18,
		.flags = IORESOURCE_MEM,
		.name  = "frontend0_demod_addr"
	},
	[4] = {
		.start = INT_DEMUX,                   //demux 0 irq
		.end   = INT_DEMUX,
		.flags = IORESOURCE_IRQ,
		.name  = "demux0_irq"
	},
	[5] = {
		.start = INT_DEMUX_1,                    //demux 1 irq
		.end   = INT_DEMUX_1,
		.flags = IORESOURCE_IRQ,
		.name  = "demux1_irq"
	},
	[6] = {
		.start = INT_DEMUX_2,                    //demux 2 irq
		.end   = INT_DEMUX_2,
		.flags = IORESOURCE_IRQ,
		.name  = "demux2_irq"
	},	
	[7] = {
		.start = INT_ASYNC_FIFO_FLUSH,                   //dvr 0 irq
		.end   = INT_ASYNC_FIFO_FLUSH,
		.flags = IORESOURCE_IRQ,
		.name  = "dvr0_irq"
	},
	[8] = {
		.start = INT_ASYNC_FIFO2_FLUSH,          //dvr 1 irq
		.end   = INT_ASYNC_FIFO2_FLUSH,
		.flags = IORESOURCE_IRQ,
		.name  = "dvr1_irq"
	},	
};

static  struct platform_device amlogic_dvb_device = {
	.name             = "amlogic-dvb",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(amlogic_dvb_resource),
	.resource         = amlogic_dvb_resource,
};
#endif

#if defined(CONFIG_AM_SMARTCARD)
static struct resource amlogic_smc_resource[]  = {
	[0] = {
		.start = ((GPIOD_bank_bit2_24(11)<<16) | GPIOD_bit_bit2_24(11)),                          //smc POWER gpio
		.end   = ((GPIOD_bank_bit2_24(11)<<16) | GPIOD_bit_bit2_24(11)),
		.flags = IORESOURCE_MEM,
		.name  = "smc0_power"
	},
	[1] = {
		.start = INT_SMART_CARD,                   //smc irq number
		.end   = INT_SMART_CARD,
		.flags = IORESOURCE_IRQ,
		.name  = "smc0_irq"
	},

};

static  struct platform_device amlogic_smc_device = {
	.name             = "amlogic-smc",
	.id               = -1,
	.num_resources    = ARRAY_SIZE(amlogic_smc_resource),
	.resource         = amlogic_smc_resource,
};
#endif

#if defined(CONFIG_CRYPTO_DEVICE_DRIVER)
static struct platform_device crypto_device = {
	.name = "crypto_device",
	.id = -1,
};
#endif

#if defined(CONFIG_SUSPEND)
typedef struct {
	char name[32];
	unsigned bank;
	unsigned bit;
	gpio_mode_t mode;
	unsigned value;
	unsigned enable;
} gpio_data_t;

#if 0

#define MAX_GPIO 0
//static gpio_data_t gpio_data[MAX_GPIO] = {
static gpio_data_t gpio_data[] = {
	// ----------------------------------- power ctrl ---------------------------------
	{"GPIOC_3 -- AVDD_EN",		GPIOC_bank_bit0_26(3),		GPIOC_bit_bit0_26(3),	GPIO_OUTPUT_MODE, 1, 1},
	{"GPIOA_7 -- BL_PWM",		GPIOA_bank_bit0_14(7),		GPIOA_bit_bit0_14(7),	GPIO_OUTPUT_MODE, 1, 1},
	{"GPIOA_6 -- VCCx2_EN",		GPIOA_bank_bit0_14(6),		GPIOA_bit_bit0_14(6),	GPIO_OUTPUT_MODE, 1, 1},
	// ----------------------------------- i2s ---------------------------------
	{"TEST_N -- I2S_DOUT",		GPIOJTAG_bank_bit(16),		GPIOJTAG_bit_bit16(16),	GPIO_OUTPUT_MODE, 1, 1},
	// ----------------------------------- wifi&bt ---------------------------------
	{"GPIOD_12 -- WL_RST_N",	GPIOD_bank_bit2_24(12), 	GPIOD_bit_bit2_24(12), 	GPIO_OUTPUT_MODE, 1, 1},
	{"GPIOD_14 -- BT/GPS_RST_N",GPIOD_bank_bit2_24(14),		GPIOD_bit_bit2_24(14), 	GPIO_OUTPUT_MODE, 1, 1},
	{"GPIOD_18 -- UART_CTS_N",	GPIOD_bank_bit2_24(18), 	GPIOD_bit_bit2_24(18), 	GPIO_OUTPUT_MODE, 1, 1},
	{"GPIOD_21 -- BT/GPS",		GPIOD_bank_bit2_24(21), 	GPIOD_bit_bit2_24(21), 	GPIO_OUTPUT_MODE, 1, 1},
	// ----------------------------------- lcd ---------------------------------
	{"GPIOC_12 -- LCD_U/D",		GPIOC_bank_bit0_26(12), 	GPIOC_bit_bit0_26(12), 	GPIO_OUTPUT_MODE, 1, 1},
	{"GPIOA_3 -- LCD_PWR_EN",	GPIOA_bank_bit0_14(3),		GPIOA_bit_bit0_14(3),	GPIO_OUTPUT_MODE, 1, 1},
};	


static void save_gpio(int port) 
{
	gpio_data[port].mode = get_gpio_mode(gpio_data[port].bank, gpio_data[port].bit);
	if (gpio_data[port].mode==GPIO_OUTPUT_MODE)
	{
		if (gpio_data[port].enable){
			printk("change %s output %d to input\n", gpio_data[port].name, gpio_data[port].value); 
			gpio_data[port].value = get_gpio_val(gpio_data[port].bank, gpio_data[port].bit);
			set_gpio_mode(gpio_data[port].bank, gpio_data[port].bit, GPIO_INPUT_MODE);
		}
		else{
			printk("no change %s output %d\n", gpio_data[port].name, gpio_data[port].value); 
		}
	}
}

static void restore_gpio(int port)
{
	if ((gpio_data[port].mode==GPIO_OUTPUT_MODE)&&(gpio_data[port].enable))
	{
		set_gpio_val(gpio_data[port].bank, gpio_data[port].bit, gpio_data[port].value);
		set_gpio_mode(gpio_data[port].bank, gpio_data[port].bit, GPIO_OUTPUT_MODE);
		// printk("%s output %d\n", gpio_data[port].name, gpio_data[port].value); 
	}
}

typedef struct {
	char name[32];
	unsigned reg;
	unsigned bits;
	unsigned enable;
} pinmux_data_t;


#define MAX_PINMUX	12

pinmux_data_t pinmux_data[MAX_PINMUX] = {
	{"HDMI", 	0, (1<<2)|(1<<1)|(1<<0), 						1},
	{"TCON", 	0, (1<<14)|(1<<11), 							1},
	{"I2S_OUT",	0, (1<<18),						 				1},
	{"I2S_CLK",	1, (1<<19)|(1<<15)|(1<<11),		 				1},
	{"SPI",		1, (1<<29)|(1<<27)|(1<<25)|(1<<23),				1},
	{"I2C",		2, (1<<5)|(1<<2),								1},
	{"SD",		2, (1<<15)|(1<<14)|(1<<13)|(1<<12)|(1<<8),		1},
	{"PWM",		2, (1<<31),										1},
	{"UART_A",	3, (1<<24)|(1<23),								0},
	{"RGB",		4, (1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0),	1},
	{"UART_B",	5, (1<<24)|(1<23),								0},
	{"REMOTE",	5, (1<<31),										1},
};

static unsigned pinmux_backup[6];

static void save_pinmux(void)
{
	int i;
	for (i=0;i<6;i++)
		pinmux_backup[i] = READ_CBUS_REG(PERIPHS_PIN_MUX_0+i);
	for (i=0;i<MAX_PINMUX;i++){
		if (pinmux_data[i].enable){
			printk("%s %x\n", pinmux_data[i].name, pinmux_data[i].bits);
			clear_mio_mux(pinmux_data[i].reg, pinmux_data[i].bits);
		}
	}
}

static void restore_pinmux(void)
{
	int i;
	for (i=0;i<6;i++)
		 WRITE_CBUS_REG(PERIPHS_PIN_MUX_0+i, pinmux_backup[i]);
}

#endif // 0
	
static void set_vccx2(int power_on)
{
//	int i;
    if (power_on){
//		restore_pinmux();
//		for (i=0;i<MAX_GPIO;i++)
//			restore_gpio(i);
		
        printk(KERN_INFO "set_vccx2 power up\n");
        set_gpio_val(GPIOA_bank_bit0_14(6), GPIOA_bit_bit0_14(6), 1);
        set_gpio_mode(GPIOA_bank_bit0_14(6), GPIOA_bit_bit0_14(6), GPIO_OUTPUT_MODE);        
        //set clk for wifi
    }
    else{
        printk(KERN_INFO "set_vccx2 power down\n");        
        set_gpio_val(GPIOA_bank_bit0_14(6), GPIOA_bit_bit0_14(6), 0);
        set_gpio_mode(GPIOA_bank_bit0_14(6), GPIOA_bit_bit0_14(6), GPIO_OUTPUT_MODE);   

//		save_pinmux();
//		for (i=0;i<MAX_GPIO;i++)
//			save_gpio(i);
    }
}

static struct meson_pm_config aml_pm_pdata = {
    .pctl_reg_base  = __void_cast( IO_APB_BUS_BASE ),
    .mmc_reg_base   = __void_cast( APB_REG_ADDR(0x1000) ),
    .hiu_reg_base   = __void_cast( CBUS_REG_ADDR(0x1000) ),
    .power_key      = 1<<15,
    .ddr_clk        = 0x00110820,
    .sleepcount     = 128,
    .set_vccx2      = set_vccx2,
    .core_voltage_adjust = 5,
};

static struct platform_device aml_pm_device = {
    .name           = "pm-meson",
    .dev = {
        .platform_data  = &aml_pm_pdata,
    },
    .id             = -1,
};
#endif
 #ifdef CONFIG_POST_PROCESS_MANAGER
static struct resource ppmgr_resources[] = {
    [0] = {
        .start =  PPMGR_ADDR_START,
        .end   = PPMGR_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};
static struct platform_device ppmgr_device = {
    .name       = "ppmgr",
    .id         = 0,
    .num_resources = ARRAY_SIZE(ppmgr_resources),
    .resource      = ppmgr_resources,
};
#endif
#ifdef CONFIG_FREE_SCALE
static struct resource freescale_resources[] = {
    [0] = {
        .start = FREESCALE_ADDR_START,
        .end   = FREESCALE_ADDR_END,
        .flags = IORESOURCE_MEM,
    },
};

static struct platform_device freescale_device =
{
    .name           = "freescale",
    .id             = 0,
    .num_resources  = ARRAY_SIZE(freescale_resources),
    .resource       = freescale_resources,
};
#endif

static struct platform_device __initdata *platform_devs[] = {
//    #if defined(CONFIG_AM_UART_WITH_S_CORE)
    #if defined(CONFIG_SERIAL_MESON_ONE)
        &aml_uart_device,
    #endif
    #if defined(CONFIG_JPEGLOGO)
	&jpeglogo_device,
    #endif	
    #if defined(CONFIG_FB_AM)
    	&fb_device,
    #endif
    #if  defined(CONFIG_AM_TV_OUTPUT)||defined(CONFIG_AM_TCON_OUTPUT)
       &vout_device,	
    #endif		
    #if defined(CONFIG_AM_STREAMING)
		&codec_device,
    #endif
    #if defined(CONFIG_AM_VIDEO)
	&deinterlace_device,
    #endif
    #if defined(CONFIG_TVIN_VDIN)
        &vdin_device,
	&bt656in_device,
    #endif
    #if defined(CONFIG_AML_AUDIO_DSP)
	&audiodsp_device,
    #endif
	&aml_sound_card,
    #if defined(CONFIG_MESON_ONE_CARDREADER)
    	&amlogic_card_device,
    #endif
    #if defined(CONFIG_KEYPADS_AM)||defined(CONFIG_VIRTUAL_REMOTE)||defined(CONFIG_KEYPADS_AM_MODULE) 
		&input_device,
    #endif	
    #if defined(CONFIG_AMLOGIC_SPI_NOR)
    	&amlogic_spi_nor_device,
    #endif
    #ifdef CONFIG_AM_NAND
		&aml_nand_device,
    #endif		
	
    #if defined(CONFIG_I2C_SW_AML)
		&aml_sw_i2c_device_fe1,
		&aml_sw_i2c_device_fe2,
    #endif
    #if defined(CONFIG_I2C_AML)
		&aml_i2c_device,
    #endif
    #if defined(CONFIG_ANDROID_PMEM)
		&android_pmem_device,
    #endif
    #if defined(CONFIG_AML_RTC)
              &aml_rtc_device,
    #endif
    #if defined(CONFIG_AM_DVB)
		&amlogic_dvb_device,
		&gx1001_device,
		&amlfe_device,
    #endif
    #if defined(CONFIG_AM_SMARTCARD)	
		&amlogic_smc_device,
    #endif
		#ifdef CONFIG_BT_DEVICE
	&bt_device,
	#endif

    #ifdef CONFIG_USB_ANDROID
               &android_usb_device,
   	#ifdef CONFIG_USB_ANDROID_MASS_STORAGE
//               &usb_mass_storage_device,
   	#endif
    #endif    
    #if defined(CONFIG_SUSPEND)
            &aml_pm_device,
    #endif
    #if defined(CONFIG_PM)
        &aml_eth_pm,
    #endif
#ifdef CONFIG_POST_PROCESS_MANAGER
    &ppmgr_device,
#endif
#ifdef CONFIG_FREE_SCALE
        &freescale_device,
#endif        
    #ifdef CONFIG_EFUSE
	&aml_efuse_device,
    #endif

    #if defined(CONFIG_CRYPTO_DEVICE_DRIVER)
       &crypto_device,
    #endif
};

#if 0
#ifdef CONFIG_PM
static void eth_clock_enable(int enable)
{
    if (enable)
        eth_clk_set(ETH_CLKSRC_APLL_CLK,400*CLK_1M,50*CLK_1M);
    else
        eth_clk_set(ETH_CLKSRC_APLL_CLK,400*CLK_1M,0);
}
#endif
#endif

static void __init eth_pinmux_init(void)
{

	/*for dpf_sz with ethernet*/	
		///GPIOC17 -int
	///GPIOC19/NA	nRst;
	printk("eth pinmux init\n");
	eth_set_pinmux(ETH_BANK2_GPIOD15_D23,ETH_CLK_OUT_GPIOD24_REG5_1,0);
	CLEAR_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, 1);
	SET_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, (1 << 1));
	SET_CBUS_REG_MASK(PREG_ETHERNET_ADDR0, 1);
	udelay(100);
	/*reset*/
	///GPIOC19/NA	nRst;
	set_gpio_mode(PREG_GGPIO,12,GPIO_OUTPUT_MODE);
	set_gpio_val(PREG_GGPIO,12,0);
	udelay(100);	//waiting reset end;
	set_gpio_val(PREG_GGPIO,12,1);
	udelay(10);	//waiting reset end;
}

static void __init device_pinmux_init(void )
{
	clearall_pinmux();

	/* other deivce power on */
	/* GPIOA_200e_bit4..usb/eth/YUV power on */
	//set_gpio_mode(PREG_EGPIO,1<<4,GPIO_OUTPUT_MODE);
	//set_gpio_val(PREG_EGPIO,1<<4,1);

	/* uart port A */
	uart_set_pinmux(UART_PORT_A,UART_A_GPIO_B2_B3);

#ifndef CONFIG_I2C_SW_AML
	/* uart port B */
	uart_set_pinmux(UART_PORT_B,UART_B_GPIO_C13_C14);
	//uart_set_pinmux(UART_PORT_B,UART_B_TCK_TDO);
#endif

	/* pinmux of eth */
	eth_pinmux_init();

	/* IR decoder pinmux */
	set_mio_mux(5, 1<<31);

#ifdef CONFIG_AM_SMARTCARD
	/* SmartCard pinmux */
	set_mio_mux(2, 0xF<<20);
#endif

	set_audio_pinmux(AUDIO_IN_JTAG); // for MIC input
    set_audio_pinmux(AUDIO_OUT_TEST_N); //External AUDIO DAC
    set_audio_pinmux(SPDIF_OUT_GPIOA);
    /* audio gpio mute control */

    set_gpio_mode(PREG_GGPIO,6,GPIO_OUTPUT_MODE);
    set_gpio_val (PREG_GGPIO,6,0);
}

static int __init eth_clk_src_index_setup(char *ptr)
{
    char *endptr;   /* local pointer to end of parsed string */

    eth_clk_src_index = simple_strtoull(ptr, &endptr, 0);

    printk("******************%s: eth_clk_src_index = %d\n", __func__, eth_clk_src_index);
    return 0;
}

__setup("eth_clksrc=", eth_clk_src_index_setup);

static void __init  device_clk_setting(void)
{
    unsigned int freq;
    unsigned int ethernet_clk_measure_index[4] = {
        SYS_PLL_DIV3, DEMOD_PLL_CLK400,
        DDR_PLL_CLK, OTHER_PLL_CLK
    };
    
    /*Demod CLK for eth and sata*/
    demod_apll_setting(0,1200*CLK_1M);
    /*eth clk*/
    //eth_clk_set(ETH_CLKSRC_SYS_D3,(900000000/2)*2/3,50*CLK_1M);
    //eth_clk_set(ETH_CLKSRC_APLL_CLK,400*CLK_1M,50*CLK_1M);
      if ((eth_clk_src_index>=0) && (eth_clk_src_index <4)) {
          freq = clk_util_clk_msr_rl(ethernet_clk_measure_index[eth_clk_src_index]);
            if(freq % 50) {  //in MHz
                  printk("******************%s:clk[%d] freq = %dMHz!! can not use this clock source!!we will set clk 1!\n", __func__, eth_clk_src_index, freq);
                  eth_clk_set(1, 400*CLK_1M, 50*CLK_1M);
            } else {
              eth_clk_set(eth_clk_src_index, freq*CLK_1M, 50*CLK_1M);
                  printk("******************%s: freq = %dMHz!! can not use this clock source!!\n", __func__, freq);
            }
      } else {
          printk("******************%s: eth_clk_src_index = %d is invalide(0-3)!!\n", __func__, eth_clk_src_index);
          
          eth_clk_set(1, 400*CLK_1M, 50*CLK_1M);
      }
}

#if 0
static void __init  device_clk_setting(void)
{
	/*Demod CLK for eth and sata*/
	demod_apll_setting(0,1200*CLK_1M);
	/*eth clk*/

    	//eth_clk_set(ETH_CLKSRC_SYS_D3,900*CLK_1M/3,50*CLK_1M);
    	eth_clk_set(ETH_CLKSRC_APLL_CLK,400*CLK_1M,50*CLK_1M);
}
#endif

static void disable_unused_model(void)
{
	CLK_GATE_OFF(VIDEO_IN);
	CLK_GATE_OFF(BT656_IN);
//	CLK_GATE_OFF(ETHERNET);
//	CLK_GATE_OFF(SATA);
	CLK_GATE_OFF(WIFI);
//	video_dac_disable();
	//audio_internal_dac_disable();
     //disable wifi
    SET_CBUS_REG_MASK(HHI_GCLK_MPEG2, (1<<5)); 
    SET_CBUS_REG_MASK(HHI_WIFI_CLK_CNTL, (1<<0));
    _outl(0xCFF,0xC9320ED8);
    _outl((_inl(0xC9320EF0))&0xF9FFFFFF,0xC9320EF0);
    CLEAR_CBUS_REG_MASK(HHI_GCLK_MPEG2, (1<<5)); 
    CLEAR_CBUS_REG_MASK(HHI_WIFI_CLK_CNTL, (1<<0));
}

static void meson_one_poweroff( void )
{
    meson_one_reset( 0, NULL );
}

static __init void m1_init_machine(void)
{
	meson_cache_init();

	device_clk_setting();
	device_pinmux_init();
	platform_add_devices(platform_devs, ARRAY_SIZE(platform_devs));

    pm_power_off = meson_one_poweroff;

    // ddd
    //
    #if 0

	//mute A18 tech 20120104
	// set_gpio_val(GPIOD_bank_bit2_24(8), GPIOD_bit_bit2_24(8), 1);
	// set_gpio_mode(GPIOD_bank_bit2_24(8), GPIOD_bit_bit2_24(8), GPIO_OUTPUT_MODE);

	// stat_en AB7 as mute function tech 2012.03.09
	set_gpio_val(GPIOD_bank_bit2_24(12), GPIOD_bit_bit2_24(12), 0);
	set_gpio_mode(GPIOD_bank_bit2_24(12), GPIOD_bit_bit2_24(12), GPIO_OUTPUT_MODE);
 
    
	
	//wifi power A19 Joe 111121
	// set_gpio_val(GPIOD_bank_bit2_24(4), GPIOD_bit_bit2_24(4), 0); // remove the wifi setting by tech 2012.03.17
	set_gpio_val(GPIOD_bank_bit2_24(4), GPIOD_bit_bit2_24(4), 1);
	set_gpio_mode(GPIOD_bank_bit2_24(4), GPIOD_bit_bit2_24(4), GPIO_OUTPUT_MODE);  

    //MTC:Liuese 2012-08-09 add for wifi hub GL850G-Y22 begin
    //hub power B19 Joe 111121
    //set_gpio_val(GPIOD_bank_bit2_24(5), GPIOD_bit_bit2_24(5), 1);
    //set_gpio_mode(GPIOD_bank_bit2_24(5), GPIOD_bit_bit2_24(5), GPIO_OUTPUT_MODE);  


//MTC:Liuese 2012-08-15 add CONFIG_USB_HUB_NO_DELAY_POWER
#ifdef CONFIG_USB_HUB_NO_DELAY_POWER
    //hub power B19 Joe 111121
    set_gpio_val(GPIOD_bank_bit2_24(5), GPIOD_bit_bit2_24(5), 1);
    set_gpio_mode(GPIOD_bank_bit2_24(5), GPIOD_bit_bit2_24(5), GPIO_OUTPUT_MODE);
    printk("MTC:m1_init_machine:GPIOD_bank_bit2_24(5) set high\n");
#else
    //MTC:Liuese 2012-08-09 add for wifi hub GL850G-Y22 begin
    set_gpio_mode(GPIOD_bank_bit2_24(5), GPIOD_bit_bit2_24(5), GPIO_OUTPUT_MODE);
    set_gpio_val(GPIOD_bank_bit2_24(5), GPIOD_bit_bit2_24(5), 0); //low
    printk("MTC:m1_init_machine:GPIOD_bank_bit2_24(5) set low\n");
   	//MTC:Liuese 2012-08-09 add for wifi hub GL850G-Y22 end
#endif
	
	//otg usb power off init C21 Joe 111122
	set_gpio_mode(GPIOC_bank_bit0_26(20),GPIOC_bit_bit0_26(20),GPIO_OUTPUT_MODE);
	set_gpio_val(GPIOC_bank_bit0_26(20),GPIOC_bit_bit0_26(20),0);

    #endif
    //
    // my board power led key
	set_gpio_val (GPIOD_bank_bit2_24(7),GPIOD_bit_bit2_24(7), 0 );
	set_gpio_mode(GPIOD_bank_bit2_24(7),GPIOD_bit_bit2_24(7),GPIO_OUTPUT_MODE);

#ifdef CONFIG_USB_DWC_OTG_HCD
	set_usb_phy_clk(USB_PHY_CLOCK_SEL_XTAL_DIV2);
	lm_device_register(&usb_ld_a);
	lm_device_register(&usb_ld_b);
#endif
#ifdef CONFIG_SATA_DWC_AHCI
	set_sata_phy_clk(SATA_PHY_CLOCK_SEL_DEMOD_PLL);
	lm_device_register(&sata_ld);
#endif

    disable_unused_model();
}

/*VIDEO MEMORY MAPING*/
//static __initdata struct map_desc meson_video_mem_desc[] = {
//	{
//		.virtual	= PAGE_ALIGN(__phys_to_virt(RESERVED_MEM_START)),
//		.pfn		= __phys_to_pfn(RESERVED_MEM_START),
//		.length		= RESERVED_MEM_END-RESERVED_MEM_START+1,
//		.type		= MT_DEVICE,
//	},
//};

static __init void m1_map_io(void)
{
	meson_map_io();
    //
    // treeyan.
    //
    //meson_video_mem_desc[0].virtual = PAGE_ALIGN(__phys_to_virt(RESERVED_MEM_START));
    //iotable_init(meson_video_mem_desc, ARRAY_SIZE(meson_video_mem_desc));
}

static __init void m1_irq_init(void)
{
	meson_init_irq();
}

static __init void m1_fixup(struct tag *tag, char **cmdline)
{
    //
    // add memblock to here.
    //
}

MACHINE_START(MESON_8726M_DVBC, "AMLOGIC MESON-M1 8726M")
//	.phys_io		= MESON_PERIPHS1_PHYS_BASE,
//	.io_pg_offst	= (MESON_PERIPHS1_PHYS_BASE >> 18) & 0xfffc,
//	.boot_params	= BOOT_PARAMS_OFFSET,
    .atag_offset    = 0x100,
	.map_io			= m1_map_io,
	.init_irq		= m1_irq_init,
//	.timer			= &meson_sys_timer,
	.init_time		= meson_timer_init,
	.init_machine	= m1_init_machine,
	.fixup			= m1_fixup,
//	.video_start	= RESERVED_MEM_START,
//	.video_end		= RESERVED_MEM_END,
    .restart        = meson_one_reset,
MACHINE_END
