/*******************************************************************************
  CAN FD SPI Driver: API Functions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_canfdspi_api.h

  Summary:
    This header file provides the API function prototypes for the CAN FD SPI
    controller.

  Description:
    API function prototypes for the CAN FD SPI controller family:
 * MCP2517FD, and MCP2518FD.
 * .
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

#ifndef _DRV_CANFDSPI_API_H
#define _DRV_CANFDSPI_API_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files

#include <driver/mcp25xxfd_driver/CanFD/drv_canfdspi_defines.h>
#include <stdint.h>
#include <stdbool.h>


void CANFD_Configure(CAN_CONFIG* config);
void CANFD_RamInit(void);
void CANFD_ConfigureObjectReset(CAN_CONFIG* config);
void CANFD_OperationModeSelect(CAN_OPERATION_MODE opMode);
CAN_OPERATION_MODE CANFD_OperationModeGet(void);
void CANFD_Filter_Enable(CAN_FILTER filter);
void CANFD_Filter_Disable(CAN_FILTER filter);
void CANFD_Filter_Mask_Objects_Configure(CAN_FILTER filter, CAN_FILTEROBJ_ID* id, CAN_MASKOBJ_ID* mask , CAN_FIFO_INDEX fifo_index);
void CANFD_TransmitFIOF_Configure( CAN_FIFO_INDEX fifo_index, CAN_TX_FIFO_CONFIG* config);
void CANFD_TransmitFIOF_ConfigureObject_Reset(CAN_TX_FIFO_CONFIG* config);
int8_t CANFD_ReceiveFIOF_Configure(CAN_FIFO_INDEX buffer_index,CAN_RX_FIFO_CONFIG* config);
void CANFD_ReceiveChannelConfigureObjectReset(CAN_RX_FIFO_CONFIG* config);
void CANFD_Tef_Configure(CAN_TEF_CONFIG* config);
void CANFD_Tef_Configure_Object_Reset(CAN_TEF_CONFIG* config);

void CANFD_Transmit_Object_Load( CAN_FIFO_INDEX fifo_index , CAN_TX_MSGOBJ* txObj,uint8_t *txd, uint32_t txdNumBytes, bool REQ);
void CANFD_Tx_Fifo_Transmit(CAN_FIFO_INDEX fifo_index, bool REQ);
void CANFD_Transmit_Request_Fifos_Set(CAN_TXREQ_CHANNEL txreq);
void CANFD_Transmit_Request_Fifos_Get(uint32_t* txreq);
void CANFD_Transmit_Fifo_Abort(CAN_FIFO_INDEX fifo_index);
uint32_t CANFD_Dlc_To_Data_Bytes(CAN_DLC dlc);

void CANFD_Interrupt_Enable( CAN_MODULE_EVENT flags);
void CANFD_Interrupt_Disable( CAN_MODULE_EVENT flags);
void CANFD_Interrupt_Event_Clear(CAN_MODULE_EVENT flags);
void CANFD_Interrupt_Source_Get(CAN_MODULE_EVENT* flags);
void CANFD_Interrupt_EventRxCodeGet(CAN_RXCODE* rxCode);
void CANFD_Interrupt_EventTxCodeGet(CAN_TXCODE* txCode);
void CANFD_Interrupt_Event_Filter_Hit_Get(CAN_FILTER* filterHit);
void CANFD_Interrupt_Event_Icode_Get(CAN_ICODE* icode);
void CANFD_Tef_Interrupt_Source_Get(CAN_TEF_FIFO_EVENT* flags);
void CANFD_Tef_Interrupt_Enable(CAN_TEF_FIFO_EVENT flags);
void CANFD_Tef_Interrupt_Disable(CAN_TEF_FIFO_EVENT flags);
void CANFD_Tef_Status_Get(CANFDSPI_MODULE_ID index,CAN_TEF_FIFO_STATUS* status);
void CANFD_Tef_Message_Get(CAN_TEF_MSGOBJ* tefObj);
void CANFD_Tef_Reset(void);
void CANFD_Tef_Increment(void);
void CANFD_Receive_Message_Get(CAN_FIFO_INDEX buffer_index, CAN_RX_MSGOBJ* rxObj,uint8_t *rxd, uint8_t nBytes);
void CANFD_Receive_fifo_Reset(CAN_FIFO_INDEX buffer_index);
void CANFD_Receive_Fifo_Increment(CAN_FIFO_INDEX buffer_index);
void  CANFD_Receive_Channel_Status_Get(CAN_FIFO_INDEX buffer_index , CAN_RX_FIFO_STATUS* status);
void CANFD_Error_TX_Counter_Get(uint8_t* tec);
void DRV_CANFDSPI_Error_State_Get(CAN_ERROR_STATE* flags);
void CANFD_Bus_Diagnostics_Get(CAN_BUS_DIAGNOSTIC* bd);
void DRV_CANFDSPI_BusDiagnosticsClear(void);


int8_t CANFD_Bit_Time_Configure( CAN_BITTIME_SETUP bitTime, CAN_SSP_MODE sspMode, CAN_SYSCLK_SPEED clk);
void DRV_CANFDSPI_BitTimeConfigureNominal40MHz(CAN_BITTIME_SETUP bitTime);
void DRV_CANFDSPI_BitTimeConfigureData40MHz(CAN_BITTIME_SETUP bitTime);
void CANFD_Bit_Time_Configure_Nominal_20MHz(CAN_BITTIME_SETUP bitTime);
void CANFD_Bit_Time_Configure_Data_20MHz( CAN_BITTIME_SETUP bitTime, CAN_SSP_MODE sspMode);
void CANFD_Bit_Time_Configure_Nominal_10MHz(CAN_BITTIME_SETUP bitTime);
int8_t CANFD_Bit_Time_Configure_Data_10MHz(CAN_BITTIME_SETUP bitTime);

void CANFD_TimeStamp_Enable(void);
void CANFD_TimeStamp_Disable(void);
void CANFDSPI_TimeStamp_Counter_Get(uint32_t* ts);
void CANFD_TimeStamp_Counter_Set(uint32_t ts);
int8_t CANFD_TimeStamp_Mode_Configure(CAN_TS_MODE mode);
int8_t CANFD_TimeStamp_Prescaler_Set(uint16_t ps);


#endif // _DRV_CANFDSPI_API_H
