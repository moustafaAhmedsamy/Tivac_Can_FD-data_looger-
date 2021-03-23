/*
 * Transmation.c
 *
 *  Created on: Mar 22, 2021
 *      Author: mostafa
 */


#include <driver/mcp25xxfd_driver/CanFD/drv_canfdspi_api.h>
#include <driver/mcp25xxfd_driver/CanFD/drv_canfdspi_defines.h>
#include <driver/mcp25xxfd_driver/CanFD/drv_canfdspi_register.h>

#include "../spi/drv_spi.h"
#include "mcp25xxfd_demo_h2_rel/firmware/src/system_config.h"
/*
 * After the Message Object was loaded into RAM, the FIFO needs to be incremented by setting
CiFIFOCONm.UINC. This will cause the CAN FD Controller module to increment the head of the
FIFO and update CiFIFOUAm.
Now the message is ready for transmission and the next message can be loaded at the new
address.
 */
void CANFD_Transmit_Object_Load( CAN_FIFO_INDEX fifo_index , CAN_TX_MSGOBJ* txObj,uint8_t *txd, uint32_t txdNumBytes, bool REQ)
{
    uint16_t a;                                            // address of the first register
    uint32_t fifoReg[3];                                   //buffer for reading the 3 registers
    uint32_t dataBytesInObject;

    REG_CiFIFOCON ciFifoCon;                               // register 1 control
    __attribute__((unused)) REG_CiFIFOSTA ciFifoSta;       // register 2 status
    REG_CiFIFOUA ciFifoUa;                                 // register 3 address

    // Get FIFO registers
    a = cREGADDR_CiFIFOCON + (fifo_index * CiFIFO_OFFSET);    // get address

    SPI_Read_Word_Array(a, fifoReg, 3);                  // read the 3 registers

    // Check that it is a transmit buffer
    ciFifoCon.word = fifoReg[0];                        // assign the fist word in the first register
    if (!ciFifoCon.txBF.TxEnable)                       // Check that it is a transmit buffer
    {
        return -2;
    }

    // Check that DLC is big enough for data
    dataBytesInObject = DRV_CANFDSPI_DlcToDataBytes((CAN_DLC) txObj->bF.ctrl.DLC);  // from the lookup table get the no. of bytes to be send
    if (dataBytesInObject < txdNumBytes)
    {
        return -3;
    }

    // Get status
    ciFifoSta.word = fifoReg[1];                        // assign the 2nd word in the 2nd register

    // Get address
    ciFifoUa.word = fifoReg[2];                        // assign the 3rd word in the 3rd register

    a = ciFifoUa.bF.UserAddress;                      // next address to write the message object , offset from 0 to last message object

    a += cRAMADDR_START;                              // the address for next write = base address of rame(0x400)+ offset

    uint8_t txBuffer[MAX_MSG_SIZE];

    //not using 'for' to reduce no of instructions
    txBuffer[0] = txObj->byte[0];                   // fill the TX_OBJECT_BUFFER , to be written in the ram
    txBuffer[1] = txObj->byte[1];                   // writing the frist 2 words
    txBuffer[2] = txObj->byte[2];
    txBuffer[3] = txObj->byte[3];

    txBuffer[4] = txObj->byte[4];
    txBuffer[5] = txObj->byte[5];
    txBuffer[6] = txObj->byte[6];
    txBuffer[7] = txObj->byte[7];

    uint8_t i;                                      // fill the TX_OBJECT_BUFFER by the data after the first 2 words
    for (i = 0; i < txdNumBytes; i++)
    {
        txBuffer[i + 8] = txd[i];
    }

    // Make sure we write a multiple of 4 bytes to RAM
    uint16_t n = 0;
    uint8_t j = 0;

    if (txdNumBytes % 4)                          // soon
    {
        // Need to add bytes
        n = 4 - (txdNumBytes % 4);
        i = txdNumBytes + 8;

        for (j = 0; j < n; j++)
        {
            txBuffer[i + 8 + j] = 0;
        }
    }

    SPI_Write_Byte_Array( a, txBuffer, txdNumBytes + 8 + n);

    // Set UINC and TXREQ
    //              ^^^^^
    CANFD_Tx_Fifo_Transmit(fifo_index, REQ);

}

void DRV_CANFDSPI_TransmitChannelFlush(CANFDSPI_MODULE_ID index,
        CAN_FIFO_CHANNEL channel)
{
    uint8_t d = 0;
    uint16_t a = 0;
    int8_t spiTransferError = 0;

    // Address of TXREQ
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET);
    a += 1;

    // Set TXREQ
    d = 0x02;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, d);

    return spiTransferError;
}


void CANFD_Tx_Fifo_Transmit(CAN_FIFO_INDEX fifo_index, bool REQ)
{
    uint16_t a;
    REG_CiFIFOCON ciFifoCon;

    // Set UINC
    a = cREGADDR_CiFIFOCON + (fifo_index * CiFIFO_OFFSET) + 1; // Byte that contains FRESET
    ciFifoCon.word = 0;
    ciFifoCon.txBF.UINC = 1;

    // Set TXREQ
    if (REQ)
    {
        ciFifoCon.txBF.TxRequest = 1;
    }
    SPI_Write_Byte(a, ciFifoCon.byte[1]);
}
/*
 *  req bit for every fifo
 *
 */
void CANFD_Transmit_Request_Fifos_Set(CAN_TXREQ_CHANNEL txreq)
{
    // Write TXREQ register
    uint32_t w = txreq;
    SPI_Write_Word(index, cREGADDR_CiTXREQ, w);

}
void CANFD_Transmit_Request_Fifos_Get(uint32_t* txreq)
{
    SPI_Read_Word(cREGADDR_CiTXREQ, txreq);
}

void CANFD_Transmit_Fifo_Abort(CAN_FIFO_INDEX fifo_index)
{
    uint16_t a;
    uint8_t d;

    // Address
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET);
    a += 1; // byte address of TXREQ

    // Clear TXREQ
    d = 0x00;

    // Write
    SPI_Write_Byte(a, d);
}
uint32_t CANFD_Dlc_To_Data_Bytes(CAN_DLC dlc)
{
    uint32_t dataBytesInObject = 0;

    Nop();
    Nop();

    if (dlc < CAN_DLC_12) {
        dataBytesInObject = dlc;
    } else {
        switch (dlc) {
            case CAN_DLC_12:
                dataBytesInObject = 12;
                break;
            case CAN_DLC_16:
                dataBytesInObject = 16;
                break;
            case CAN_DLC_20:
                dataBytesInObject = 20;
                break;
            case CAN_DLC_24:
                dataBytesInObject = 24;
                break;
            case CAN_DLC_32:
                dataBytesInObject = 32;
                break;
            case CAN_DLC_48:
                dataBytesInObject = 48;
                break;
            case CAN_DLC_64:
                dataBytesInObject = 64;
                break;
            default:
                break;
        }
    }

    return dataBytesInObject;
}
