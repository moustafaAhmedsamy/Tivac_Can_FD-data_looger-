

/**
 * main.c
 */
#include "driver/CanFD/CanFD_API.h"
#include "driverlib/rom_map.h"
#include <driverlib/interrupt.h>
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

int main(void)
{
    GPIODirModeSet(0,0,1);
	return 0;
}
