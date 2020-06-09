/*
 * ili9341.c
 *
 *  Created on: 5. 6. 2019
 *      Author: Michal Horn
 */

#include "ili9341.h"
#include "ili9341_spi_cmds.h"

/**
 * Definition of ili9341 driver instance descriptor.
 *
 * Since the descriptor itself is defined as ADT - using incomplete data type in
 * the header file, all following data is hidden from the driver user.
 */
struct ili9341_desc {
	uint16_t default_width;
	uint16_t default_height;
	uint16_t current_width;
	uint16_t current_height;
	ili9341_orientation_t default_orientation;
	ili9341_orientation_t current_orientation;
	spi_tx_dma_nb_t spi_tx_dma;
	spi_tx_dma_ready_t  spi_tx_ready;
	gpio_rst_pin_t rst_pin;
	gpio_cs_pin_t cs_pin;
	gpio_dc_pin_t dc_pin;
	uint32_t timeout_ms;
	uint32_t restart_delay_ms;
	uint32_t wup_delay_ms;
	uint32_t curr_time_cnt;
};

/**
 * Driver instances pool
 */
struct ili9341_drivers_pool_st {
  struct ili9341_desc drivers[ILI9341_MAX_INSTANCES_CNT];
  uint8_t current_driver;
};

/**
 * Memory pool for driver instalces allocation. To not use malloc and heap.
 */
static struct ili9341_drivers_pool_st ili9341_drivers_pool;


/* Private methods. */
int _ili9341_init_display(const ili9341_desc_ptr_t desc) {
	int err = ILI9341_SUCCESS;
	_ili9341_enable(desc);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_SWRESET);
	_ili9341_delay_ms(desc->restart_delay_ms);

	err |= _ili9341_write_cmd(desc, ILI9341_CMD_PWCTRLA);
	err |= _ili9341_write_byte(desc, 0x39);
	err |= _ili9341_write_byte(desc, 0x2C);
	err |= _ili9341_write_byte(desc, 0x00);
	err |= _ili9341_write_byte(desc, 0x34);
	err |= _ili9341_write_byte(desc, 0x02);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_PWCTRLB);
	err |= _ili9341_write_byte(desc, 0x00);
	err |= _ili9341_write_byte(desc, 0xC1);
	err |= _ili9341_write_byte(desc, 0x30);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_TIMCTRLA);
	err |= _ili9341_write_byte(desc, 0x85);
	err |= _ili9341_write_byte(desc, 0x00);
	err |= _ili9341_write_byte(desc, 0x78);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_TIMCTRLB);
	err |= _ili9341_write_byte(desc, 0xEA);
	err |= _ili9341_write_byte(desc, 0x00);
	err |= _ili9341_write_byte(desc, 0x00);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_PONSEQCTRL);
	err |= _ili9341_write_byte(desc, 0x64);
	err |= _ili9341_write_byte(desc, 0x03);
	err |= _ili9341_write_byte(desc, 0x12);
	err |= _ili9341_write_byte(desc, 0x81);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_PUMPRATCTRL);
	err |= _ili9341_write_byte(desc, 0x20);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_PWCTR1);
	err |= _ili9341_write_byte(desc, 0x23);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_PWCTR2);
	err |= _ili9341_write_byte(desc, 0x10);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_VMCTR1);
	err |= _ili9341_write_byte(desc, 0x3E);
	err |= _ili9341_write_byte(desc, 0x28);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_VMCTR2);
	err |= _ili9341_write_byte(desc, 0x86);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_MADCTL);
	err |= _ili9341_write_byte(desc, 0x48);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_PIXFMT);
	err |= _ili9341_write_byte(desc, 0x55);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_FRMCTR1);
	err |= _ili9341_write_byte(desc, 0x00);
	err |= _ili9341_write_byte(desc, 0x18);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_DFUNCTR);
	err |= _ili9341_write_byte(desc, 0x08);
	err |= _ili9341_write_byte(desc, 0x82);
	err |= _ili9341_write_byte(desc, 0x27);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_3GAMMACTRL);
	err |= _ili9341_write_byte(desc, 0x00);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_GAMMASET);
	err |= _ili9341_write_byte(desc, 0x01);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_GMCTRP1);
	err |= _ili9341_write_byte(desc, 0x0F);
	err |= _ili9341_write_byte(desc, 0x31);
	err |= _ili9341_write_byte(desc, 0x2B);
	err |= _ili9341_write_byte(desc, 0x0C);
	err |= _ili9341_write_byte(desc, 0x0E);
	err |= _ili9341_write_byte(desc, 0x08);
	err |= _ili9341_write_byte(desc, 0x4E);
	err |= _ili9341_write_byte(desc, 0xF1);
	err |= _ili9341_write_byte(desc, 0x37);
	err |= _ili9341_write_byte(desc, 0x07);
	err |= _ili9341_write_byte(desc, 0x10);
	err |= _ili9341_write_byte(desc, 0x03);
	err |= _ili9341_write_byte(desc, 0x0E);
	err |= _ili9341_write_byte(desc, 0x09);
	err |= _ili9341_write_byte(desc, 0x00);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_GMCTRN1);
	err |= _ili9341_write_byte(desc, 0x00);
	err |= _ili9341_write_byte(desc, 0x0E);
	err |= _ili9341_write_byte(desc, 0x14);
	err |= _ili9341_write_byte(desc, 0x03);
	err |= _ili9341_write_byte(desc, 0x11);
	err |= _ili9341_write_byte(desc, 0x07);
	err |= _ili9341_write_byte(desc, 0x31);
	err |= _ili9341_write_byte(desc, 0xC1);
	err |= _ili9341_write_byte(desc, 0x48);
	err |= _ili9341_write_byte(desc, 0x08);
	err |= _ili9341_write_byte(desc, 0x0F);
	err |= _ili9341_write_byte(desc, 0x0C);
	err |= _ili9341_write_byte(desc, 0x31);
	err |= _ili9341_write_byte(desc, 0x36);
	err |= _ili9341_write_byte(desc, 0x0F);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_SLPOUT);
	_ili9341_delay_ms(desc->wup_delay_ms);
	err |= _ili9341_write_cmd(desc, ILI9341_CMD_DISPON);
	err |= ili9341_set_orientation(desc, desc->default_orientation);

	return err;
}

void _ili9341_enable(const ili9341_desc_ptr_t desc) {
	desc->rst_pin(ILI9341_PIN_SET);
}

int _ili9341_write_cmd(const ili9341_desc_ptr_t desc, ili9341_cmd_t command) {
	int err = ILI9341_SUCCESS;

	desc->dc_pin(ILI9341_PIN_RESET);
	desc->cs_pin(ILI9341_PIN_RESET);
	err |= _ili9341_wait_for_spi_ready(desc);
	err |= desc->spi_tx_dma(&command, ILI9341_CMD_LEN);
	err |= _ili9341_wait_for_spi_ready(desc);
	desc->cs_pin(ILI9341_PIN_SET);

	return err;
}

int _ili9341_write_byte(const ili9341_desc_ptr_t desc, uint8_t byte) {
	int err = ILI9341_SUCCESS;

	desc->dc_pin(ILI9341_PIN_SET);
	desc->cs_pin(ILI9341_PIN_RESET);
	err |= _ili9341_wait_for_spi_ready(desc);
	err |= desc->spi_tx_dma(&byte, 1);
	err |= _ili9341_wait_for_spi_ready(desc);
	desc->cs_pin(ILI9341_PIN_SET);

	return err;
}

int _ili9341_write_data(const ili9341_desc_ptr_t desc, const uint8_t* buffer, uint32_t size) {
	int err = ILI9341_SUCCESS;

	desc->dc_pin(ILI9341_PIN_SET);
	desc->cs_pin(ILI9341_PIN_RESET);
	err |= _ili9341_wait_for_spi_ready(desc);
	err |= desc->spi_tx_dma(&byte, size);
	err |= _ili9341_wait_for_spi_ready(desc);
	desc->cs_pin(ILI9341_PIN_SET);

	return err;
}

int _ili9341_wait_for_spi_ready(const ili9341_desc_ptr_t desc) {
	desc->curr_time_cnt = 0;
	bool timeout, tx_ready = false;
	do {
		tx_ready = desc->spi_tx_ready();
		timeout = (desc->curr_time_cnt >= desc->timeout_ms);
	} while(!tx_ready && !timeout);

	if (timeout) {
		return -ILI9341_ERR_COMM_TIMEOUT;
	}

	return ILI9341_SUCCESS;
}

/* Public interface methods. */
ili9341_desc_ptr_t ili9341_init(const ili9341_cfg_t* cfg) {
	  if (cfg == NULL ||
		  cfg->spi_tx_dma == NULL ||
		  cfg->spi_tx_ready == NULL ||
		  cfg->rst_pin == NULL ||
		  cfg->cs_pin == NULL ||
		  cfg->dc_pin == NULL) {
	      return NULL;
	  }

	  if (ili9341_drivers_pool.current_ili9341_driver >= ILI9341_MAX_INSTANCES_CNT) {
	      return NULL;
	  }

	  ili9341_desc_ptr_t driver_desc = &ili9341_drivers_pool.drivers[ili9341_drivers_pool.current_driver++];
	  driver_desc->default_width = cfg->width;
	  driver_desc->default_height = cfg->height;
	  driver_desc->current_width = cfg->width;
	  driver_desc->current_height = cfg->height;
	  driver_desc->default_orientation = cfg->orientation;
	  driver_desc->current_orientation = cfg->orientation;

	  driver_desc->spi_tx_dma = cfg->spi_tx_dma;
	  driver_desc->spi_tx_ready = cfg->spi_tx_ready;
	  driver_desc->rst_pin = cfg->rst_pin;
	  driver_desc->cs_pin = cfg->cs_pin;
	  driver_desc->dc_pin = cfg->dc_pin;

	  driver_desc->timeout_ms = cfg->timeout_ms;
	  driver_desc->restart_delay_ms = cfg->restart_delay_ms;
	  driver_desc->wup_delay_ms = cfg->wup_delay_ms;
	  driver_desc->curr_time_cnt = 0;

	  if (_ili9341_init_display(desc) < 0) {
		  return NULL;
	  }

	  return driver_desc;
}

int ili9341_set_orientation(const ili9341_desc_ptr_t desc, ili9341_orientation_t orientation) {
	int err = ILI9341_SUCCESS;

	switch (orientation) {
	case ILI9341_ORIENTATION_VERTICAL:
		err |= _ili9341_write_cmd(*desc, ILI9341_CMD_MADCTL);
		desc->current_width = desc->default_width;
		desc->current_height = desc->default_height;
		err |= _ili9341_write_byte(desc, 0x40|0x08);
		break;
	case ILI9341_ORIENTATION_HORIZONTAL:
		err |= _ili9341_write_cmd(*desc, ILI9341_CMD_MADCTL);
		desc->current_width = desc->default_height;
		desc->current_height = desc->default_width;
		err |= _ili9341_write_byte(desc, 0x40|0x80|0x20|0x08);
	default:
		err = -ILI9341_ERR_INV_PARAM;
		break;
	}

	return err;
}

int ili9341_set_region(const ili9341_desc_ptr_t desc, coord_2d_t top_left, coord_2d_t bottom_right) {
	int err = ILI9341_SUCCESS;

	err |= _ili9341_write_cmd(desc, ILI9341_CMD_CASET);
	err |= _ili9341_write_byte(desc, top_left.x >> 8);
	err |= _ili9341_write_byte(desc, top_left.x);
	err |= _ili9341_write_byte(desc, bottom_right.x >> 8);
	err |= _ili9341_write_byte(desc, bottom_right.x);
	err |= _ili9341_write_cmd(*desc, ILI9341_CMD_PASET);
	err |= _ili9341_write_byte(desc, top_left.y >> 8);
	err |= _ili9341_write_byte(desc, top_left.y);
	err |= _ili9341_write_byte(desc, bottom_right.y >> 8);
	err |= _ili9341_write_byte(desc, bottom_right.y);
	err |= _ili9341_write_cmd(*desc, ILI9341_CMD_RAMWR);

	return err;
}

void ili9341_1ms_timer_cb() {
	for (int i = 0; i < ili9341_drivers_pool.current_driver; i++) {
		ili9341_drivers_pool.drivers[i].curr_time_cnt++;
	}
}

int ili9341_draw_RGB565_dma(const ili9341_desc_ptr_t desc, uint32_t size) {
	int err = ILI9341_SUCCESS;

	err |= _ili9341_write_data(desc, buffer, size)

	return err;
}



