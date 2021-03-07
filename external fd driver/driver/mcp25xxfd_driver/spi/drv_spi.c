/*******************************************************************************
 Simple SPI Transfer function

  File Name:
    drv_spi.c

  Summary:
    Initializes SPI 1. Transfers data over SPI.
    Uses SPI FIFO to speed up transfer.

  Description:
    .

  Remarks:

 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "drv_spi.h"
//#include "peripheral/spi/plib_spi.h"		// SPI PLIB Header
#include "../../../mcp25xxfd_demo_h2_rel/firmware/src/system_config.h"
//#include "system_definitions.h"


int8_t DRV_SPI_ChipSelectAssert(uint8_t spiSlaveDeviceIndex, bool assert)
{

    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
//    int8_t error = 0;
//
//    // Select Chip Select
//    switch (spiSlaveDeviceIndex) {
//        case DRV_CANFDSPI_INDEX_0:
//            if (assert) SYS_PORTS_PinClear(PORTS_ID_0, SPI_CS0_PORT_ID, SPI_CS0_PORT_PIN);
//            else SYS_PORTS_PinSet(PORTS_ID_0, SPI_CS0_PORT_ID, SPI_CS0_PORT_PIN);
//            break;
//        case DRV_CANFDSPI_INDEX_1:
//            if (assert) SYS_PORTS_PinClear(PORTS_ID_0, SPI_CS1_PORT_ID, SPI_CS1_PORT_PIN);
//            else SYS_PORTS_PinSet(PORTS_ID_0, SPI_CS1_PORT_ID, SPI_CS1_PORT_PIN);
//            break;
//        default:
//            error = -1;
//            break;
    //}

    //return error;
}

void DRV_SPI_Initialize()
{
//    /* Disable the SPI module to configure it*/
//    PLIB_SPI_Disable(SPI_ID_1);
//
//    /* Set up Master mode */
//    PLIB_SPI_MasterEnable(SPI_ID_1);
//    PLIB_SPI_PinDisable(SPI_ID_1, SPI_PIN_SLAVE_SELECT);
//
//    /* Set up if the SPI is allowed to run while the rest of the CPU is in idle mode*/
//    PLIB_SPI_StopInIdleDisable(SPI_ID_1);
//
//    /* Set up clock Polarity and output data phase*/
//#ifdef SPI_MODE_00
//    PLIB_SPI_ClockPolaritySelect(SPI_ID_1, SPI_CLOCK_POLARITY_IDLE_LOW);
//    PLIB_SPI_OutputDataPhaseSelect(SPI_ID_1, SPI_OUTPUT_DATA_PHASE_ON_ACTIVE_TO_IDLE_CLOCK);
//#else
//    PLIB_SPI_ClockPolaritySelect(SPI_ID_1, SPI_CLOCK_POLARITY_IDLE_HIGH);
//    PLIB_SPI_OutputDataPhaseSelect(SPI_ID_1, SPI_OUTPUT_DATA_PHASE_ON_IDLE_TO_ACTIVE_CLOCK);
//#endif
//
//    /* Set up the Input Sample Phase*/
//    PLIB_SPI_InputSamplePhaseSelect(SPI_ID_1, SPI_INPUT_SAMPLING_PHASE_IN_MIDDLE);
//
//    /* Communication Width Selection */
//    PLIB_SPI_CommunicationWidthSelect(SPI_ID_1, SPI_COMMUNICATION_WIDTH_8BITS);
//
//    /* Baud rate selection */
//    PLIB_SPI_BaudRateSet(SPI_ID_1, SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_1), SPI_BAUDRATE);
//
//    /* Protocol selection */
//    PLIB_SPI_FramedCommunicationDisable(SPI_ID_1);
//    if (PLIB_SPI_ExistsAudioProtocolControl(SPI_ID_1)) {
//        PLIB_SPI_AudioProtocolDisable(SPI_ID_1);
//    }
//
//    /* Buffer type selection */
//    if (PLIB_SPI_ExistsFIFOControl(SPI_ID_1)) {
//        PLIB_SPI_FIFOEnable(SPI_ID_1);
//        PLIB_SPI_FIFOInterruptModeSelect(SPI_ID_1, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_COMPLETELY_EMPTY);
//        PLIB_SPI_FIFOInterruptModeSelect(SPI_ID_1, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
//    }
//
//    PLIB_SPI_BufferClear(SPI_ID_1);
//    PLIB_SPI_ReceiverOverflowClear(SPI_ID_1);
//
//    /* Enable the Module */
//    PLIB_SPI_Enable(SPI_ID_1);
//
//    return;
}

void enc_rbm(uint8_t *buf, uint16_t count) {
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0); //
    spi_send(0x20 | 0x1A);
    int i;
    for (i = 0; i < count; i++) {
        *buf = spi_send(0xFF);
        buf++;
    }
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}
/**
 * Write Buffer Memory.
 */
void enc_wbm(const uint8_t *buf, uint16_t count) {
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(0x60 | 0x1A);
    int i;
    for (i = 0; i < count; i++) {
        spi_send(*buf);
        buf++;
    }
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}

/**
 * Write Control Register (WCR)
 */
void enc_wcr(uint8_t reg, uint8_t val) {
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(0x40 | reg);
    spi_send(val);
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}
/**
 * Read Control Register for MAC an MII registers.
 * Reading MAC and MII registers produces an initial dummy
 * byte. Presumably because it takes longer to fetch the values
 * of those registers.
 */
uint8_t enc_rcr_m(uint8_t reg) {
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(reg);
    spi_send(0xFF);
    uint8_t b = spi_send(0xFF); // Dummy
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
    return b;
}


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
//    do {
//        continueLoop = false;
//        unitsTxed = 0;
//
//        // Fill transmit FIFO
//        if (PLIB_SPI_TransmitBufferIsEmpty(SPI_ID_1)) {
//            while (!PLIB_SPI_TransmitBufferIsFull(SPI_ID_1) && (txcounter < spiTransferSize) && unitsTxed != maxUnits) {
//                PLIB_SPI_BufferWrite(SPI_ID_1, SpiTxData[txcounter]);
//                txcounter++;
//                continueLoop = true;
//                unitsTxed++;
//            }
//        }
//
//        // Read as many bytes as were queued for transmission
//        while (txcounter != rxcounter) {
//            while (PLIB_SPI_ReceiverFIFOIsEmpty(SPI_ID_1));
//            SpiRxData[rxcounter] = PLIB_SPI_BufferRead(SPI_ID_1);
//            rxcounter++;
//            continueLoop = true;
//        }
//
//        // Make sure data gets transmitted even if buffer wasn't empty when we started out with
//        if ((txcounter > rxcounter) || (txcounter < spiTransferSize)) {
//            continueLoop = true;
//        }
//
//    } while (continueLoop);
//
