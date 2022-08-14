#ifndef __MEMORY_H
#define __MEMORY_H
#include "main.h"

#define MAIN_PROGRAM_START_ADDR   (uint32_t)0x08000000
#define FIRST_PAGE_PROPERTY_ADDR  (uint32_t)0x0800F800
#define FIRST_PAGE_PROPERTY_SIZE  (uint32_t)0x400
#define SECOND_PAGE_PROPERTY_ADDR (uint32_t)0x0800FC00
#define SECOND_PAGE_PROPERTY_SIZE (uint32_t)0x400

typedef struct{
	uint32_t uartBaudrate;
	uint8_t deviceAddress;
  uint8_t reserve1;
  uint8_t reserve2;
  uint8_t reserve3;	
}UartProperty;

typedef struct{
	UartProperty uartProperty;
	//AdjPoint adjPoints[ADJ_POINTS_COUNT];
}Memory;

void Memory_Init(Memory* memory);
void memoryWriteHoldingRegisters(uint16_t* registers,uint16_t startAddr, uint16_t regCount);

#endif

