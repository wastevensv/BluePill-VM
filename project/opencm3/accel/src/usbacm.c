/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
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

#include "usbser.h"
#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/cm3/scb.h>

int cdcacm_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf,
		uint16_t *len, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
	(void)complete;
	(void)buf;
	(void)usbd_dev;

	switch (req->bRequest) {
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
		/*
		 * This Linux cdc_acm driver requires this to be implemented
		 * even though it's optional in the CDC spec, and we don't
		 * advertise it in the ACM functional descriptor.
		 */
		char local_buf[10];
		struct usb_cdc_notification *notif = (void *)local_buf;

		/* We echo signals back to host as notification. */
		notif->bmRequestType = 0xA1;
		notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
		notif->wValue = 0;
		notif->wIndex = 0;
		notif->wLength = 2;
		local_buf[8] = req->wValue & 3;
		local_buf[9] = 0;
		// usbd_ep_write_packet(0x83, buf, 10);
		return 1;
		}
	case USB_CDC_REQ_SET_LINE_CODING:
		if (*len < sizeof(struct usb_cdc_line_coding))
			return 0;
		return 1;
	}
	return 0;
}

void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)ep;
	(void)usbd_dev;

	char buf[64];
	int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);

	if (len) {
		//for(int i = 0; i < len; i++) {
		//	if(buf[i] > 'a' && buf[i] < 'z') {
		//		buf[i] = (((buf[i] - 'a') + 13) % ('Z'-'A'-1)) + 'a';
		//	} else if(buf[i] > 'A' && buf[i] < 'Z') {
		//		buf[i] = (((buf[i] - 'A') + 13) % ('Z'-'A'-1)) + 'A';
		//	}
		//}
		usbd_ep_write_packet(usbd_dev, 0x82, buf, len);
		buf[len] = 0;
	}
}

void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	(void)wValue;
	(void)usbd_dev;

	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, cdcacm_data_rx_cb);
	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);
	usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

	usbd_register_control_callback(
				usbd_dev,
				USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				cdcacm_control_request);
}
