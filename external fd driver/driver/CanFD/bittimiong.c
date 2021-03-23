/*
 * bittimiong.c
 *
 *  Created on: Mar 17, 2021
 *      Author: mostafa
 */




// *****************************************************************************
// *****************************************************************************
// Section: Oscillator and Bit Time

int8_t CANFD_Bit_Time_Configure( CAN_BITTIME_SETUP bitTime, CAN_SSP_MODE sspMode, CAN_SYSCLK_SPEED clk)
{
    int8_t spiTransferError = 0;

    // Decode clk
    switch (clk) {
        case CAN_SYSCLK_40M:
            CANFD_Bit_Time_Configure_Nominal_40MHz(index, bitTime);
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

void DRV_CANFDSPI_BitTimeConfigureNominal40MHz(CAN_BITTIME_SETUP bitTime)
{
    int8_t spiTransferError = 0;
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
        case :
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

        default:
            return -1;
            break;
    }

    // Write Bit time registers
    SPI_Write_Word(cREGADDR_CiNBTCFG, ciNbtcfg.word);

}

void DRV_CANFDSPI_BitTimeConfigureData40MHz(CAN_BITTIME_SETUP bitTime)
{
    int8_t spiTransferError = 0;
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

        default:
            return -1;
            break;
    }

    // Write Bit time registers
    SPI_Write_Word( cREGADDR_CiDBTCFG, ciDbtcfg.word);

    SPI_Write_Word( cREGADDR_CiTDC, ciTdc.word);

}

void CANFD_Bit_Time_Configure_Nominal_20MHz(CAN_BITTIME_SETUP bitTime)
{
    int8_t spiTransferError = 0;
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

        default:
            return -1;
            break;
    }

    // Write Bit time registers
    SPI_Write_Word( cREGADDR_CiNBTCFG, ciNbtcfg.word);
}

void CANFD_Bit_Time_Configure_Data_20MHz( CAN_BITTIME_SETUP bitTime, CAN_SSP_MODE sspMode)
{
    int8_t spiTransferError = 0;
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
            return -1;
            break;

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
            return -1;
            break;
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

        default:
            return -1;
            break;
    }

    // Write Bit time registers
    SPI_Write_Word( cREGADDR_CiDBTCFG, ciDbtcfg.word);
    SPI_Write_Word(index, cREGADDR_CiTDC, ciTdc.word);

}

void CANFD_Bit_Time_Configure_Nominal_10MHz(CAN_BITTIME_SETUP bitTime)
{
    int8_t spiTransferError = 0;
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
            break;

        default:
            return -1;
            break;
    }

    // Write Bit time registers
    SPI_Write_Word( cREGADDR_CiNBTCFG, ciNbtcfg.word);
}

int8_t CANFD_Bit_Time_Configure_Data_10MHz(CAN_BITTIME_SETUP bitTime)
{
    int8_t spiTransferError = 0;
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
            return -1;
            break;

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
            return -1;
            break;
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
            return -1;
            break;

        default:
            return -1;
            break;
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


int8_t CANFD_TimeStamp_Mode_Configure(CAN_TS_MODE mode)
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
int8_t CANFD_TimeStamp_Prescaler_Set(uint16_t ps)
{
    // Write
    SPI_Write_Half_Word( cREGADDR_CiTSCON, ps);

}
