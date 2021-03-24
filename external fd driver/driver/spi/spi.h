#ifndef _SPI_H
#define _SPI_H

#include <stdint.h>


#include "driverlib/rom_map.h"
#include <driverlib/interrupt.h>
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"


#define ENC_CS_PORT         GPIO_PORTA_BASE
#define ENC_INT_PORT        GPIO_PORTD_BASE
//#define ENC_RESET_PORT        GPIO_PORTA_BASE
#define ENC_CS          GPIO_PIN_7
#define ENC_INT         GPIO_PIN_6
//#define ENC_RESET     GPIO_PIN_2

uint8_t spi_send(uint8_t c);
void spi_init(void);
void gpio_comm_init(void);
#endif
