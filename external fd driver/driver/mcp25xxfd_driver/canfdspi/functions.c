/*
 * functions.c
 *
 *  Created on: Mar 9, 2021
 *      Author: mostafa
 */




/*
 *
 *  the passed buffer is already established
 *  this interface should only send on the bus
 */
void SPI_Write_Data(const uint8_t *buf, uint16_t count)
{
        MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
        int i;
        for (i = 0; i < count; i++) {
            spi_send(*buf);
            buf++;
        }
        MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}

void SPI_Read_Data_Byte(uint8_t *buf , uint8_t byte ) {
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(buf[0]);
    spi_send(buf[1]);
    byte = spi_send(buf[2]);
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}

void SPI_Read_Data_Word(uint8_t *buf , uint32_t byte ) {
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(buf[0]);
    spi_send(buf[1]);
    byte = spi_send(buf[2]);
    byte = spi_send(buf[3])<<8;
    byte = spi_send(buf[4])<<16;
    byte = spi_send(buf[5])<<24;
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}
void SPI_Read_Data_Half_Word(uint8_t *buf , uint16t Half_word )
{
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(buf[0]);
    spi_send(buf[1]);
    Half_word = spi_send(buf[2]);
    Half_word = spi_send(buf[3])<<8;

    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}



                                                                                  /////////////////////////////////////////////////////////////////////////////////////
                                                                                  /////////////////////////////////////////////////////////////////////////////////////
                                                                                  /////////////////////////////////////////////////////////////////////////////////////

// Section: Reset

int8_t DRV_CANFDSPI_Reset(void )
{
    uint16_t spiTransferSize = 2;
    int8_t spiTransferError = 0;            txObj.word[0] = 0;
    txObj.word[1] = 0;

    txObj.bF.id.SID = TX_RESPONSE_ID;
    txObj.bF.id.EID = 0;

    txObj.bF.ctrl.BRS = 1;
    txObj.bF.ctrl.DLC = CAN_DLC_64;
    txObj.bF.ctrl.FDF = 1;
    txObj.bF.ctrl.IDE = 0;

    // Configure message data
    int i;
    for (i = 0; i < MAX_DATA_BYTES; i++) txd[i] = txObj.bF.id.SID + i;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) (cINSTRUCTION_RESET << 4);
    spiTransmitBuffer[1] = 0;

    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(spiTransmitBuffer[0]);
    spi_send(spiTransmitBuffer[1]);
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);

    return spiTransferError;
}


// *****************************************************************************
// *****************************************************************************
// Section: SPI Access Functions

int8_t DRV_CANFDSPI_ReadByte( uint16_t address, uint8_t *rxd)
{
    uint16_t spiTransferSize = 3;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);
    spiTransmitBuffer[2] = 0;

    // Update data
    uint8_t byte =0;
    SPI_Read_Data_Byte(spiTransmitBuffer , byte)
    *rxd = byte;
}

int8_t DRV_CANFDSPI_WriteByte(uint16_t address, uint8_t txd)
{
    uint16_t spiTransferSize = 3;
    int8_t spiTransferError = 0;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);
    spiTransmitBuffer[2] = txd;

    int8_t spiTransferError = DRV_SPI_Write_Data(spiTransmitBuffer,spiTransferSize);// buffer is already established this interface should only send on the bus
    return spiTransferError;
}

void  DRV_CANFDSPI_ReadWord(uint16_t address, uint32_t *rxd)
{
    uint8_t i;
    uint32_t x;
    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    SPI_Read_Data_Word(uint8_t *buf , *rxd );

}

void DRV_CANFDSPI_WriteWord( uint16_t address,uint32_t txd)
{
    uint8_t i;
    uint16_t spiTransferSize = 6;
    int8_t spiTransferError = 0;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    // Split word into 4 bytes and add them to buffer
    for (i = 0; i < 4; i++) {
        spiTransmitBuffer[i + 2] = (uint8_t) ((txd >> (i * 8)) & 0xFF);
    }

    SPI_Write_Data(spiTransmitBuffer, spiTransferSize);

}

int8_t DRV_CANFDSPI_ReadHalfWord(uint16_t address, uint16_t *rxd)
{
    uint8_t i;
    uint32_t x;
    uint16_t spiTransferSize = 4;
    int8_t spiTransferError = 0;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    SPI_Read_Data_Half_Word(spiTransmitBuffer , *rxd );

}

int8_t DRV_CANFDSPI_WriteHalfWord(uint16_t address,uint16_t txd)
{
    uint8_t i;
    uint16_t spiTransferSize = 4;
    int8_t spiTransferError = 0;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    // Split word into 2 bytes and add them to buffer
    for (i = 0; i < 2; i++) {
        spiTransmitBuffer[i + 2] = (uint8_t) ((txd >> (i * 8)) & 0xFF);
    }
    SPI_Write_Data(spiTransmitBuffer, spiTransferSize);

    return spiTransferError;
}








/////////////////////////////////////////////////////////////////////////////////////
                                    /////////////////////////////////////////////////////////////////////////////////////
                                                        /////////////////////////////////////////////////////////////////////////////////////









// Section: ECC

int8_t DRV_CANFDSPI_EccEnable(void)
{
    int8_t spiTransferError = 0;
    uint8_t d = 0;

    // Read
    DRV_CANFDSPI_ReadByte(cREGADDR_ECCCON, &d);

    // Modify
    d |= 0x01;

    // Write
    DRV_CANFDSPI_WriteByte(cREGADDR_ECCCON , d);
    return 0;
}



void APP_CANFDSPI_Init()
{
    // Reset device
    DRV_CANFDSPI_Reset();

    // Enable ECC and initialize RAM
    DRV_CANFDSPI_EccEnable();

    if (!ramInitialized) {
        DRV_CANFDSPI_RamInit(DRV_CANFDSPI_INDEX_0, 0xff);
        ramInitialized = true;
    }

    // Configure device
    DRV_CANFDSPI_ConfigureObjectReset(&config);
    config.IsoCrcEnable = 1;
    config.StoreInTEF = 0;

    DRV_CANFDSPI_Configure(DRV_CANFDSPI_INDEX_0, &config);

    // Setup TX FIFO
    DRV_CANFDSPI_TransmitChannelConfigureObjectReset(&txConfig);
    txConfig.FifoSize = 7;
    txConfig.PayLoadSize = CAN_PLSIZE_64;
    txConfig.TxPriority = 1;

    DRV_CANFDSPI_TransmitChannelConfigure(DRV_CANFDSPI_INDEX_0, APP_TX_FIFO, &txConfig);

    // Setup RX FIFO
    DRV_CANFDSPI_ReceiveChannelConfigureObjectReset(&rxConfig);
    rxConfig.FifoSize = 15;
    rxConfig.PayLoadSize = CAN_PLSIZE_64;

    DRV_CANFDSPI_ReceiveChannelConfigure(DRV_CANFDSPI_INDEX_0, APP_RX_FIFO, &rxConfig);

    // Setup RX Filter
    fObj.word = 0;
    fObj.bF.SID = 0xda;
    fObj.bF.EXIDE = 0;
    fObj.bF.EID = 0x00;

    DRV_CANFDSPI_FilterObjectConfigure(DRV_CANFDSPI_INDEX_0, CAN_FILTER0, &fObj.bF);

    // Setup RX Mask
    mObj.word = 0;
    mObj.bF.MSID = 0x0;
    mObj.bF.MIDE = 1; // Only allow standard IDs
    mObj.bF.MEID = 0x0;
    DRV_CANFDSPI_FilterMaskConfigure(DRV_CANFDSPI_INDEX_0, CAN_FILTER0, &mObj.bF);

    // Link FIFO and Filter
    DRV_CANFDSPI_FilterToFifoLink(DRV_CANFDSPI_INDEX_0, CAN_FILTER0, APP_RX_FIFO, true);

    // Setup Bit Time
    DRV_CANFDSPI_BitTimeConfigure(DRV_CANFDSPI_INDEX_0, selectedBitTime, CAN_SSP_MODE_AUTO, CAN_SYSCLK_40M);

    // Setup Transmit and Receive Interrupts
    DRV_CANFDSPI_GpioModeConfigure(DRV_CANFDSPI_INDEX_0, GPIO_MODE_INT, GPIO_MODE_INT);
    DRV_CANFDSPI_TransmitChannelEventEnable(DRV_CANFDSPI_INDEX_0, APP_TX_FIFO, CAN_TX_FIFO_NOT_FULL_EVENT);
    DRV_CANFDSPI_ReceiveChannelEventEnable(DRV_CANFDSPI_INDEX_0, APP_RX_FIFO, CAN_RX_FIFO_NOT_EMPTY_EVENT);
    DRV_CANFDSPI_ModuleEventEnable(DRV_CANFDSPI_INDEX_0, CAN_TX_EVENT | CAN_RX_EVENT);

    // Select Normal Mode
    DRV_CANFDSPI_OperationModeSelect(DRV_CANFDSPI_INDEX_0, CAN_NORMAL_MODE);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup TX FIFO

    CAN_TX_FIFO_CONFIG txConfig;

    DRV_CANFDSPI_TransmitFIFOConfigureObjectReset(&txConfig);

    txConfig.FifoSize = 7;
    txConfig.PayLoadSize = CAN_PLSIZE_64;
    txConfig.TxPriority = 1;

    DRV_CANFDSPI_TransmitFIFOConfigure(DRV_CANFDSPI_INDEX_0, APP_TX_FIFO, &txConfig);

///////////////////////////////////////////////////////////////////////////////////////////
    CAN_TX_MSGOBJ txObj;

    txObj.word[0] = 0;
    txObj.word[1] = 0;

    txObj.bF.id.SID = TX_RESPONSE_ID;
    txObj.bF.id.EID = 0;

    txObj.bF.ctrl.BRS = 1;
    txObj.bF.ctrl.DLC = CAN_DLC_64;
    txObj.bF.ctrl.FDF = 1;
    txObj.bF.ctrl.IDE = 0;

    // Configure message data
    int i;
    for (i = 0; i < MAX_DATA_BYTES; i++) txd[i] = txObj.bF.id.SID + i;
//////////////////////////////////////////////////////////////////////////////////////////////

