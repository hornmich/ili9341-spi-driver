/*
 * ili9341.h
 *
 *  Created on: 5. 6. 2019
 *      Author: Michal Horn

 */

#ifndef ILI9341_ILI9341_H_
#define ILI9341_ILI9341_H_

#define ILI9341_MAX_DRIVERS_CNT       (2)  /**< Maximal number of driver instances (displays attached). */

typedef struct ili9341_desc* ili9341_desc_ptr_t;  /**< ILI9341 driver instance descriptor. */

/* Hardware interface */
typedef int (*spi_tx_dma_nb_t)(const uint8_t*, int len);

typedef bool (*spi_tx_dma_ready_t) (void);

typedef void (*gpio_rst_pin_t)(ili9341_gpio_pin_value_t value);

typedef void (*gpio_cs_pin_t)(ili9341_gpio_pin_value_t value);

typedef void (*gpio_dc_pin_t)(ili9341_gpio_pin_value_t value);

typedef enum {
	ILI9341_ORIENTATION_HORIZONTAL,
	ILI9341_ORIENTATION_VERTICAL
} ili9341_orientation_t;

typedef enum {
	ILI9341_PIN_RESET = 0,
	ILI9341_PIN_SET = 1
} ili9341_gpio_pin_value_t;

typedef struct ili9341_cfg_st {
	uint16_t width;
	uint16_t height;
	ili9341_orientation_t orientation;
	spi_tx_dma_nb_t spi_tx_dma;
	spi_tx_dma_ready_t  spi_tx_ready;
	gpio_rst_pin_t rst_pin;
	gpio_cs_pin_t cs_pin;
	gpio_dc_pin_t dc_pin;
} ili9341_cfg_t;

typedef struct coord_2d_st {
	uint16_t x;
	uint16_t y;
} coord_2d_t;


ili9341_desc_ptr_t ili9341_init(const ili9341_cfg_t* cfg);

int ili9341_reset(const ili9341_desc_ptr_t *desc);

int ili9341_set_orientation(const ili9341_desc_ptr_t *desc, ili9341_orientation_t orientation);

int ili9341_set_region(const ili9341_desc_ptr_t *desc, coord_2d_t top_left, coord_2d_t bottom_right);

int ili9341_draw_RGB565_dma(const ili9341_desc_ptr_t desc, uint32_t size);

void ili9341_1ms_timer_cb();
#endif /* ILI9341_ILI9341_H_ */
