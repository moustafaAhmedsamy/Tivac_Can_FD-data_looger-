/*
 * gpios.c
 *
 *  Created on: Mar 17, 2021
 *      Author: mostafa
 */




int8_t DRV_CANFDSPI_GpioModeConfigure(CANFDSPI_MODULE_ID index,
        GPIO_PIN_MODE gpio0, GPIO_PIN_MODE gpio1)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read
    a = cREGADDR_IOCON + 3;
    REG_IOCON iocon;
    iocon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &iocon.byte[3]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    iocon.bF.PinMode0 = gpio0;
    iocon.bF.PinMode1 = gpio1;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, iocon.byte[3]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_GpioDirectionConfigure(CANFDSPI_MODULE_ID index,
        GPIO_PIN_DIRECTION gpio0, GPIO_PIN_DIRECTION gpio1)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read
    a = cREGADDR_IOCON;
    REG_IOCON iocon;
    iocon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &iocon.byte[0]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    iocon.bF.TRIS0 = gpio0;
    iocon.bF.TRIS1 = gpio1;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, iocon.byte[0]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_GpioStandbyControlEnable(CANFDSPI_MODULE_ID index)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read
    a = cREGADDR_IOCON;
    REG_IOCON iocon;
    iocon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &iocon.byte[0]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    iocon.bF.XcrSTBYEnable = 1;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, iocon.byte[0]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_GpioStandbyControlDisable(CANFDSPI_MODULE_ID index)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read
    a = cREGADDR_IOCON;
    REG_IOCON iocon;
    iocon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &iocon.byte[0]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    iocon.bF.XcrSTBYEnable = 0;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, iocon.byte[0]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_GpioInterruptPinsOpenDrainConfigure(CANFDSPI_MODULE_ID index,
        GPIO_OPEN_DRAIN_MODE mode)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read
    a = cREGADDR_IOCON + 3;
    REG_IOCON iocon;
    iocon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &iocon.byte[3]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    iocon.bF.INTPinOpenDrain = mode;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, iocon.byte[3]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_GpioTransmitPinOpenDrainConfigure(CANFDSPI_MODULE_ID index,
        GPIO_OPEN_DRAIN_MODE mode)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read
    a = cREGADDR_IOCON + 3;
    REG_IOCON iocon;
    iocon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &iocon.byte[3]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    iocon.bF.TXCANOpenDrain = mode;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, iocon.byte[3]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_GpioPinSet(CANFDSPI_MODULE_ID index,
        GPIO_PIN_POS pos, GPIO_PIN_STATE latch)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read
    a = cREGADDR_IOCON + 1;
    REG_IOCON iocon;
    iocon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &iocon.byte[1]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    switch (pos) {
        case GPIO_PIN_0:
            iocon.bF.LAT0 = latch;
            break;
        case GPIO_PIN_1:
            iocon.bF.LAT1 = latch;
            break;
        default:
            return -1;
            break;
    }

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, iocon.byte[1]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_GpioPinRead(CANFDSPI_MODULE_ID index,
        GPIO_PIN_POS pos, GPIO_PIN_STATE* state)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read
    a = cREGADDR_IOCON + 2;
    REG_IOCON iocon;
    iocon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &iocon.byte[2]);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    switch (pos) {
        case GPIO_PIN_0:
            *state = (GPIO_PIN_STATE) iocon.bF.GPIO0;
            break;
        case GPIO_PIN_1:
            *state = (GPIO_PIN_STATE) iocon.bF.GPIO1;
            break;
        default:
            return -1;
            break;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_GpioClockOutputConfigure(CANFDSPI_MODULE_ID index,
        GPIO_CLKO_MODE mode)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read
    a = cREGADDR_IOCON + 3;
    REG_IOCON iocon;
    iocon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &iocon.byte[3]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    iocon.bF.SOFOutputEnable = mode;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, iocon.byte[3]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

