/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2013 Stephen Dwyer <scdwyer@ualberta.ca>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "usb.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/stm32/spi.h>
#include <stdlib.h>

static void clock_setup(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	/* Enable GPIOA, GPIOB, GPIOC clock. */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Enable clocks for GPIO port A (for GPIO_USART2_TX) and USART2. */
	//	rcc_periph_clock_enable(RCC_AFIO);

	/* Enable SPI1 Periph and gpio clocks */
	rcc_periph_clock_enable(RCC_SPI1);
}

static void spi_setup(void) {

  /* Configure GPIOs: SS=PA4, SCK=PA5, MISO=PA6 and MOSI=PA7 */
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO4 |
            								GPIO5 |
                                            GPIO7 );

  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
          GPIO6);

  /* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
  spi_reset(SPI1);

  /* Set up SPI in Master mode with:
   * Clock baud rate: 1/64 of peripheral clock frequency
   * Clock polarity: Idle High
   * Clock phase: Data valid on 2nd clock pulse
   * Data frame format: 8-bit
   * Frame format: MSB First
   */
  spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_64, SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_2, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);

  /*
   * Set NSS management to software.
   *
   * Note:
   * Setting nss high is very important, even if we are controlling the GPIO
   * ourselves this bit needs to be at least set to 1, otherwise the spi
   * peripheral will not send any data out.
   */
  spi_enable_software_slave_management(SPI1);
  spi_set_nss_high(SPI1);

  /* Enable SPI1 periph. */
  spi_enable(SPI1);
}

static void gpio_setup(void)
{
	/* Set GPIO13 (in GPIO port C) to 'output push-pull'. */
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

static void blink(uint8_t count) {
	/* LED on/off */
	gpio_set(GPIOC, GPIO13);
	for (int i = 0; i < 1600000; i++)	/* Wait a bit. */
		__asm__("nop");

	for (int i = 0; i < 15; i++) {
		if(count & (1 << i) == 0) {
			gpio_clear(GPIOC, GPIO13);
			for (int j = 0; j < 200000; j++)
				__asm__("nop");

			gpio_set(GPIOC, GPIO13);
			for (int j = 0; j < 200000; j++)
				__asm__("nop");
		}

		gpio_clear(GPIOC, GPIO13);
		for (int j = 0; j < 200000; j++)
			__asm__("nop");

		gpio_set(GPIOC, GPIO13);
		for (int i = 0; i < 800000; i++)
		  __asm__("nop");
	}
}

static void itoh(uint32_t num, char *buf, size_t buf_len) {
	const char hexstr[16] = "0123456789ABCDEF";
	for(int i = 0; i < buf_len; i++) {
		buf[i] = hexstr[(num >> (4 * i)) & 0xF];
	}
}

#define BUF_LEN 4
int main(void)
{
	uint8_t buf[BUF_LEN+1] = "0000\0";
	uint16_t rx_value = 5;
	uint16_t counter = 0;

	//	SCB_VTOR = (uint32_t) 0x08002000;

	clock_setup();
	gpio_setup();
	spi_setup();

	//	usb_vcp_init();

	gpio_set(GPIOC, GPIO13);

	for (int i = 0; i < 0x800000; i++)
		__asm__("nop");

	gpio_clear(GPIOA, GPIO4);
	spi_send(SPI1, (uint8_t) 0x1B);
	spi_send(SPI1, (uint8_t) 0x00);
	gpio_set(GPIOA, GPIO4);
	/* Blink the LED (PA8) on the board with every transmitted byte. */
	while (1) {
		//blink(rx_value);
		/* blocking send of the byte out SPI1 */
		gpio_clear(GPIOA, GPIO4);
		spi_send(SPI1, (uint8_t) 0x7F);
		spi_send(SPI1, (uint8_t) 0x00);
		gpio_set(GPIOA, GPIO4);
		/* Read the byte that just came in (use a loopback between MISO and MOSI
		 * to get the same byte back)
		 */
		rx_value = spi_read((uint8_t)SPI1);

		if(rx_value == 0x73) {
			gpio_clear(GPIOC, GPIO13);
			for (int i = 0; i < 800000; i++)
				__asm__("nop");
			gpio_set(GPIOC, GPIO13);
			for (int i = 0; i < 800000; i++)
				__asm__("nop");
		}

		counter++;
		
		//		itoh(rx_value, buf, BUF_LEN);
		//		usb_vcp_send_strn_cooked(buf, BUF_LEN);
	}

	return 0;
}
