/*
 * interupts.c
 *
 *  Created on: Mar 22, 2021
 *      Author: mostafa
 */

#include "drv_canfdspi_api.h"
#include "drv_canfdspi_register.h"
#include "drv_canfdspi_defines.h"

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
void CANFD_Interrupt_EventRxCodeGet(CAN_TXCODE* txCode)
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
void CANFD_Tef_Interrupt_Enable(CAN_TEF_FIFO_EVENT flags)
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

void CANFD_Tef_Status_Get(CANFDSPI_MODULE_ID index,CAN_TEF_FIFO_STATUS* status)
{
    uint16_t a = 0;

    // Read
    REG_CiTEFSTA ciTefSta;
    ciTefSta.word = 0;
    a = cREGADDR_CiTEFSTA;

    DRV_CANFDSPI_ReadByte(a, &ciTefSta.byte[0]);

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

    DRV_CANFDSPI_ReadWordArray(a, fifoReg, 3);

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
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Set FRESET
    a = cREGADDR_CiTEFCON + 1;
    REG_CiTEFCON ciTefCon;
    ciTefCon.word = 0;
    ciTefCon.bF.FRESET = 1;

    // Write byte
    SPI_Write_Byte(index, a, ciTefCon.byte[1]);

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
//SPI_Read_Byte
//SPI_Write_Byte
