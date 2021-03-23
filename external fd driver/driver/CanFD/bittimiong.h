/*
 * bittimiong.h
 *
 *  Created on: Mar 18, 2021
 *      Author: mostafa
 */

#ifndef DRIVER_MCP25XXFD_DRIVER_CANFD_BITTIMIONG_H_
#define DRIVER_MCP25XXFD_DRIVER_CANFD_BITTIMIONG_H_


void DRV_CANFDSPI_BitTimeConfigure( CAN_BITTIME_SETUP bitTime, CAN_SSP_MODE sspMode,CAN_SYSCLK_SPEED clk);
void DRV_CANFDSPI_BitTimeConfigureNominal40MHz(CAN_BITTIME_SETUP bitTime);
void DRV_CANFDSPI_BitTimeConfigureData40MHz(CAN_BITTIME_SETUP bitTime, CAN_SSP_MODE sspMode);
void DRV_CANFDSPI_BitTimeConfigureNominal20MHz(CAN_BITTIME_SETUP bitTime);
void DRV_CANFDSPI_BitTimeConfigureData20MHz( CAN_BITTIME_SETUP bitTime, CAN_SSP_MODE sspMode);




#endif /* DRIVER_MCP25XXFD_DRIVER_CANFD_BITTIMIONG_H_ */
