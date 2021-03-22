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



int8_t DRV_CANFDSPI_TransmitQueueConfigure(CANFDSPI_MODULE_ID index,
        CAN_TX_QUEUE_CONFIG* config)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Setup FIFO
    REG_CiTXQCON ciFifoCon;
    ciFifoCon.word = canFifoResetValues[0];

    ciFifoCon.txBF.TxEnable = 1;
    ciFifoCon.txBF.FifoSize = config->FifoSize;
    ciFifoCon.txBF.PayLoadSize = config->PayLoadSize;
    ciFifoCon.txBF.TxAttempts = config->TxAttempts;
    ciFifoCon.txBF.TxPriority = config->TxPriority;

    a = cREGADDR_CiTXQCON;
    spiTransferError = DRV_CANFDSPI_WriteWord(index, a, ciFifoCon.word);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TransmitQueueConfigureObjectReset(CAN_TX_QUEUE_CONFIG* config)
{
    REG_CiFIFOCON ciFifoCon;
    ciFifoCon.word = canFifoResetValues[0];

    config->TxPriority = ciFifoCon.txBF.TxPriority;
    config->TxAttempts = ciFifoCon.txBF.TxAttempts;
    config->FifoSize = ciFifoCon.txBF.FifoSize;
    config->PayLoadSize = ciFifoCon.txBF.PayLoadSize;

    return 0;
}

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


int8_t DRV_CANFDSPI_ReceiveMessageGet(CAN_FIFO_INDEX buffer_index, CAN_RX_MSGOBJ* rxObj,uint8_t *rxd, uint8_t nBytes)
{
    uint8_t n = 0;
    uint8_t i = 0;
    uint16_t a;
    uint32_t fifoReg[3];
    REG_CiFIFOCON ciFifoCon;
    __attribute__((unused)) REG_CiFIFOSTA ciFifoSta;
    REG_CiFIFOUA ciFifoUa;
    int8_t spiTransferError = 0;

    // Get FIFO registers
    a = cREGADDR_CiFIFOCON + (buffer_index * CiFIFO_OFFSET);

    DRV_CANFDSPI_ReadWordArray( a, fifoReg, 3);


    // Check that it is a receive buffer
    ciFifoCon.word = fifoReg[0];
    if (ciFifoCon.txBF.TxEnable) {
        return -2;
    }

    // Get Status
    ciFifoSta.word = fifoReg[1];

    // Get address
    ciFifoUa.word = fifoReg[2];
#ifdef USERADDRESS_TIMES_FOUR
    a = 4 * ciFifoUa.bF.UserAddress;
#else
    a = ciFifoUa.bF.UserAddress;
#endif
    a += cRAMADDR_START;

    // Number of bytes to read
    n = nBytes + 8; // Add 8 header bytes  , first 2 words

    if (ciFifoCon.rxBF.RxTimeStampEnable) {
        n += 4; // Add 4 time stamp bytes , if it is configured
    }

    // Make sure we read a multiple of 4 bytes from RAM
    if (n % 4) {
        n = n + 4 - (n % 4);
    }

    // Read rxObj using one access
    uint8_t ba[MAX_MSG_SIZE];

    if (n > MAX_MSG_SIZE) {
        n = MAX_MSG_SIZE;
    }

    DRV_CANFDSPI_ReadByteArray( a, ba, n);


    // Assign message header
    REG_t myReg;

    myReg.byte[0] = ba[0];
    myReg.byte[1] = ba[1];
    myReg.byte[2] = ba[2];
    myReg.byte[3] = ba[3];
    rxObj->word[0] = myReg.word;

    myReg.byte[0] = ba[4];
    myReg.byte[1] = ba[5];
    myReg.byte[2] = ba[6];
    myReg.byte[3] = ba[7];
    rxObj->word[1] = myReg.word;

    if (ciFifoCon.rxBF.RxTimeStampEnable) {
        myReg.byte[0] = ba[8];
        myReg.byte[1] = ba[9];
        myReg.byte[2] = ba[10];
        myReg.byte[3] = ba[11];
        rxObj->word[2] = myReg.word;

        // Assign message data
        for (i = 0; i < nBytes; i++) {
            rxd[i] = ba[i + 12];
        }
    } else {
        rxObj->word[2] = 0;

        // Assign message data
        for (i = 0; i < nBytes; i++) {
            rxd[i] = ba[i + 8];
        }
    }

    // UINC channel
    DRV_CANFDSPI_ReceiveChannelUpdate(buffer_index);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_ReceiveChannelReset(CANFDSPI_MODULE_ID index,
        CAN_FIFO_CHANNEL channel)
{
    uint16_t a = 0;
    REG_CiFIFOCON ciFifoCon;
    int8_t spiTransferError = 0;

    // Address and data
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET) + 1; // Byte that contains FRESET
    ciFifoCon.word = 0;
    ciFifoCon.rxBF.FRESET = 1;

    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, ciFifoCon.byte[1]);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_ReceiveChannelUpdate(CAN_FIFO_INDEX buffer_index)
{
    uint16_t a = 0;
    REG_CiFIFOCON ciFifoCon;
    int8_t spiTransferError = 0;
    ciFifoCon.word = 0;

    // Set UINC
    a = cREGADDR_CiFIFOCON + (buffer_index * CiFIFO_OFFSET) + 1; // Byte that contains FRESET
    ciFifoCon.rxBF.UINC = 1;

    // Write byte
    spiTransferError = DRV_CANFDSPI_WriteByte( a, ciFifoCon.byte[1]);

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

// Section: Transmit Event FIFO

int8_t DRV_CANFDSPI_TefStatusGet(CANFDSPI_MODULE_ID index,
        CAN_TEF_FIFO_STATUS* status)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read
    REG_CiTEFSTA ciTefSta;
    ciTefSta.word = 0;
    a = cREGADDR_CiTEFSTA;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &ciTefSta.byte[0]);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    *status = (CAN_TEF_FIFO_STATUS) (ciTefSta.byte[0] & CAN_TEF_FIFO_STATUS_MASK);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TefMessageGet(CANFDSPI_MODULE_ID index,
        CAN_TEF_MSGOBJ* tefObj)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    uint32_t fifoReg[3];
    uint8_t n = 0;

    // Get FIFO registers
    a = cREGADDR_CiTEFCON;

    spiTransferError = DRV_CANFDSPI_ReadWordArray(index, a, fifoReg, 3);
    if (spiTransferError) {
        return -1;
    }

    // Get control
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = fifoReg[0];

    // Get status
    __attribute__((unused)) REG_CiTEFSTA ciTefSta;
    ciTefSta.word = fifoReg[1];

    // Get address
    REG_CiFIFOUA ciTefUa;
    ciTefUa.word = fifoReg[2];
#ifdef USERADDRESS_TIMES_FOUR
    a = 4 * ciTefUa.bF.UserAddress;
#else
    a = ciTefUa.bF.UserAddress;
#endif
    a += cRAMADDR_START;

    // Number of bytes to read
    n = 8; // 8 header bytes

    if (ciTefCon.bF.TimeStampEnable) {
        n += 4; // Add 4 time stamp bytes
    }

    // Read rxObj using one access
    uint8_t ba[12];

    spiTransferError = DRV_CANFDSPI_ReadByteArray(index, a, ba, n);
    if (spiTransferError) {
        return -2;
    }

    // Assign message header
    REG_t myReg;

    myReg.byte[0] = ba[0];
    myReg.byte[1] = ba[1];
    myReg.byte[2] = ba[2];
    myReg.byte[3] = ba[3];
    tefObj->word[0] = myReg.word;

    myReg.byte[0] = ba[4];
    myReg.byte[1] = ba[5];
    myReg.byte[2] = ba[6];
    myReg.byte[3] = ba[7];
    tefObj->word[1] = myReg.word;

    if (ciTefCon.bF.TimeStampEnable) {
        myReg.byte[0] = ba[8];
        myReg.byte[1] = ba[9];
        myReg.byte[2] = ba[10];
        myReg.byte[3] = ba[11];
        tefObj->word[2] = myReg.word;
    } else {
        tefObj->word[2] = 0;
    }

    // Set UINC
    spiTransferError = DRV_CANFDSPI_TefUpdate(index);
    if (spiTransferError) {
        return -3;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TefReset(CANFDSPI_MODULE_ID index)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Set FRESET
    a = cREGADDR_CiTEFCON + 1;
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = 0;
    ciTefCon.bF.FRESET = 1;

    // Write byte
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, ciTefCon.byte[1]);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TefUpdate(CANFDSPI_MODULE_ID index)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Set UINC
    a = cREGADDR_CiTEFCON + 1;
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = 0;
    ciTefCon.bF.UINC = 1;

    // Write byte
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, ciTefCon.byte[1]);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TefConfigure(CANFDSPI_MODULE_ID index, CAN_TEF_CONFIG* config)
{
    int8_t spiTransferError = 0;

    // Setup FIFO
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = canFifoResetValues[0];

    ciTefCon.bF.FifoSize = config->FifoSize;
    ciTefCon.bF.TimeStampEnable = config->TimeStampEnable;

    spiTransferError = DRV_CANFDSPI_WriteWord(index, cREGADDR_CiTEFCON, ciTefCon.word);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TefConfigureObjectReset(CAN_TEF_CONFIG* config)
{
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = canFifoResetValues[0];

    config->FifoSize = ciTefCon.bF.FifoSize;
    config->TimeStampEnable = ciTefCon.bF.TimeStampEnable;

    return 0;
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

// interputs
int8_t DRV_CANFDSPI_ModuleEventGet(CANFDSPI_MODULE_ID index,
        CAN_MODULE_EVENT* flags)
{
    int8_t spiTransferError = 0;

    // Read Interrupt flags
    REG_CiINTFLAG intFlags;
    intFlags.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadHalfWord(index, cREGADDR_CiINTFLAG, &intFlags.word);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    *flags = (CAN_MODULE_EVENT) (intFlags.word & CAN_ALL_EVENTS);

    return spiTransferError;
}

int8_t CANFDSPI_Module_Event_Enable( CAN_MODULE_EVENT flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Interrupt Enables
    a = cREGADDR_CiINTENABLE;
    REG_CiINTENABLE intEnables;
    intEnables.word = 0;

    DRV_CANFDSPI_ReadHalfWord(index, a, &intEnables.word);

    // Modify
    intEnables.word |= (flags & CAN_ALL_EVENTS);

    // Write
    spiTransferError = DRV_CANFDSPI_WriteHalfWord(index, a, intEnables.word);

}

int8_t DRV_CANFDSPI_ModuleEventDisable(CANFDSPI_MODULE_ID index,
        CAN_MODULE_EVENT flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Interrupt Enables
    a = cREGADDR_CiINTENABLE;
    REG_CiINTENABLE intEnables;
    intEnables.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadHalfWord(index, a, &intEnables.word);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    intEnables.word &= ~(flags & CAN_ALL_EVENTS);

    // Write
    spiTransferError = DRV_CANFDSPI_WriteHalfWord(index, a, intEnables.word);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_ModuleEventClear(CANFDSPI_MODULE_ID index,
        CAN_MODULE_EVENT flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Interrupt flags
    a = cREGADDR_CiINTFLAG;
    REG_CiINTFLAG intFlags;
    intFlags.word = 0;

    // Write 1 to all flags except the ones that we want to clear
    // Writing a 1 will not set the flag
    // Only writing a 0 will clear it
    // The flags are HS/C
    intFlags.word = CAN_ALL_EVENTS;
    intFlags.word &= ~flags;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteHalfWord(index, a, intFlags.word);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_ModuleEventRxCodeGet(CANFDSPI_MODULE_ID index,
        CAN_RXCODE* rxCode)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    uint8_t rxCodeByte = 0;

    // Read
    a = cREGADDR_CiVEC + 3;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &rxCodeByte);
    if (spiTransferError) {
        return -1;
    }

    // Decode data
    // 0x40 = "no interrupt" (CAN_FIFO_CIVEC_NOINTERRUPT)
    if ((rxCodeByte < CAN_RXCODE_TOTAL_CHANNELS) || (rxCodeByte == CAN_RXCODE_NO_INT)) {
        *rxCode = (CAN_RXCODE) rxCodeByte;
    } else {
        *rxCode = CAN_RXCODE_RESERVED; // shouldn't get here
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_ModuleEventTxCodeGet(CANFDSPI_MODULE_ID index,
        CAN_TXCODE* txCode)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    uint8_t txCodeByte = 0;

    // Read
    a = cREGADDR_CiVEC + 2;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &txCodeByte);
    if (spiTransferError) {
        return -1;
    }

    // Decode data
    // 0x40 = "no interrupt" (CAN_FIFO_CIVEC_NOINTERRUPT)
    if ((txCodeByte < CAN_TXCODE_TOTAL_CHANNELS) || (txCodeByte == CAN_TXCODE_NO_INT)) {
        *txCode = (CAN_TXCODE) txCodeByte;
    } else {
        *txCode = CAN_TXCODE_RESERVED; // shouldn't get here
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_ModuleEventFilterHitGet(CANFDSPI_MODULE_ID index,
        CAN_FILTER* filterHit)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    uint8_t filterHitByte = 0;

    // Read
    a = cREGADDR_CiVEC + 1;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &filterHitByte);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    *filterHit = (CAN_FILTER) filterHitByte;

    return spiTransferError;
}

int8_t DRV_CANFDSPI_ModuleEventIcodeGet(CANFDSPI_MODULE_ID index,
        CAN_ICODE* icode)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    uint8_t icodeByte = 0;

    // Read
    a = cREGADDR_CiVEC;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &icodeByte);
    if (spiTransferError) {
        return -1;
    }

    // Decode
    if ((icodeByte < CAN_ICODE_RESERVED) && ((icodeByte < CAN_ICODE_TOTAL_CHANNELS) || (icodeByte >= CAN_ICODE_NO_INT))) {
        *icode = (CAN_ICODE) icodeByte;
    } else {
        *icode = CAN_ICODE_RESERVED; // shouldn't get here
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



// *****************************************************************************
// *****************************************************************************
// Section: Transmit Event FIFO Events

int8_t DRV_CANFDSPI_TefEventGet(CANFDSPI_MODULE_ID index,
        CAN_TEF_FIFO_EVENT* flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Interrupt flags
    REG_CiTEFSTA ciTefSta;
    ciTefSta.word = 0;
    a = cREGADDR_CiTEFSTA;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &ciTefSta.byte[0]);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    *flags = (CAN_TEF_FIFO_EVENT) (ciTefSta.byte[0] & CAN_TEF_FIFO_ALL_EVENTS);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TefEventEnable(CANFDSPI_MODULE_ID index,
        CAN_TEF_FIFO_EVENT flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Interrupt Enables
    a = cREGADDR_CiTEFCON;
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &ciTefCon.byte[0]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    ciTefCon.byte[0] |= (flags & CAN_TEF_FIFO_ALL_EVENTS);

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, ciTefCon.byte[0]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TefEventDisable(CANFDSPI_MODULE_ID index,
        CAN_TEF_FIFO_EVENT flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Interrupt Enables
    a = cREGADDR_CiTEFCON;
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &ciTefCon.byte[0]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    ciTefCon.byte[0] &= ~(flags & CAN_TEF_FIFO_ALL_EVENTS);

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, ciTefCon.byte[0]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

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


