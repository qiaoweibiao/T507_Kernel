#ifndef __WCN_PORT_H
#define __WCN_PORT_H

struct wcn_port_t {
	int pin_reset;
	int pin_chip_en;
	int pin_int_ap;
	int pin_wakeup_ap;
	int sdio_bus;
	bool cfg_adma_rx;
	bool cfg_adma_tx;
	bool cfg_pwrseq;
	bool cfg_data_irq;
	char *btwf_path;
	char *gnss_path;
	void (*plat_set_power)(bool op);
};

extern struct wcn_port_t wcn_port;
int wcn_port_init(void);

#endif
