# ili9341-spi-driver

Simple Hardware and platform independent driver for SPI ILI9341 display controller.

## Features

This is a simple driver for setting up and controlling the ILI9341 display
drivers. Currently the following features are supported:

* Multidisplay support
* Complete Power ON configuration
* Hardware abstraction for easy porting
* Basic graphics operations
* Basic display manipulations

### Multidisplay suport

The driver is designed in such way that several instances of the driver can be
created and used independently for devices with more than one display.

Instantiating is done simply by definig a handler of type *ili9341_desc_ptr_t*
and calling *ili9341_init* function with configuration passed as parameters.

    ili9341_desc_ptr_t display;
    const ili9341_cfg_t display_cfg = { ... }
    const ili9341_hw_cfg_t hw_cfg = { ... }
    display = ili9341_init(&display_cfg, &hw_cfg);
    if (display == NULL) {
       Error_Handler();
    }

The result of such call is either NULL in case of error or valid ILI9341 driver
instance that can be used when calling any other driver API functions.

The maximal number of instances is configurable by *ILI9341_MAX_DRIVERS_CNT*
macro defined in ili9341.h.

### Complete Power ON configuration

The ILI9341 requires certain configuration to be done when powering on. Such
configuration can vary from device to device. The ili9341 driver implements
complete ILI9341 chip register set description and allows for easy 'first shot'
configuration by using a default or to modify easily every single register field.

The usage of the default configuration that is functional e.g. on
STM32F4-DISCOVERY kit describes the following snippet:

    const ili9341_hw_cfg_t hw_cfg = ili9341_get_default_hw_cfg();

The easiest way of getting chip cpnfiguration and modify is to get the default
first and then modify the fields of interest:

    const ili9341_hw_cfg_t hw_cfg = ili9341_get_default_hw_cfg();
    hw_cfg.pwctrla.fields.vbc = 0b011; /* Set DDVDH to 5.5V */

When done with the configuration, pass the structure to the *ili9341_init*
function when instantiating driver.

###  Hardware abstraction

The driver code is not tied to any platform, MCU or compiler. To use the driver on your target, you need simply to:
* Register GPIO and SPI handlers in the ili9341_cfg_t.
* Call 1ms timer callback *ili9341_1ms_timer_cb* in your Timer interrupt.

See more in the **Usage** section of the README.

### Basic graphics operations

The following basic graphics operations are implemented:

* Fill dispay region with solid color
* Draw RGBA565 bitmap

These functions can be combined with the display manipulation functions, e.g.
bitmap can be drawn on a predefined display region with proper rotations.

### Basic display manipulations

The following display manipulations are available:

* Display rotations
* Drawing region selection.

## Usage

Installing and running the driver consists of the follwing steps:

* Clone the repository to your project, possibly as a submodule for easy updating.
* Register the path to the *ili9341.h* header file to your toolchain
* Include the main header file ili9341.h
* Define handlers for mannipulating GPIOs and SPI transmission
* Register 1ms timer callback
* Fill in the driver configuration
* Instantiate driver and use it.

The possible usage may look like that:

    #include "ili9341.h"

    /* Handler functions required by ILI9341 driver */
    void gpio_cs_pin (ili9341_gpio_pin_value_t value);
    void gpio_dc_pin (ili9341_gpio_pin_value_t value);
    void gpio_rst_pin (ili9341_gpio_pin_value_t value);
    bool spi_tx_dma_ready (void);
    int spi_tx_dma_b (const uint8_t* data, uint32_t len);

    int main(void)
    {
        ili9341_desc_ptr_t display;
        const ili9341_cfg_t display_cfg = {
            .cs_pin = gpio_cs_pin,
            .dc_pin = gpio_dc_pin,
            .rst_pin = gpio_rst_pin,
            .spi_tx_dma = spi_tx_dma_b,
            .spi_tx_ready = spi_tx_dma_ready,
            .orientation = ILI9341_ORIENTATION_HORIZONTAL,
            .width = 320,
            .height = 240,
            .timeout_ms = 10000,
            .wup_delay_ms = 20,
            .restart_delay_ms = 20
        };

        const ili9341_hw_cfg_t hw_cfg = ili9341_get_default_hw_cfg();

        ... Some other init code needed by your target ...

        display = ili9341_init(&display_cfg, &hw_cfg);
        if (display == NULL) {
            Error_Handler();
        }

        /* Fill the screen with RED color */
        coord_2d_t top_left, bottom_right;
        top_left.x = 0;
        top_left.y = 0;
        bottom_right.x = ili9341_get_screen_width(display);
        bottom_right.y = ili9341_get_screen_height(display);
        ili9341_set_region(display, top_left, bottom_right);
        ili9341_fill_region(display, RED);

        while (1) {
            ... Some application code ...
        }
    }

    /* Handlers definitions in pseudocode. */
    void gpio_cs_pin (ili9341_gpio_pin_value_t value) {
        GPIO_WritePin(CSX_Port, CSX_Pin, value);
    }

    void gpio_dc_pin (ili9341_gpio_pin_value_t value) {
        GPIO_WritePin(WRX_DCX_Port, WRX_DCX_Pin, value);
    }

    void gpio_rst_pin (ili9341_gpio_pin_value_t value) {
        GPIO_WritePin(RST_NC_Port, RST_NC_Pin, value);
    }

    bool spi_tx_dma_ready (void) {
        return SPI_GetState(used_spi) == SPI_STATE_READY;
    }

    int spi_tx_dma_b (const uint8_t* data, uint32_t len) {
        HAL_SPI_Transmit_DMA(spi_used, data, len);
        return 0;
    }

    /* 1MS Timer callback */
    void Timer_elapsed_interrupt () {
        /* Update Display driver timers. */
        ili9341_1ms_timer_cb();
    }

## Examples

* [ili9341-spi-example](...) - STM32F4-DISCOVERY kit example
