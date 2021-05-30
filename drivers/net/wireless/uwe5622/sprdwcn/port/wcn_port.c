#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>

#include "wcn_port.h"
#include "wcn_dbg.h"

#define SPRD_ADMA_TX           1
#define SPRD_ADMA_RX           1
#define SPRD_PWRSEQ            0
#define SPRD_DATA_IRQ          1

#define SPRD_BTWF_FILE_NAME    "/vendor/etc/firmware/wcnmodem.bin"
#define SPRD_GNSS_FILE_NAME    "/vendor/etc/firmware/gnssmodem.bin"

#define RESET_PIN_COMPATIBLE   "allwinner,sunxi-bt"
#define CHIP_EN_PIN_COMPATIBLE "allwinner,sunxi-wlan"
#define INT_AP_PIN_COMPATIBLE  "allwinner,sunxi-btlpm"

#define RESET_PIN_ALIAS        "bt_rst_n"
#define CHIP_EN_PIN_ALIAS      "wlan_regon"
#define INT_AP_PIN_ALIAS       "bt_hostwake"

#define SDIOHAL_RES_COMPATIBLE "allwinner,sunxi-wlan"
#define SDIOHAL_PIN_ALIAS      "wlan_hostwake"
#define SDIOHAL_BUS_NUM_ALIAS  "wlan_busnum"

extern void sunxi_wlan_set_power(bool on);
struct wcn_port_t wcn_port;

int wcn_port_init(void)
{
	struct device_node *np;
	struct wcn_port_t  *data = &wcn_port;

	/* reset pin */
	np = of_find_compatible_node(NULL, NULL, RESET_PIN_COMPATIBLE);
	if (!np) {
		WCN_ERR("dts node for reset pin not found");
		return -1;
	}

	data->pin_reset = of_get_named_gpio(np,
			RESET_PIN_ALIAS, 0);

	/* chip_en pin */
	np = of_find_compatible_node(NULL, NULL, CHIP_EN_PIN_COMPATIBLE);
	if (!np) {
		WCN_ERR("dts node for chip_en not found");
		return -1;
	}

	data->pin_chip_en = of_get_named_gpio(np,
			CHIP_EN_PIN_ALIAS, 0);

	/* int_ap pin */
	np = of_find_compatible_node(NULL, NULL, INT_AP_PIN_COMPATIBLE);
	if (!np) {
		WCN_ERR("dts node for int_ap not found");
		return -1;
	}

	data->pin_int_ap = of_get_named_gpio(np,
			INT_AP_PIN_ALIAS, 0);

	/* wakeup_ap pin & sdio_bus */
	np = of_find_compatible_node(NULL, NULL, SDIOHAL_RES_COMPATIBLE);
	if (!np) {
		WCN_ERR("dts node for sdio hal not found");
		return -1;
	}

	data->pin_wakeup_ap = of_get_named_gpio(np, SDIOHAL_PIN_ALIAS, 0);

	if (of_property_read_u32(np, SDIOHAL_BUS_NUM_ALIAS, &data->sdio_bus)) {
		WCN_ERR("%s not found", SDIOHAL_BUS_NUM_ALIAS);
		return -1;
	}

	data->btwf_path = SPRD_BTWF_FILE_NAME;
	data->gnss_path = SPRD_GNSS_FILE_NAME;

	data->cfg_adma_rx  = SPRD_ADMA_RX  ? true : false;
	data->cfg_adma_tx  = SPRD_ADMA_TX  ? true : false;
	data->cfg_pwrseq   = SPRD_PWRSEQ   ? true : false;
	data->cfg_data_irq = SPRD_DATA_IRQ ? true : false;

	data->plat_set_power = sunxi_wlan_set_power;

	return 0;
}
