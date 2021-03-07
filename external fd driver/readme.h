/*
 * readme.h
 *
 *  Created on: Mar 7, 2021
 *      Author: mostafa
 */

#ifndef README_H_
#define README_H_


APP_CANFDSPI_Init()
>>DRV_CANFDSPI_Reset
>>DRV_CANFDSPI_EccEnable
>>DRV_CANFDSPI_RamInit
>>DRV_CANFDSPI_ConfigureObjectReset
>>DRV_CANFDSPI_Configure
>>DRV_CANFDSPI_TransmitChannelConfigureObjectReset
>>
    txConfig.FifoSize = 7;
    txConfig.PayLoadSize = CAN_PLSIZE_64;
    txConfig.TxPriority = 1;
>>
DRV_CANFDSPI_TransmitChannelConfigure
DRV_CANFDSPI_ReceiveChannelConfigureObjectReset(&rxConfig);
>>
    rxConfig.FifoSize = 15;
    rxConfig.PayLoadSize = CAN_PLSIZE_64;
>>
DRV_CANFDSPI_ReceiveChannelConfigure(DRV_CANFDSPI_INDEX_0, APP_RX_FIFO, &rxConfig);

fObj.word = 0;
    fObj.bF.SID = 0xda;
    fObj.bF.EXIDE = 0;
    fObj.bF.EID = 0x00;

    DRV_CANFDSPI_FilterObjectConfigure(DRV_CANFDSPI_INDEX_0, CAN_FILTER0, &fObj.bF);

    mObj.word = 0;
        mObj.bF.MSID = 0x0;
        mObj.bF.MIDE = 1; // Only allow standard IDs
        mObj.bF.MEID = 0x0;
        DRV_CANFDSPI_FilterMaskConfigure(DRV_CANFDSPI_INDEX_0, CAN_FILTER0, &mObj.bF);
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


#endif /* README_H_ */
