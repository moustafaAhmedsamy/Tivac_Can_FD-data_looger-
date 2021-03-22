/*
 * final_code.c
 *
 *  Created on: Mar 17, 2021
 *      Author: mostafa
 */


// Section: Included Files

#include "drv_canfdspi_api.h"
#include "drv_canfdspi_register.h"
#include "drv_canfdspi_defines.h"
#include "../spi/drv_spi.h"
#include "mcp25xxfd_demo_h2_rel/firmware/src/system_config.h"



// *****************************************************************************
// *****************************************************************************
// Section: Configuration


//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

// *****************************************************************************
// *****************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: CAN Transmit





int8_t DRV_CANFDSPI_Transmit_Object_Load( CAN_FIFO_INDEX fifo_index , CAN_TX_MSGOBJ* txObj,        uint8_t *txd, uint32_t txdNumBytes, bool flush)
{
    uint16_t a;                                            // address of the first register
    uint32_t fifoReg[3];                                   //buffer for reading the 3 registers
    uint32_t dataBytesInObject;

    REG_CiFIFOCON ciFifoCon;                               // register 1 control
    __attribute__((unused)) REG_CiFIFOSTA ciFifoSta;       // register 2 status
    REG_CiFIFOUA ciFifoUa;                                 // register 3 address

    int8_t spiTransferError = 0;

    // Get FIFO registers
    a = cREGADDR_CiFIFOCON + (fifo_index * CiFIFO_OFFSET);    // get address

    spiTransferError = DRV_CANFDSPI_ReadWordArray(index, a, fifoReg, 3); // read the 3 registers
    if (spiTransferError) {
        return -1;
    }

    // Check that it is a transmit buffer
    ciFifoCon.word = fifoReg[0];                        // assign the fist word in the first register
    if (!ciFifoCon.txBF.TxEnable) {                     // Check that it is a transmit buffer
        return -2;
    }

    // Check that DLC is big enough for data
    dataBytesInObject = DRV_CANFDSPI_DlcToDataBytes((CAN_DLC) txObj->bF.ctrl.DLC);  // from the lookup table get the no. of bytes to be send
    if (dataBytesInObject < txdNumBytes) {
        return -3;
    }

    // Get status
    ciFifoSta.word = fifoReg[1];                        // assign the 2nd word in the 2nd register

    // Get address
    ciFifoUa.word = fifoReg[2];                        // assign the 3rd word in the 3rd register
#ifdef USERADDRESS_TIMES_FOUR
    a = 4 * ciFifoUa.bF.UserAddress;
#else
    a = ciFifoUa.bF.UserAddress;                      // next address to write the message object , offset from 0 to last message object
#endif
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
    for (i = 0; i < txdNumBytes; i++) {
        txBuffer[i + 8] = txd[i];
    }

    // Make sure we write a multiple of 4 bytes to RAM
    uint16_t n = 0;
    uint8_t j = 0;

    if (txdNumBytes % 4) {                          // soon
        // Need to add bytes
        n = 4 - (txdNumBytes % 4);
        i = txdNumBytes + 8;

        for (j = 0; j < n; j++) {
            txBuffer[i + 8 + j] = 0;
        }
    }

    spiTransferError = DRV_CANFDSPI_WriteByteArray( a, txBuffer, txdNumBytes + 8 + n);
    if (spiTransferError) {
        return -4;
    }

    // Set UINC and TXREQ
    //              ^^^^^
    spiTransferError = DRV_CANFDSPI_TransmitChannelUpdate( fifo_index, flush);
    if (spiTransferError) {
        return -5;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TransmitChannelFlush(CANFDSPI_MODULE_ID index,
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

int8_t DRV_CANFDSPI_TransmitChannelStatusGet(CANFDSPI_MODULE_ID index,
        CAN_FIFO_CHANNEL channel, CAN_TX_FIFO_STATUS* status)
{
    uint16_t a = 0;
    uint32_t sta = 0;
    uint32_t fifoReg[2];
    REG_CiFIFOSTA ciFifoSta;
    REG_CiFIFOCON ciFifoCon;
    int8_t spiTransferError = 0;

    // Get FIFO registers
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET);

    spiTransferError = DRV_CANFDSPI_ReadWordArray(index, a, fifoReg, 2);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    ciFifoCon.word = fifoReg[0];
    ciFifoSta.word = fifoReg[1];

    // Update status
    sta = ciFifoSta.byte[0];

    if (ciFifoCon.txBF.TxRequest) {
        sta |= CAN_TX_FIFO_TRANSMITTING;
    }

    *status = (CAN_TX_FIFO_STATUS) (sta & CAN_TX_FIFO_STATUS_MASK);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TransmitChannelReset(CANFDSPI_MODULE_ID index,
        CAN_FIFO_CHANNEL channel)
{
    return DRV_CANFDSPI_ReceiveChannelReset(index, channel);
}

int8_t DRV_CANFDSPI_TransmitFifoUpdate(CAN_FIFO_INDEX fifo_index, bool flush)
{
    uint16_t a;
    REG_CiFIFOCON ciFifoCon;
    int8_t spiTransferError = 0;

    // Set UINC
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET) + 1; // Byte that contains FRESET
    ciFifoCon.word = 0;
    ciFifoCon.txBF.UINC = 1;

    // Set TXREQ
    if (flush) {
        ciFifoCon.txBF.TxRequest = 1;
    }

    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, ciFifoCon.byte[1]);
    if (spiTransferError) {
        return -1;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TransmitRequestSet(CANFDSPI_MODULE_ID index,
        CAN_TXREQ_CHANNEL txreq)
{
    int8_t spiTransferError = 0;

    // Write TXREQ register
    uint32_t w = txreq;

    spiTransferError = DRV_CANFDSPI_WriteWord(index, cREGADDR_CiTXREQ, w);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TransmitRequestGet(CANFDSPI_MODULE_ID index,
        uint32_t* txreq)
{
    int8_t spiTransferError = 0;

    spiTransferError = DRV_CANFDSPI_ReadWord(index, cREGADDR_CiTXREQ, txreq);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TransmitChannelAbort(CANFDSPI_MODULE_ID index,
        CAN_FIFO_CHANNEL channel)
{
    uint16_t a;
    uint8_t d;
    int8_t spiTransferError = 0;

    // Address
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET);
    a += 1; // byte address of TXREQ

    // Clear TXREQ
    d = 0x00;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, d);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TransmitAbortAll(CANFDSPI_MODULE_ID index)
{
    uint8_t d;
    int8_t spiTransferError = 0;

    // Read CiCON byte 3
    spiTransferError = DRV_CANFDSPI_ReadByte(index, (cREGADDR_CiCON + 3), &d);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    d |= 0x8;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, (cREGADDR_CiCON + 3), d);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}
//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************



//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************




//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

// Section: Transmit Event FIFO




//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************


int8_t DRV_CANFDSPI_ReceiveChannelStatusGet(CANFDSPI_MODULE_ID index,
        CAN_FIFO_CHANNEL channel, CAN_RX_FIFO_STATUS* status)
{
    uint16_t a;
    REG_CiFIFOSTA ciFifoSta;
    int8_t spiTransferError = 0;

    // Read
    ciFifoSta.word = 0;
    a = cREGADDR_CiFIFOSTA + (channel * CiFIFO_OFFSET);

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &ciFifoSta.byte[0]);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    *status = (CAN_RX_FIFO_STATUS) (ciFifoSta.byte[0] & 0x0F);

    return spiTransferError;
}






//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************




int8_t DRV_CANFDSPI_TefEventOverflowClear(CANFDSPI_MODULE_ID index)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Interrupt Flags
    REG_CiTEFSTA ciTefSta;
    ciTefSta.word = 0;
    a = cREGADDR_CiTEFSTA;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &ciTefSta.byte[0]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    ciTefSta.byte[0] &= ~(CAN_TEF_FIFO_OVERFLOW_EVENT);

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, ciTefSta.byte[0]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}


// *****************************************************************************
// *****************************************************************************
// Section: Error Handling

int8_t DRV_CANFDSPI_ErrorCountTransmitGet(CANFDSPI_MODULE_ID index,
        uint8_t* tec)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Error count
    a = cREGADDR_CiTREC + 1;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, tec);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_ErrorCountReceiveGet(CANFDSPI_MODULE_ID index,
        uint8_t* rec)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Error count
    a = cREGADDR_CiTREC;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, rec);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_ErrorStateGet(CANFDSPI_MODULE_ID index,
        CAN_ERROR_STATE* flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Error state
    a = cREGADDR_CiTREC + 2;
    uint8_t f = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &f);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    *flags = (CAN_ERROR_STATE) (f & CAN_ERROR_ALL);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_ErrorCountStateGet(CANFDSPI_MODULE_ID index,
        uint8_t* tec, uint8_t* rec, CAN_ERROR_STATE* flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Error
    a = cREGADDR_CiTREC;
    REG_CiTREC ciTrec;
    ciTrec.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadWord(index, a, &ciTrec.word);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    *tec = ciTrec.byte[1];
    *rec = ciTrec.byte[0];
    *flags = (CAN_ERROR_STATE) (ciTrec.byte[2] & CAN_ERROR_ALL);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_BusDiagnosticsGet(CANFDSPI_MODULE_ID index,
        CAN_BUS_DIAGNOSTIC* bd)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read diagnostic registers all in one shot
    a = cREGADDR_CiBDIAG0;
    uint32_t w[2];

    spiTransferError = DRV_CANFDSPI_ReadWordArray(index, a, w, 2);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    CAN_BUS_DIAGNOSTIC b;
    b.word[0] = w[0];
    b.word[1] = w[1] & 0x0000ffff;
    b.word[2] = (w[1] >> 16) & 0x0000ffff;
    *bd = b;

    return spiTransferError;
}

int8_t DRV_CANFDSPI_BusDiagnosticsClear(CANFDSPI_MODULE_ID index)
{
    int8_t spiTransferError = 0;
    uint8_t a = 0;

    // Clear diagnostic registers all in one shot
    a = cREGADDR_CiBDIAG0;
    uint32_t w[2];
    w[0] = 0;
    w[1] = 0;

    spiTransferError = DRV_CANFDSPI_WriteWordArray(index, a, w, 2);

    return spiTransferError;
}


//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************
// *****************************************************************************
// *****************************************************************************
// Section: Time Stamp

int8_t DRV_CANFDSPI_TimeStampEnable(CANFDSPI_MODULE_ID index)
{
    int8_t spiTransferError = 0;
    uint8_t d = 0;

    // Read
    spiTransferError = DRV_CANFDSPI_ReadByte(index, cREGADDR_CiTSCON + 2, &d);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    d |= 0x01;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, cREGADDR_CiTSCON + 2, d);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TimeStampDisable(CANFDSPI_MODULE_ID index)
{
    int8_t spiTransferError = 0;
    uint8_t d = 0;

    // Read
    spiTransferError = DRV_CANFDSPI_ReadByte(index, cREGADDR_CiTSCON + 2, &d);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    d &= 0x06;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, cREGADDR_CiTSCON + 2, d);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TimeStampGet(CANFDSPI_MODULE_ID index, uint32_t* ts)
{
    int8_t spiTransferError = 0;

    // Read
    spiTransferError = DRV_CANFDSPI_ReadWord(index, cREGADDR_CiTBC, ts);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TimeStampSet(CANFDSPI_MODULE_ID index, uint32_t ts)
{
    int8_t spiTransferError = 0;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteWord(index, cREGADDR_CiTBC, ts);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TimeStampModeConfigure(CANFDSPI_MODULE_ID index,
        CAN_TS_MODE mode)
{
    int8_t spiTransferError = 0;
    uint8_t d = 0;

    // Read
    spiTransferError = DRV_CANFDSPI_ReadByte(index, cREGADDR_CiTSCON + 2, &d);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    d &= 0x01;
    d |= mode << 1;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, cREGADDR_CiTSCON + 2, d);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TimeStampPrescalerSet(CANFDSPI_MODULE_ID index,
        uint16_t ps)
{
    int8_t spiTransferError = 0;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteHalfWord(index, cREGADDR_CiTSCON, ps);

    return spiTransferError;
}

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//************************************************************************************************************************************************

//
// *****************************************************************************
// *****************************************************************************
// Section: Miscellaneous

uint32_t DRV_CANFDSPI_DlcToDataBytes(CAN_DLC dlc)
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

int8_t DRV_CANFDSPI_FifoIndexGet(CANFDSPI_MODULE_ID index,
        CAN_FIFO_CHANNEL channel, uint8_t* mi)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Status register
    uint8_t b = 0;
    a = cREGADDR_CiFIFOSTA + (channel * CiFIFO_OFFSET);
    a += 1; // byte[1]

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &b);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    *mi = b & 0x1f;

    return spiTransferError;
}


