/*
 * CanFD_API.c
 *
 *  Created on: Mar 23, 2021
 *      Author: mostafa
 */

#include "CanFD_API.h"
#include "CanFd_register.h"
#include "SPI_interface.h"

int8_t CANFD_Bit_Time_Configure( CAN_BITTIME_SETUP bitTime, CAN_SSP_MODE sspMode, CAN_SYSCLK_SPEED clk)
{
    int8_t spiTransferError = 0;

    // Decode clk
    switch (clk) {
        case CAN_SYSCLK_40M:
            CANFD_Bit_Time_Configure_Nominal_40MHz( bitTime);
            CANFD_Bit_Time_Configure_Data_40MHz(bitTime);
            break;
        case CAN_SYSCLK_20M:
            CANFD_Bit_Time_Configure_Nominal_20MHz( bitTime);
            CANFD_Bit_Time_Configure_Data_20MHz(bitTime);
            break;
        case CAN_SYSCLK_10M:
            CANFD_Bit_Time_Configure_Nominal_10MHz(bitTime);
            CANFD_Bit_Time_Configure_Data_10MHz( bitTime);
            break;
        default:
            spiTransferError = -1;
            break;
    }

    return spiTransferError;
}

void CANFD_Bit_Time_Configure_Nominal_40MHz(CAN_BITTIME_SETUP bitTime)
{
    REG_CiNBTCFG ciNbtcfg;

    ciNbtcfg.word = canControlResetValues[cREGADDR_CiNBTCFG / 4];

    // Arbitration Bit rate
    switch (bitTime) {
            // All 500K
        case CAN_500K_1M:
        case CAN_500K_2M:
        case CAN_500K_3M:
        case CAN_500K_4M:
        case CAN_500K_5M:
        case CAN_500K_8M:
        case CAN_500K_10M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 62;
            ciNbtcfg.bF.TSEG2 = 15;
            ciNbtcfg.bF.SJW = 15;
            break;

            // All 250K
        case CAN_250K_500K:
        case CAN_250K_833K:
        case CAN_250K_1M:
        case CAN_250K_1M5:
        case CAN_250K_2M:
        case CAN_250K_3M:
        case CAN_250K_4M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 126;
            ciNbtcfg.bF.TSEG2 = 31;
            ciNbtcfg.bF.SJW = 31;
            break;

        case CAN_1000K_4M:
        case CAN_1000K_8M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 30;
            ciNbtcfg.bF.TSEG2 = 7;
            ciNbtcfg.bF.SJW = 7;
            break;

        case CAN_125K_500K:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 254;
            ciNbtcfg.bF.TSEG2 = 63;
            ciNbtcfg.bF.SJW = 63;
            break;

    }

    // Write Bit time registers
    SPI_Write_Word(cREGADDR_CiNBTCFG, ciNbtcfg.word);

}

void CANFD_Bit_Time_Configure_Data_40MHz(CAN_BITTIME_SETUP bitTime)
{
    REG_CiDBTCFG ciDbtcfg;
    REG_CiTDC ciTdc;


    ciDbtcfg.word = canControlResetValues[cREGADDR_CiDBTCFG / 4];
    ciTdc.word = 0;

    // Configure Bit time and sample point
    ciTdc.bF.TDCMode = CAN_SSP_MODE_AUTO;
    uint32_t tdcValue = 0;

    // Data Bit rate and SSP
    switch (bitTime) {
        case CAN_500K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 30;
            ciDbtcfg.bF.TSEG2 = 7;
            ciDbtcfg.bF.SJW = 7;
            // SSP
            ciTdc.bF.TDCOffset = 31;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_2M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 14;
            ciDbtcfg.bF.TSEG2 = 3;
            ciDbtcfg.bF.SJW = 3;
            // SSP
            ciTdc.bF.TDCOffset = 15;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_3M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 8;
            ciDbtcfg.bF.TSEG2 = 2;
            ciDbtcfg.bF.SJW = 2;
            // SSP
            ciTdc.bF.TDCOffset = 9;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_4M:
        case CAN_1000K_4M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_5M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 4;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 5;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_6M7:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 3;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 4;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_8M:
        case CAN_1000K_8M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 2;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 3;
            ciTdc.bF.TDCValue = 1;
            break;
        case CAN_500K_10M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 1;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 2;
            ciTdc.bF.TDCValue = 0;
            break;

        case CAN_250K_500K:
        case CAN_125K_500K:
            ciDbtcfg.bF.BRP = 1;
            ciDbtcfg.bF.TSEG1 = 30;
            ciDbtcfg.bF.TSEG2 = 7;
            ciDbtcfg.bF.SJW = 7;
            // SSP
            ciTdc.bF.TDCOffset = 31;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_833K:
            ciDbtcfg.bF.BRP = 1;
            ciDbtcfg.bF.TSEG1 = 17;
            ciDbtcfg.bF.TSEG2 = 4;
            ciDbtcfg.bF.SJW = 4;
            // SSP
            ciTdc.bF.TDCOffset = 18;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 30;
            ciDbtcfg.bF.TSEG2 = 7;
            ciDbtcfg.bF.SJW = 7;
            // SSP
            ciTdc.bF.TDCOffset = 31;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_1M5:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 18;
            ciDbtcfg.bF.TSEG2 = 5;
            ciDbtcfg.bF.SJW = 5;
            // SSP
            ciTdc.bF.TDCOffset = 19;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_2M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 14;
            ciDbtcfg.bF.TSEG2 = 3;
            ciDbtcfg.bF.SJW = 3;
            // SSP
            ciTdc.bF.TDCOffset = 15;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_3M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 8;
            ciDbtcfg.bF.TSEG2 = 2;
            ciDbtcfg.bF.SJW = 2;
            // SSP
            ciTdc.bF.TDCOffset = 9;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_4M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;


    }

    // Write Bit time registers
    SPI_Write_Word( cREGADDR_CiDBTCFG, ciDbtcfg.word);

    SPI_Write_Word( cREGADDR_CiTDC, ciTdc.word);

}

void CANFD_Bit_Time_Configure_Nominal_20MHz(CAN_BITTIME_SETUP bitTime)
{

    REG_CiNBTCFG ciNbtcfg;

    ciNbtcfg.word = canControlResetValues[cREGADDR_CiNBTCFG / 4];

    // Arbitration Bit rate
    switch (bitTime) {
            // All 500K
        case CAN_500K_1M:
        case CAN_500K_2M:
        case CAN_500K_4M:
        case CAN_500K_5M:
        case CAN_500K_6M7:
        case CAN_500K_8M:
        case CAN_500K_10M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 30;
            ciNbtcfg.bF.TSEG2 = 7;
            ciNbtcfg.bF.SJW = 7;
            break;

            // All 250K
        case CAN_250K_500K:
        case CAN_250K_833K:
        case CAN_250K_1M:
        case CAN_250K_1M5:
        case CAN_250K_2M:
        case CAN_250K_3M:
        case CAN_250K_4M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 62;
            ciNbtcfg.bF.TSEG2 = 15;
            ciNbtcfg.bF.SJW = 15;
            break;

        case CAN_1000K_4M:
        case CAN_1000K_8M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 14;
            ciNbtcfg.bF.TSEG2 = 3;
            ciNbtcfg.bF.SJW = 3;
            break;

        case CAN_125K_500K:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 126;
            ciNbtcfg.bF.TSEG2 = 31;
            ciNbtcfg.bF.SJW = 31;
            break;

    }

    // Write Bit time registers
    SPI_Write_Word( cREGADDR_CiNBTCFG, ciNbtcfg.word);
}

void CANFD_Bit_Time_Configure_Data_20MHz( CAN_BITTIME_SETUP bitTime)
{

    REG_CiDBTCFG ciDbtcfg;
    REG_CiTDC ciTdc;
    //    sspMode;

    ciDbtcfg.word = canControlResetValues[cREGADDR_CiDBTCFG / 4];
    ciTdc.word = 0;

    // Configure Bit time and sample point
    ciTdc.bF.TDCMode = CAN_SSP_MODE_AUTO;
    uint32_t tdcValue = 0;

    // Data Bit rate and SSP
    switch (bitTime) {
        case CAN_500K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 14;
            ciDbtcfg.bF.TSEG2 = 3;
            ciDbtcfg.bF.SJW = 3;
            // SSP
            ciTdc.bF.TDCOffset = 15;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_2M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_4M:
        case CAN_1000K_4M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 2;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 3;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_5M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 1;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 2;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_6M7:
        case CAN_500K_8M:
        case CAN_500K_10M:
        case CAN_1000K_8M:
            //qDebug("Data Bitrate not feasible with this clock!");

        case CAN_250K_500K:
        case CAN_125K_500K:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 30;
            ciDbtcfg.bF.TSEG2 = 7;
            ciDbtcfg.bF.SJW = 7;
            // SSP
            ciTdc.bF.TDCOffset = 31;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_833K:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 17;
            ciDbtcfg.bF.TSEG2 = 4;
            ciDbtcfg.bF.SJW = 4;
            // SSP
            ciTdc.bF.TDCOffset = 18;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 14;
            ciDbtcfg.bF.TSEG2 = 3;
            ciDbtcfg.bF.SJW = 3;
            // SSP
            ciTdc.bF.TDCOffset = 15;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_1M5:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 8;
            ciDbtcfg.bF.TSEG2 = 2;
            ciDbtcfg.bF.SJW = 2;
            // SSP
            ciTdc.bF.TDCOffset = 9;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_2M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_3M:
            //qDebug("Data Bitrate not feasible with this clock!");


        case CAN_250K_4M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 2;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 3;
            ciTdc.bF.TDCValue = tdcValue;
            break;

    }

    // Write Bit time registers
    SPI_Write_Word( cREGADDR_CiDBTCFG, ciDbtcfg.word);
    SPI_Write_Word(cREGADDR_CiTDC, ciTdc.word);

}

void CANFD_Bit_Time_Configure_Nominal_10MHz(CAN_BITTIME_SETUP bitTime)
{

    REG_CiNBTCFG ciNbtcfg;

    ciNbtcfg.word = canControlResetValues[cREGADDR_CiNBTCFG / 4];

    // Arbitration Bit rate
    switch (bitTime) {
            // All 500K
        case CAN_500K_1M:
        case CAN_500K_2M:
        case CAN_500K_4M:
        case CAN_500K_5M:
        case CAN_500K_6M7:
        case CAN_500K_8M:
        case CAN_500K_10M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 14;
            ciNbtcfg.bF.TSEG2 = 3;
            ciNbtcfg.bF.SJW = 3;
            break;

            // All 250K
        case CAN_250K_500K:
        case CAN_250K_833K:
        case CAN_250K_1M:
        case CAN_250K_1M5:
        case CAN_250K_2M:
        case CAN_250K_3M:
        case CAN_250K_4M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 30;
            ciNbtcfg.bF.TSEG2 = 7;
            ciNbtcfg.bF.SJW = 7;
            break;

        case CAN_1000K_4M:
        case CAN_1000K_8M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 7;
            ciNbtcfg.bF.TSEG2 = 2;
            ciNbtcfg.bF.SJW = 2;
            break;

        case CAN_125K_500K:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 62;
            ciNbtcfg.bF.TSEG2 = 15;
            ciNbtcfg.bF.SJW = 15;


    }

    // Write Bit time registers
    SPI_Write_Word( cREGADDR_CiNBTCFG, ciNbtcfg.word);
}

void CANFD_Bit_Time_Configure_Data_10MHz(CAN_BITTIME_SETUP bitTime)
{

    REG_CiDBTCFG ciDbtcfg;
    REG_CiTDC ciTdc;

    ciDbtcfg.word = canControlResetValues[cREGADDR_CiDBTCFG / 4];
    ciTdc.word = 0;

    // Configure Bit time and sample point
    ciTdc.bF.TDCMode = CAN_SSP_MODE_AUTO;
    uint32_t tdcValue = 0;

    // Data Bit rate and SSP
    switch (bitTime) {
        case CAN_500K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_2M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 2;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 3;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_4M:
        case CAN_500K_5M:
        case CAN_500K_6M7:
        case CAN_500K_8M:
        case CAN_500K_10M:
        case CAN_1000K_4M:
        case CAN_1000K_8M:
            //qDebug("Data Bitrate not feasible with this clock!");



        case CAN_250K_500K:
        case CAN_125K_500K:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 14;
            ciDbtcfg.bF.TSEG2 = 3;
            ciDbtcfg.bF.SJW = 3;
            // SSP
            ciTdc.bF.TDCOffset = 15;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_833K:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 7;
            ciDbtcfg.bF.TSEG2 = 2;
            ciDbtcfg.bF.SJW = 2;
            // SSP
            ciTdc.bF.TDCOffset = 8;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_1M5:
            //qDebug("Data Bitrate not feasible with this clock!");


        case CAN_250K_2M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 2;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 3;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_3M:
        case CAN_250K_4M:
            //qDebug("Data Bitrate not feasible with this clock!");

    }

    // Write Bit time registers
    SPI_Write_Word( cREGADDR_CiDBTCFG, ciDbtcfg.word);
    SPI_Write_Word(cREGADDR_CiTDC, ciTdc.word);

}
// Section: Time Stamp
/*
 * there should be a function that takes care of overflowing of the time stamp counter
 */
void CANFD_TimeStamp_Enable(void)
{

    uint8_t d = 0;
    // Read
    SPI_Read_Byte(cREGADDR_CiTSCON + 2, &d);
    // Modify
    d |= 0x01;
    // Write
    SPI_Write_Byte(cREGADDR_CiTSCON + 2, d);

}
void CANFD_TimeStamp_Disable(void)
{
    uint8_t d = 0;

    // Read
    SPI_Read_Byte(cREGADDR_CiTSCON + 2, &d);

    // Modify
    d &= 0x06;

    // Write
    SPI_Write_Byte(cREGADDR_CiTSCON + 2, d);

}
void CANFDSPI_TimeStamp_Counter_Get(uint32_t* ts)
{
    // Read
    SPI_Read_Word(cREGADDR_CiTBC, ts);

}

void CANFD_TimeStamp_Counter_Set(uint32_t ts)
{
    // Write
    SPI_Write_Word(cREGADDR_CiTBC, ts);
}
void CANFD_TimeStamp_Mode_Configure(CAN_TS_MODE mode)
{
    uint8_t d = 0;
    // Read
    SPI_Read_Byte(cREGADDR_CiTSCON + 2, &d);
    // Modify
    d &= 0x01;
    d |= mode << 1;

    // Write
    SPI_Write_Byte(cREGADDR_CiTSCON + 2, d);
}
/*
 * -1 , from systemclock
 */
void CANFD_TimeStamp_Prescaler_Set(uint16_t ps)
{
    // Write
    SPI_Write_Half_Word( cREGADDR_CiTSCON, ps);

}
//*****************************************************************************************************************************************************************
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
        SPI_Write_Byte_Array_TX( a, txd, SPI_DEFAULT_BUFFER_LENGTH);
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
    uint8_t d = 0;
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
    SPI_Read_Byte(cREGADDR_CiCON + 2, &d);

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
    SPI_Read_Byte( a, &fCtrl.byte);

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
    SPI_Read_Byte( a, &fCtrl.byte);

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
    SPI_Write_Word(a, mObj.word);

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
void CANFD_ReceiveFIOF_Configure(CAN_FIFO_INDEX buffer_index,CAN_RX_FIFO_CONFIG* config)
{

    uint16_t a = 0;

//    if (buffer_index == CAN_TXQUEUE_CH0) {
//        return -100;
//    }

    // Setup FIFO
    REG_CiFIFOCON ciFifoCon;
    ciFifoCon.word = canFifoResetValues[0];

    ciFifoCon.rxBF.TxEnable = 0;
    ciFifoCon.rxBF.FifoSize = config->FifoSize;
    ciFifoCon.rxBF.PayLoadSize = config->PayLoadSize;
    ciFifoCon.rxBF.RxTimeStampEnable = config->RxTimeStampEnable;

    a = cREGADDR_CiFIFOCON + (buffer_index * CiFIFO_OFFSET);

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

void CANFD_Tef_Configure(CAN_TEF_CONFIG* config)
{
    // Setup FIFO
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = canFifoResetValues[0];

    ciTefCon.bF.FifoSize = config->FifoSize;
    ciTefCon.bF.TimeStampEnable = config->TimeStampEnable;

    SPI_Write_Word(cREGADDR_CiTEFCON, ciTefCon.word);
}

void CANFD_Tef_Configure_Object_Reset(CAN_TEF_CONFIG* config)
{
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = canFifoResetValues[0];

    config->FifoSize = ciTefCon.bF.FifoSize;
    config->TimeStampEnable = ciTefCon.bF.TimeStampEnable;

}
//*****************************************************************************************************************************************************************

/*
 *  CiINT – INTERRUPT REGISTER
 *
 *
 *   RECEIVE/tx event  FIFO INTERRUPT – RFIF
 *   All three interrupts can be enabled individually. The interrupts can not be cleared by the
application; they will be cleared when the condition of the FIFO terminates.
The three interrupt sources are OR’d together and reflected in the CiRXIF.RFIF[m] flag.

CAN BUS ERROR INTERRUPT – CERRIF
The CERRIF flag will be set based on the error counter values. The flag must be cleared by the
application.
CERRIF will be set each time a threshold in the TEC/REC counter is crossed by the following
conditions:
• TEC or REC exceeds the Error Warning state threshold
• The transmitter or receiver transitions to Error Passive state
• The transmitter transitions to Bus Off state
• The transmitter or receiver transitions from Error Passive to Error Active state
• The module transitions from Bus Off to Error Active state, after the bus off recovery
sequence
When the user clears CERRIF, it will remain clear until a new counter crossing occurs.

If more than one object has an interrupt pending, the interrupt or FIFO with the highest number
will show up in RXCODE, TXCODE and ICODE. Once the interrupt with the highest priority is
cleared, the next highest priority interrupt will show up in CiVEC. RXCODE, TXCODE and
ICODE are implemented with combinatorial logic using the interrupt flags as inputs.

 */
// interputs
void CANFD_Interrupt_Enable( CAN_MODULE_EVENT flags)
{
    uint16_t a = 0;

    // Read Interrupt Enables
    a = cREGADDR_CiINTENABLE;
    REG_CiINTENABLE intEnables;
    intEnables.word = 0;

    SPI_Read_Half_Word(a, &intEnables.word);

    // Modify
    intEnables.word |= (flags & CAN_ALL_EVENTS);

    // Write
    SPI_Write_Half_Word(a, intEnables.word);

}
void CANFD_Interrupt_Disable( CAN_MODULE_EVENT flags)
{
    uint16_t a = 0;

    // Read Interrupt Enables
    a = cREGADDR_CiINTENABLE;
    REG_CiINTENABLE intEnables;
    intEnables.word = 0;

    SPI_Read_Half_Word( a, &intEnables.word);

    // Modify
    intEnables.word &= ~(flags & CAN_ALL_EVENTS);

    // Write
    SPI_Write_Half_Word( a, intEnables.word);

}
void CANFD_Interrupt_Event_Clear(CAN_MODULE_EVENT flags)
{
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
    SPI_Write_Half_Word(a, intFlags.word);
}
void CANFD_Interrupt_Source_Get(CAN_MODULE_EVENT* flags)
{
    // Read Interrupt flags
    REG_CiINTFLAG intFlags;
    intFlags.word = 0;

    SPI_Read_Half_Word( cREGADDR_CiINTFLAG, &intFlags.word);

    // Update data
    *flags = (CAN_MODULE_EVENT) (intFlags.word & CAN_ALL_EVENTS);

}


void CANFD_Interrupt_EventRxCodeGet(CAN_RXCODE* rxCode)
{
    uint16_t a = 0;
    uint8_t rxCodeByte = 0;

    // Read
    a = cREGADDR_CiVEC + 3;

    SPI_Read_Byte(a, &rxCodeByte);

    // Decode data
    // 0x40 = "no interrupt" (CAN_FIFO_CIVEC_NOINTERRUPT)
    if ((rxCodeByte < CAN_RXCODE_TOTAL_CHANNELS) || (rxCodeByte == CAN_RXCODE_NO_INT)) {
        *rxCode = (CAN_RXCODE) rxCodeByte;
    }
    else
    {
        *rxCode = CAN_RXCODE_RESERVED; // shouldn't get here
    }
}
void CANFD_Interrupt_EventTxCodeGet(CAN_TXCODE* txCode)
{
    uint16_t a = 0;
    uint8_t txCodeByte = 0;

    // Read
    a = cREGADDR_CiVEC + 2;

    SPI_Read_Byte(a,  &txCodeByte);

    // Decode data
    // 0x40 = "no interrupt" (CAN_FIFO_CIVEC_NOINTERRUPT)
    if ((txCodeByte < CAN_TXCODE_TOTAL_CHANNELS) || (txCodeByte == CAN_TXCODE_NO_INT)) {
        *txCode = (CAN_TXCODE) txCodeByte;
    }
    else
    {
        *txCode = CAN_TXCODE_RESERVED; // shouldn't get here
    }
}
void CANFD_Interrupt_Event_Filter_Hit_Get(CAN_FILTER* filterHit)
{
    uint16_t a = 0;
    uint8_t filterHitByte = 0;

    // Read
    a = cREGADDR_CiVEC + 1;

    SPI_Read_Byte(a, &filterHitByte);

    // Update data
    *filterHit = (CAN_FILTER) filterHitByte;

}
void CANFD_Interrupt_Event_Icode_Get(CAN_ICODE* icode)
{
    uint16_t a = 0;
    uint8_t icodeByte = 0;

    // Read
    a = cREGADDR_CiVEC;

    SPI_Read_Byte(a, &icodeByte);

    // Decode
    if ((icodeByte < CAN_ICODE_RESERVED) && ((icodeByte < CAN_ICODE_TOTAL_CHANNELS) || (icodeByte >= CAN_ICODE_NO_INT)))
    {
        *icode = (CAN_ICODE) icodeByte;
    }
    else
    {
        *icode = CAN_ICODE_RESERVED; // shouldn't get here
    }
}
void CANFD_Tef_Interrupt_Source_Get(CAN_TEF_FIFO_EVENT* flags)
{
    uint16_t a = 0;

    // Read Interrupt flags
    REG_CiTEFSTA ciTefSta;
    ciTefSta.word = 0;
    a = cREGADDR_CiTEFSTA;

    SPI_Read_Byte(a, &ciTefSta.byte[0]);

    // Update data
    *flags = (CAN_TEF_FIFO_EVENT) (ciTefSta.byte[0] & CAN_TEF_FIFO_ALL_EVENTS);
}
void CANFD_Tef_Interrupt_Enable(CAN_TEF_FIFO_EVENT flags)
{
    uint16_t a = 0;

    // Read Interrupt Enables
    a = cREGADDR_CiTEFCON;
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = 0;

    SPI_Read_Byte( a, &ciTefCon.byte[0]);

    // Modify
    ciTefCon.byte[0] |= (flags & CAN_TEF_FIFO_ALL_EVENTS);

    // Write
    SPI_Write_Byte( a, ciTefCon.byte[0]);
}
void CANFD_Tef_Interrupt_Disable(CAN_TEF_FIFO_EVENT flags)
{
    uint16_t a = 0;

    // Read Interrupt Enables
    a = cREGADDR_CiTEFCON;
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = 0;

    SPI_Read_Byte(a, &ciTefCon.byte[0]);

    // Modify
    ciTefCon.byte[0] &= ~(flags & CAN_TEF_FIFO_ALL_EVENTS);

    // Write
    SPI_Write_Byte( a, ciTefCon.byte[0]);
}

void CANFD_Tef_Status_Get(CAN_TEF_FIFO_STATUS* status)
{
    uint16_t a = 0;

    // Read
    REG_CiTEFSTA ciTefSta;
    ciTefSta.word = 0;
    a = cREGADDR_CiTEFSTA;

    SPI_Read_Byte(a, &ciTefSta.byte[0]);

    // Update data
    *status = (CAN_TEF_FIFO_STATUS) (ciTefSta.byte[0] & CAN_TEF_FIFO_STATUS_MASK);
}

void CANFD_Tef_Message_Get(CAN_TEF_MSGOBJ* tefObj)
{
    uint16_t a = 0;
    uint32_t fifoReg[3];
    uint8_t n = 0;

    // Get FIFO registers
    a = cREGADDR_CiTEFCON;

    SPI_Read_Word_Array(a, fifoReg, 3);

    // Get control
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = fifoReg[0];

    // Get status
    __attribute__((unused)) REG_CiTEFSTA ciTefSta;
    ciTefSta.word = fifoReg[1];

    // Get address
    REG_CiFIFOUA ciTefUa;
    ciTefUa.word = fifoReg[2];

    a = ciTefUa.bF.UserAddress;

    a += cRAMADDR_START;

    // Number of bytes to read
    n = 8; // 8 header bytes

    if (ciTefCon.bF.TimeStampEnable)
    {
        n += 4;  // Add 4 time stamp bytes
    }

    // Read rxObj using one access
    uint8_t ba[12];

    SPI_Read_Byte_Array(a, ba, n);

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

    // Set UINC ,increment
    CANFD_Tef_Increment();
}
/*
 *  FIFO will be reset when bit is set, cleared by hardware when FIFO was reset. The user should
 *  wait for this bit to clear before taking any action.
 */

void CANFD_Tef_Reset(void)
{

    uint16_t a = 0;

    // Set FRESET
    a = cREGADDR_CiTEFCON + 1;
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = 0;
    ciTefCon.bF.FRESET = 1;

    // Write byte
    SPI_Write_Byte( a, ciTefCon.byte[1]);

}

void CANFD_Tef_Increment(void)
{
    uint16_t a = 0;
    // Set UINC
    a = cREGADDR_CiTEFCON + 1;
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = 0;
    ciTefCon.bF.UINC = 1;

    // Write byte
    SPI_Write_Byte(a, ciTefCon.byte[1]);

}
void CANFD_Receive_Message_Get(CAN_FIFO_INDEX buffer_index, CAN_RX_MSGOBJ* rxObj,uint8_t *rxd, uint8_t nBytes)
{
    uint8_t n = 0;
    uint8_t i = 0;
    uint16_t a;
    uint32_t fifoReg[3];
    REG_CiFIFOCON ciFifoCon;
    __attribute__((unused)) REG_CiFIFOSTA ciFifoSta;
    REG_CiFIFOUA ciFifoUa;

    // Get FIFO registers
    a = cREGADDR_CiFIFOCON + (buffer_index * CiFIFO_OFFSET);

    SPI_Read_Word_Array( a, fifoReg, 3);

    // Check that it is a receive buffer
    ciFifoCon.word = fifoReg[0];

    // Get Status
    ciFifoSta.word = fifoReg[1];

    // Get address
    ciFifoUa.word = fifoReg[2];

    a = ciFifoUa.bF.UserAddress;

    a += cRAMADDR_START;

    // Number of bytes to read
    n = nBytes + 8;  // Add 8 header bytes  , first 2 words

    if (ciFifoCon.rxBF.RxTimeStampEnable) {
        n += 4; // Add 4 time stamp bytes , if it is configured
    }

    // Make sure we read a multiple of 4 bytes from RAM
    if (n % 4)
    {
        n = n + 4 - (n % 4);
    }

    // Read rxObj using one access
    uint8_t ba[MAX_MSG_SIZE];

    if (n > MAX_MSG_SIZE)
    {
        n = MAX_MSG_SIZE;
    }

    SPI_Read_Byte_Array( a, ba, n);


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

    if (ciFifoCon.rxBF.RxTimeStampEnable)
    {
        myReg.byte[0] = ba[8];
        myReg.byte[1] = ba[9];
        myReg.byte[2] = ba[10];
        myReg.byte[3] = ba[11];
        rxObj->word[2] = myReg.word;

        // Assign message data
        for (i = 0; i < nBytes; i++)
        {
            rxd[i] = ba[i + 12];
        }
    }
    else
    {
        rxObj->word[2] = 0;

        // Assign message data
        for (i = 0; i < nBytes; i++)
        {
            rxd[i] = ba[i + 8];
        }
    }

    // UINC channel
    CANFD_Receive_Fifo_Increment(buffer_index);
}

void CANFD_Receive_fifo_Reset(CAN_FIFO_INDEX buffer_index)
{
    uint16_t a = 0;
    REG_CiFIFOCON ciFifoCon;

    // Address aSPI_Read_Bytend data
    a = cREGADDR_CiFIFOCON + (buffer_index * CiFIFO_OFFSET) + 1; // Byte that contains FRESET
    ciFifoCon.word = 0;
    ciFifoCon.rxBF.FRESET = 1;

    SPI_Write_Byte(a, ciFifoCon.byte[1]);
}

void CANFD_Receive_Fifo_Increment(CAN_FIFO_INDEX buffer_index)
{
    uint16_t a = 0;
    REG_CiFIFOCON ciFifoCon;

    ciFifoCon.word = 0;

    // Set UINC
    a = cREGADDR_CiFIFOCON + (buffer_index * CiFIFO_OFFSET) + 1; // Byte that contains FRESET
    ciFifoCon.rxBF.UINC = 1;

    // Write byte
    SPI_Write_Byte( a, ciFifoCon.byte[1]);

}
void  CANFD_Receive_Channel_Status_Get(CAN_FIFO_INDEX buffer_index , CAN_RX_FIFO_STATUS* status)
{
    uint16_t a;
    REG_CiFIFOSTA ciFifoSta;


    // Read
    ciFifoSta.word = 0;
    a = cREGADDR_CiFIFOSTA + (buffer_index * CiFIFO_OFFSET);

    SPI_Read_Byte( a, &ciFifoSta.byte[0]);

    // Update data
    *status = (CAN_RX_FIFO_STATUS) (ciFifoSta.byte[0] & 0x0F);

}

// Section: Error Handling

void CANFD_Error_TX_Counter_Get(uint8_t* tec)
{
    uint16_t a = 0;
    // Read Error count
    a = cREGADDR_CiTREC + 1;
    SPI_Read_Byte( a, tec);
}
void CANFD_Error_RX_Counter_Get(uint8_t* rec)
{
    uint16_t a = 0;
    // Read Error count
    a = cREGADDR_CiTREC;

    SPI_Read_Byte( a, rec);
}
/*
 * TXBO TXBP RXBP TXWARN RXWARN EWARN
 */
void DRV_CANFDSPI_Error_State_Get(CAN_ERROR_STATE* flags)
{
    uint16_t a = 0;
    // Read Error state
    a = cREGADDR_CiTREC + 2;
    uint8_t f = 0;

    SPI_Read_Byte(a, &f);

    // Update data
    *flags = (CAN_ERROR_STATE) (f & CAN_ERROR_ALL);

}

void CANFD_Bus_Diagnostics_Get(CAN_BUS_DIAGNOSTIC* bd)
{
    uint16_t a = 0;

    // Read diagnostic registers all in one shot
    a = cREGADDR_CiBDIAG0;
    uint32_t w[2];

    SPI_Read_Word_Array(a, w, 2);
    // Update data
    CAN_BUS_DIAGNOSTIC b;
    b.word[0] = w[0];
    b.word[1] = w[1] & 0x0000ffff;
    b.word[2] = (w[1] >> 16) & 0x0000ffff;
    *bd = b;

}

void DRV_CANFDSPI_BusDiagnosticsClear(void)
{
    uint8_t a = 0;

    // Clear diagnostic registers all in one shot
    a = cREGADDR_CiBDIAG0;
    uint32_t w[2];
    w[0] = 0;
    w[1] = 0;
    SPI_Write_Word_Array( a, w, 2);
}
//*******************************************************************************************************************************************************************

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
//    if (!ciFifoCon.txBF.TxEnable)                       // Check that it is a transmit buffer
//    {
//        return -2;
//    }

    // Check that DLC is big enough for data
    dataBytesInObject = CANFD_Dlc_To_Data_Bytes((CAN_DLC) txObj->bF.ctrl.DLC);  // from the lookup table get the no. of bytes to be send
//    if (dataBytesInObject < txdNumBytes)
//    {
//        return -3;
//    }

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

    SPI_Write_Byte_Array_TX(a,txBuffer, txdNumBytes + 8 + n);

    // Set UINC and TXREQ
    //              ^^^^^
    CANFD_Tx_Fifo_Transmit(fifo_index, REQ);

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
    SPI_Write_Word(cREGADDR_CiTXREQ, w);

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
    a = cREGADDR_CiFIFOCON + (fifo_index * CiFIFO_OFFSET);
    a += 1; // byte address of TXREQ

    // Clear TXREQ
    d = 0x00;

    // Write
    SPI_Write_Byte(a, d);
}
uint32_t CANFD_Dlc_To_Data_Bytes(CAN_DLC dlc)
{
    uint32_t dataBytesInObject = 0;

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

