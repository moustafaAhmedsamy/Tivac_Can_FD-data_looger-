/*
 * SPI_interface.h
 *
 *  Created on: Mar 17, 2021
 *      Author: mostafa
 */


//! SPI Transmit buffer
#define SPI_DEFAULT_BUFFER_LENGTH 96

void SPI_Reset(void);
void SPI_Write_Byte_Array(const uint8_t *buf, uint16_t count);
void SPI_Write_Byte_Array_TX(uint16_t address ,const uint8_t *buf, uint16_t count);
void SPI_Write_Byte(uint16_t address , uint8_t byte );
void SPI_Write_Word( uint16_t address,uint32_t txd);
void SPI_Write_Half_Word(uint16_t address,uint16_t txd);
void SPI_Write_Word_Array(uint16_t address, uint32_t *txd, uint16_t nWords);
void SPI_Read_Byte(uint16_t address, uint8_t *rxd);
void SPI_Read_Byte_Array(uint16_t address, uint8_t *rxd, uint16_t nBytes);
void SPI_Read_Word(uint16_t address, uint32_t *rxd);
void SPI_Read_Half_Word(uint16_t address, uint16_t *rxd);
void SPI_Read_Word_Array( uint16_t address,uint32_t *rxd, uint16_t nWords);
