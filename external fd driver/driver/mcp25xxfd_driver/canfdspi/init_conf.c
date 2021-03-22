/*
 * init_conf.c
 *
 *  Created on: Mar 21, 2021
 *      Author: mostafa
 */

#include "drv_canfdspi_api.h"
#include "drv_canfdspi_register.h"
#include "drv_canfdspi_defines.h"


// Section: Configuration


/*
 * Each FIFO can be configured either as a Transmit or Receive FIFO. The FIFO control
keeps track of the FIFO head and tail, and calculates the user address. For a TX FIFO, the
user address points to the address in RAM where the data for the next transmit message
shall be stored. For an RX FIFO, the user address points to the address in RAM where the
data of the next receive message shall be read. The user notifies the FIFO that a message
was written to or read from RAM by incrementing the head/tail of the FIFO.
*
*
*The CAN FD Controller module has to be initialized before activation. This is only possible if the
module is in Configuration mode


Once the device is configured, Normal Operation mode can be requested by setting
REQOP = 000

CAN FD Bit Time Configuration
Nominal Bit Rate (NBR)
Data Bit Rate (DBR)

TEF
TX Queue
FIFO 1
FIFO 3
FIFO 2: Message Object 0
FIFO 2: Message Object 1
FIFO 2: Message Object n
FIFO 31



CiCON – CAN CONTROL REGISTER

STEF: Store in Transmit Event FIFO bit( 1)
RTXAT: Restrict Retransmission Attempts bit( 1)
BRSDIS: Bit Rate Switching Disable bit
BUSY: CAN Module is Busy bit.
OPMOD<2:0>: Operation Mode Status bits

 */
void  CANFD_Configure(CAN_CONFIG* config)
{
    REG_CiCON ciCon;

    ciCon.word = canControlResetValues[cREGADDR_CiCON / 4]; // here you will set the configuration bit

    ciCon.bF.DNetFilterCount = config->DNetFilterCount;  // all the fields that has this comment will not be used
    ciCon.bF.IsoCrcEnable = config->IsoCrcEnable;
    ciCon.bF.ProtocolExceptionEventDisable = config->ProtocolExpectionEventDisable;
    ciCon.bF.WakeUpFilterEnable = config->WakeUpFilterEnable; //
    ciCon.bF.WakeUpFilterTime = config->WakeUpFilterTime;  //
    ciCon.bF.BitRateSwitchDisable = config->BitRateSwitchDisable;
    ciCon.bF.RestrictReTxAttempts = config->RestrictReTxAttempts;
    ciCon.bF.EsiInGatewayMode = config->EsiInGatewayMode;  //
    ciCon.bF.SystemErrorToListenOnly = config->SystemErrorToListenOnly;
    ciCon.bF.StoreInTEF = config->StoreInTEF;
    ciCon.bF.TXQEnable = config->TXQEnable;  //
    ciCon.bF.TxBandWidthSharing = config->TxBandWidthSharing; //

    SPI_Write_Word(cREGADDR_CiCON, ciCon.word);

}


void CANFD_RamInit(void)
{
    uint8_t txd[SPI_DEFAULT_BUFFER_LENGTH];
    uint32_t k;

    // Prepare data
    for (k = 0; k < SPI_DEFAULT_BUFFER_LENGTH; k++) {
        txd[k] = 0;
    }

    uint16_t a = cRAMADDR_START;

    for (k = 0; k < (cRAM_SIZE / SPI_DEFAULT_BUFFER_LENGTH); k++)
    {
        SPI_Write_Word_Array( a, txd, SPI_DEFAULT_BUFFER_LENGTH);
        a += SPI_DEFAULT_BUFFER_LENGTH;
    }

}

//CiFIFOCONm1-32 ;

void CANFD_ConfigureObjectReset(CAN_CONFIG* config)
{
    REG_CiCON ciCon;
    ciCon.word = canControlResetValues[cREGADDR_CiCON / 4];

    config->DNetFilterCount = ciCon.bF.DNetFilterCount;
    config->IsoCrcEnable = ciCon.bF.IsoCrcEnable;
    config->ProtocolExpectionEventDisable = ciCon.bF.ProtocolExceptionEventDisable;
    config->WakeUpFilterEnable = ciCon.bF.WakeUpFilterEnable;
    config->WakeUpFilterTime = ciCon.bF.WakeUpFilterTime;
    config->BitRateSwitchDisable = ciCon.bF.BitRateSwitchDisable;
    config->RestrictReTxAttempts = ciCon.bF.RestrictReTxAttempts;
    config->EsiInGatewayMode = ciCon.bF.EsiInGatewayMode;
    config->SystemErrorToListenOnly = ciCon.bF.SystemErrorToListenOnly;
    config->StoreInTEF = ciCon.bF.StoreInTEF;
    config->TXQEnable = ciCon.bF.TXQEnable;
    config->TxBandWidthSharing = ciCon.bF.TxBandWidthSharing;

}

// Section: Operating mode

int8_t CANFD_OperationModeSelect(CAN_OPERATION_MODE opMode)
{
    // Read
    SPI_Read_Byte(cREGADDR_CiCON + 3, &d);

    // Modify
    d &= ~0x07;
    d |= opMode;

    // Write
    DRV_CANFDSPI_WriteByte(index, cREGADDR_CiCON + 3, d);
}

CAN_OPERATION_MODE CANFD_OperationModeGet(void)
{
    uint8_t d = 0;
    CAN_OPERATION_MODE mode = CAN_INVALID_MODE;

    // Read Opmode
    //DRV_CANFDSPI_ReadByte(index, cREGADDR_CiCON + 2, &d); // should be 3 not 2
    //SPI_Read_Byte(cREGADDR_CiCON + 3, &d);
    SPI_Read_Byte(cREGADDR_CiCON + 2, &d)

    // Get Opmode bits
    d = (d >> 5) & 0x7;

    // Decode Opmode
    switch (d) {
        case CAN_NORMAL_MODE:
            mode = CAN_NORMAL_MODE;
            break;
        case CAN_SLEEP_MODE:
            mode = CAN_SLEEP_MODE;
            break;
        case CAN_INTERNAL_LOOPBACK_MODE:
            mode = CAN_INTERNAL_LOOPBACK_MODE;
            break;
        case CAN_EXTERNAL_LOOPBACK_MODE:
            mode = CAN_EXTERNAL_LOOPBACK_MODE;
            break;
        case CAN_LISTEN_ONLY_MODE:
            mode = CAN_LISTEN_ONLY_MODE;
            break;
        case CAN_CONFIGURATION_MODE:
            mode = CAN_CONFIGURATION_MODE;
            break;
        case CAN_CLASSIC_MODE:
            mode = CAN_CLASSIC_MODE;
            break;
        case CAN_RESTRICTED_MODE:
            mode = CAN_RESTRICTED_MODE;
            break;
        default:
            mode = CAN_INVALID_MODE;
            break;
    }

    return mode;
}



void CANFD_TransmitFIOFConfigure( CAN_FIFO_INDEX fifo_index, CAN_TX_FIFO_CONFIG* config)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Setup FIFO ,this container is union
    REG_CiFIFOCON ciFifoCon;
    ciFifoCon.word = canFifoResetValues[0]; // reseting the values of the union before writing

    ciFifoCon.txBF.TxEnable = 1;
    ciFifoCon.txBF.FifoSize = config->FifoSize;
    ciFifoCon.txBF.PayLoadSize = config->PayLoadSize;
    ciFifoCon.txBF.TxAttempts = config->TxAttempts;
    ciFifoCon.txBF.TxPriority = config->TxPriority; // will not be used
    ciFifoCon.txBF.RTREnable = config->RTREnable;  // will not be used

    a = cREGADDR_CiFIFOCON + (fifo_index * CiFIFO_OFFSET);

    // this container is union , so all the writes modyied also the word member
    DRV_CANFDSPI_WriteWord( a, ciFifoCon.word);

}

void CANFD_TransmitFIOFConfigureObjectReset(CAN_TX_FIFO_CONFIG* config)
{
    REG_CiFIFOCON ciFifoCon;
    ciFifoCon.word = canFifoResetValues[0];

    config->RTREnable = ciFifoCon.txBF.RTREnable;
    config->TxPriority = ciFifoCon.txBF.TxPriority;
    config->TxAttempts = ciFifoCon.txBF.TxAttempts;
    config->FifoSize = ciFifoCon.txBF.FifoSize;
    config->PayLoadSize = ciFifoCon.txBF.PayLoadSize;

}


// filter
int8_t DRV_CANFDSPI_FilterObjectConfigure(CAN_FILTER filter, CAN_FILTEROBJ_ID* id)
{
    uint16_t a;
    REG_CiFLTOBJ fObj;
    int8_t spiTransferError = 0;

    // Setup
    fObj.word = 0;
    fObj.bF = *id;
    a = cREGADDR_CiFLTOBJ + (filter * CiFILTER_OFFSET);

    spiTransferError = DRV_CANFDSPI_WriteWord(index, a, fObj.word);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_FilterMaskConfigure(CAN_FILTER filter, CAN_MASKOBJ_ID* mask)
{
    uint16_t a;
    REG_CiMASK mObj;
    int8_t spiTransferError = 0;

    // Setup
    mObj.word = 0;
    mObj.bF = *mask;
    a = cREGADDR_CiMASK + (filter * CiFILTER_OFFSET);

    spiTransferError = DRV_CANFDSPI_WriteWord(index, a, mObj.word);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_FilterToFifoLink( CAN_FILTER filter, CAN_FIFO_CHANNEL channel, bool enable)
{
    uint16_t a;
    REG_CiFLTCON_BYTE fCtrl;
    int8_t spiTransferError = 0;

    // Enable
    if (enable) {
        fCtrl.bF.Enable = 1;
    } else {
        fCtrl.bF.Enable = 0;
    }

    // Link
    fCtrl.bF.BufferPointer = channel;
    a = cREGADDR_CiFLTCON + filter;

    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, fCtrl.byte);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_FilterEnable(CANFDSPI_MODULE_ID index, CAN_FILTER filter)
{
    uint16_t a;
    REG_CiFLTCON_BYTE fCtrl;
    int8_t spiTransferError = 0;

    // Read
    a = cREGADDR_CiFLTCON + filter;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &fCtrl.byte);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    fCtrl.bF.Enable = 1;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, fCtrl.byte);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_FilterDisable(CANFDSPI_MODULE_ID index, CAN_FILTER filter)
{
    uint16_t a;
    REG_CiFLTCON_BYTE fCtrl;
    int8_t spiTransferError = 0;

    // Read
    a = cREGADDR_CiFLTCON + filter;

    spiTransferError = DRV_CANFDSPI_ReadByte(index, a, &fCtrl.byte);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    fCtrl.bF.Enable = 0;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(index, a, fCtrl.byte);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}


// RECIVE
int8_t DRV_CANFDSPI_ReceiveChannelConfigure(CAN_FIFO_INDEX buffer_index,CAN_RX_FIFO_CONFIG* config)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    if (buffer_index == CAN_TXQUEUE_CH0) {
        return -100;
    }

    // Setup FIFO
    REG_CiFIFOCON ciFifoCon;
    ciFifoCon.word = canFifoResetValues[0];

    ciFifoCon.rxBF.TxEnable = 0;
    ciFifoCon.rxBF.FifoSize = config->FifoSize;
    ciFifoCon.rxBF.PayLoadSize = config->PayLoadSize;
    ciFifoCon.rxBF.RxTimeStampEnable = config->RxTimeStampEnable;

    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET);

    DRV_CANFDSPI_WriteWord(index, a, ciFifoCon.word);
}

int8_t DRV_CANFDSPI_ReceiveChannelConfigureObjectReset(CAN_RX_FIFO_CONFIG* config)
{
    REG_CiFIFOCON ciFifoCon;
    ciFifoCon.word = canFifoResetValues[0];

    config->FifoSize = ciFifoCon.rxBF.FifoSize;
    config->PayLoadSize = ciFifoCon.rxBF.PayLoadSize;
    config->RxTimeStampEnable = ciFifoCon.rxBF.RxTimeStampEnable;

    return 0;
}

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

