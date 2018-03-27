#ifndef BSP_M24LR6E_H
#define BSP_M24LR6E_H

#include <stdint.h>

extern int Bsp_M24lr26e_Write(uint32_t addrStart,
	uint8_t *pBuff,
	uint16_t length);

extern int Bsp_M24lr26e_Read(uint32_t addrStart,
	uint8_t *pBuff,
	uint16_t length);



#endif