/*
 * init_conf.c
 *
 *  Created on: Mar 21, 2021
 *      Author: mostafa
 */

#include "drv_canfdspi_api.h"
#include "drv_canfdspi_register.h"
#include "drv_canfdspi_defines.h"


// Section: Configuration and initialization

/*
 *  Each FIFO can be configured either as a Transmit or Receive FIFO. The FIFO control
 *  keeps track of the FIFO head and tail, and calculates the user address. For a TX FIFO,
 *  the user address points to the address in RAM where the data for the next transmit message
 *  shall be stored.
 *  For an RX FIFO, the user address points to the address in RAM where the
 *  data of the next receive message shall be read.
 *  The user notifies the FIFO that a message was written to or read from RAM by incrementing the head/tail of the FIFO.
 *
 *
 *  The CAN FD Controller module has to be initialized before activation.
 *  This is only possible if the module is in Configuration mode
 *  Once the device is configured, Normal Operation mode can be requested by setting REQOP = 000
 *
 *     TEF
 *     TX Queue
 *     FIFO 1
 *     FIFO 2
 *     FIFO 2: Message Object 0
 *     FIFO 2: Message Object 1
 *     FIFO 2: Message Object n
 *     ------
 *     ------

 *     ------
 *     ------
 *     FIFO 31
 *
 *   CiCON – CAN CONTROL REGISTER
 *   STEF: Store in Transmit Event FIFO bit( 1)
 *   RTXAT: Restrict Retransmission Attempts bit( 1)
 *   BRSDIS: Bit Rate Switching Disable bit
 *   BUSY: CAN Module is Busy bit.
 *   OPMOD<2:0>: Operation Mode Status bits
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
/*
 * Operating mode configuration
 */
void CANFD_OperationModeSelect(CAN_OPERATION_MODE opMode)
{
    // Read
    SPI_Read_Byte(cREGADDR_CiCON + 3, &d);

    // Modify
    d &= ~0x07;
    d |= opMode;

    // Write
    SPI_Write_Byte(cREGADDR_CiCON + 3, d);
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
// filter configuration
/*
 * filter is index 1>32
 * id is the identifier
 */
void CANFD_Filter_Enable(CAN_FILTER filter)
{
    uint16_t a;
    REG_CiFLTCON_BYTE fCtrl;

    // Read
    a = cREGADDR_CiFLTCON + filter;
    SPI_Read_Byte(index, a, &fCtrl.byte);

    // Modify ,set the en bit
    fCtrl.bF.Enable = 1;

    // Write
    SPI_Write_Byte(a, fCtrl.byte);
}
void CANFD_Filter_Disable(CAN_FILTER filter)
{
    uint16_t a;
    REG_CiFLTCON_BYTE fCtrl;

    // Read
    a = cREGADDR_CiFLTCON + filter;
    SPI_Read_Byte(index, a, &fCtrl.byte);

    // Modify ,set the en bit
    fCtrl.bF.Enable = 0;

    // Write
    SPI_Write_Byte(a, fCtrl.byte);

}
void CANFD_Filter_Mask_Objects_Configure(CAN_FILTER filter, CAN_FILTEROBJ_ID* id, CAN_MASKOBJ_ID* mask , CAN_FIFO_INDEX fifo_index)
{
    uint16_t a =0;
    REG_CiFLTOBJ fObj;
    REG_CiMASK mObj;
    REG_CiFLTCON_BYTE fCtrl;

    // Setup filter
    fObj.word = 0;
    fObj.bF = *id;
    a = cREGADDR_CiFLTOBJ + (filter * CiFILTER_OFFSET);

    SPI_Write_Word(a, fObj.word);

    // Setup mask
    mObj.word = 0;
    mObj.bF = *mask;
    a =0;
    a = cREGADDR_CiMASK + (filter * CiFILTER_OFFSET);
    SPI_Write_Word(a, mObj.bF);

    // Link the fifo
    fCtrl.bF.BufferPointer = fifo_index;
    a=0;
    a = cREGADDR_CiFLTCON + filter;
    SPI_Write_Word(a, fCtrl.byte);

}
// Transmit FIOF Configuration
/*
 * the tx or rx checking is made on the app. layer
 *
 */
void CANFD_TransmitFIOF_Configure( CAN_FIFO_INDEX fifo_index, CAN_TX_FIFO_CONFIG* config)
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

    // this container is union , so all the writes modified also the word member
    SPI_Write_Word( a, ciFifoCon.word);

}
void CANFD_TransmitFIOF_ConfigureObject_Reset(CAN_TX_FIFO_CONFIG* config)
{
    REG_CiFIFOCON ciFifoCon;
    ciFifoCon.word = canFifoResetValues[0];
    config->RTREnable = ciFifoCon.txBF.RTREnable;
    config->TxPriority = ciFifoCon.txBF.TxPriority;
    config->TxAttempts = ciFifoCon.txBF.TxAttempts;
    config->FifoSize = ciFifoCon.txBF.FifoSize;
    config->PayLoadSize = ciFifoCon.txBF.PayLoadSize;
}
// RECIVE FIOF Configuration
/*
 * the tx or rx checking is made on the app. layer
 *
 */
int8_t CANFD_ReceiveFIOF_Configure(CAN_FIFO_INDEX buffer_index,CAN_RX_FIFO_CONFIG* config)
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

    SPI_Write_Word(a, ciFifoCon.word);
}
void CANFD_ReceiveChannelConfigureObjectReset(CAN_RX_FIFO_CONFIG* config)
{
    REG_CiFIFOCON ciFifoCon;
    ciFifoCon.word = canFifoResetValues[0];
    config->FifoSize = ciFifoCon.rxBF.FifoSize;
    config->PayLoadSize = ciFifoCon.rxBF.PayLoadSize;
    config->RxTimeStampEnable = ciFifoCon.rxBF.RxTimeStampEnable;
}
