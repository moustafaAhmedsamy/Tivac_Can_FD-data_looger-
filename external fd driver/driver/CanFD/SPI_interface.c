/*
 * SPI_interface.c
 *
 *  Created on: Mar 17, 2021
 *      Author: mostafa
 *                                       Section: SPI interface
 *
 * RESET C = 0b0000; A = 0x000 Resets internal registers to default state; selects Configuration mode.
 * READ  C = 0b0011; A; D = SDO Read SFR/RAM from address A.
 * WRITE C = 0b0010; A; D = SDI Write SFR/RAM to address A.

 * The 4-bit command and the 12-bit address are shifted into SDI on the rising edge of SCK.

 * During a write instruction, data bits are shifted into SDI on the rising edge of SCK.

 * During a read instruction,data bits are shifted out of SDO on the falling edge of SCK.

 * One or more data bytes are transfered with one instruction.

 * Each instruction ends with driving CS high.
 *
 *  interfaces
 *  ^^^^^^^^^^
    SPI_Reset(void)
    SPI_Write_Byte_Array(const uint8_t *buf, uint16_t count)
    SPI_Write_Word( uint16_t address,uint32_t txd)
    SPI_Write_Half_Word(uint16_t address,uint16_t txd)
    SPI_Write_Word_Array(uint16_t address, uint32_t *txd, uint16_t nWords)
    SPI_Read_Byte(uint16_t address, uint8_t *rxd)
    SPI_Read_Byte_Array(uint16_t address, uint8_t *rxd, uint16_t nBytes)
    SPI_Read_Word(uint16_t address, uint32_t *rxd)
    SPI_Read_Word(uint16_t address, uint32_t *rxd)
    SPI_Read_Half_Word(uint16_t address, uint16_t *rxd)
    SPI_Read_Word_Array( uint16_t address,uint32_t *rxd, uint16_t nWords)

 */


#include "stdint.h"
#include "CanFd_register.h"
#include "SPI_interface.h"
#include "../spi/spi.h"


uint8_t spiTransmitBuffer[SPI_DEFAULT_BUFFER_LENGTH];

//! SPI Receive buffer
uint8_t spiReceiveBuffer[SPI_DEFAULT_BUFFER_LENGTH];

void SPI_Reset(void)
{
   //uint16_t spiTransferSize = 2;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) (cINSTRUCTION_RESET << 4);
    spiTransmitBuffer[1] = 0;

    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(spiTransmitBuffer[0]);
    spi_send(spiTransmitBuffer[1]);
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);

}
void SPI_Write_Byte_Array_TX(uint16_t address ,const uint8_t *buf, uint16_t count)
{
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);

    spi_send(spiTransmitBuffer[0]);
    spi_send(spiTransmitBuffer[1]);

    int i;
    for (i = 0; i < count; i++) {
        spi_send(*buf);
        buf++;
    }
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);

}
void SPI_Write_Byte(uint16_t address , uint8_t byte )
{
        spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF));
        spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

        MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
        spi_send(spiTransmitBuffer[0]);
        spi_send(spiTransmitBuffer[1]);
        spi_send(byte);
        MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}

void SPI_Write_Byte_Array(const uint8_t *buf, uint16_t count)
{
        MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
        int i;
        for (i = 0; i < count; i++) {
            spi_send(*buf);
            buf++;
        }
        MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}

void SPI_Write_Word( uint16_t address,uint32_t txd)
{
    uint8_t i;
    uint16_t spiTransferSize = 6;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    // Split word into 4 bytes and add them to buffer
    for (i = 0; i < 4; i++) {
        spiTransmitBuffer[i + 2] = (uint8_t) ((txd >> (i * 8)) & 0xFF);
    }
    SPI_Write_Byte_Array(spiTransmitBuffer , spiTransferSize);
}

void SPI_Write_Half_Word(uint16_t address,uint16_t txd)
{
    uint8_t i;
    uint16_t spiTransferSize = 4;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    // Split word into 2 bytes and add them to buffer
    for (i = 0; i < 2; i++) {
        spiTransmitBuffer[i + 2] = (uint8_t) ((txd >> (i * 8)) & 0xFF);
    }
    SPI_Write_Byte_Array(spiTransmitBuffer, spiTransferSize);
}

void SPI_Write_Word_Array(uint16_t address, uint32_t *txd, uint16_t nWords)
{
    uint16_t i, j, n;
    REG_t w;                       // union
    uint16_t spiTransferSize = nWords * 4 + 2;

    // Compose command
    spiTransmitBuffer[0] = (cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF);
    spiTransmitBuffer[1] = address & 0xFF;

    // Convert ByteArray to word array
    n = 2;
    for (i = 0; i < nWords; i++) {
        w.word = txd[i];
        for (j = 0; j < 4; j++, n++) {
            spiTransmitBuffer[n] = w.byte[j];
        }
    }
    SPI_Write_Byte_Array(spiTransmitBuffer, spiTransferSize);
}

void SPI_Read_Byte(uint16_t address, uint8_t *rxd)
{

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);
    spiTransmitBuffer[2] = 0;      //clear this byte to make sure that is clear before writing

    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(spiTransmitBuffer[0]);
    spi_send(spiTransmitBuffer[1]);
    *rxd = spi_send(spiTransmitBuffer[2]);
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);

}

void SPI_Read_Byte_Array(uint16_t address, uint8_t *rxd, uint16_t nBytes)
{
    uint16_t j;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);
    spiTransmitBuffer[2] = 0;      //clear this byte to make sure that is clear before writing

    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(spiTransmitBuffer[0]);
    spi_send(spiTransmitBuffer[1]);

    for (j=0;j<nBytes;j++)
    {
        rxd[j]= spi_send(spiTransmitBuffer[2]);
    }
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}

void SPI_Read_Word(uint16_t address, uint32_t *rxd)
{
    uint8_t i,j;
    uint32_t x;
//    uint32_t y ;
//    uint16_t spiTransferSize = 6;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(spiTransmitBuffer[0]);
    spi_send(spiTransmitBuffer[1]);

    for (j=0;j<4;j++)
        {
        spiReceiveBuffer[j]= spi_send(spiTransmitBuffer[2]);
        }
        MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);

    // Update data
    *rxd = 0;
    for (i = 0; i < 4; i++) {
        x = (uint32_t) spiReceiveBuffer[i];
        *rxd += x << ((i )*8);
        //y |= (  (spiReceiveBuffer[i] << (i*8)) & (0xff << i*8) );
    }
    //*rxd = y;
}

void SPI_Read_Half_Word(uint16_t address, uint16_t *rxd)
{

    //uint16_t spiTransferSize = 4;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, 0);
    spi_send(spiTransmitBuffer[0]);
    spi_send(spiTransmitBuffer[1]);

    *rxd = spi_send(spiTransmitBuffer[2]) & 0xff;
    *rxd |= ((spi_send(spiTransmitBuffer[2]) << 8) &0xff00);
    MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
}

void SPI_Read_Word_Array( uint16_t address,uint32_t *rxd, uint16_t nWords)
{
    uint16_t i, j, n;
    REG_t w;
    uint16_t spiTransferSize = nWords * 4 + 2;

    for(i=0;i<spiTransferSize;i++)
    {
        spiReceiveBuffer[i]=0;
    }
    SPI_Read_Byte_Array(address, spiReceiveBuffer, nWords * 4);

    // Convert Byte array to Word array
    n = 2;
    for (i = 0; i < nWords; i++) {
        w.word = 0;
        for (j = 0; j < 4; j++, n++) {
            w.byte[j] = spiReceiveBuffer[n];
        }
        rxd[i] = w.word;
    }
}

