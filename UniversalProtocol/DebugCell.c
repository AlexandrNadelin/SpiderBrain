#include "DebugCell.h"
#include "DebugCellFormat.h"
#include "UPCommon.h"
#include "Memory.h"
#include <stdio.h>
#include <string.h>
#include "DeviceData.h"

static uint32_t uartBaudrate =9600;
static uint32_t devAddr =0x01;//Во flash занимаеит 4 байта, чтобы удобнее было записывать

extern Memory memory;
extern DeviceData deviceData;

DebugCell debugCells[]=
{
	{
		.number=1,//Скорость передачи UART (если не используется USB CDC
		.attribute=DEBUG_CELL_ATTR_READ|DEBUG_CELL_ATTR_WRITE|DEBUG_CELL_ATTR_ROM,
		.format=DEBUG_CELL_FORMAT_UINT32,
		.name = "uartBaudrate",
		.data = (uint8_t*)&memory.uartProperty.uartBaudrate,
	},	
	{
		.number=2,//Адрес подключаемого датчика. Адрес этого устройства всегда 128 - устройство типа gate
		.attribute=DEBUG_CELL_ATTR_READ|DEBUG_CELL_ATTR_WRITE|DEBUG_CELL_ATTR_ROM,
		.format=DEBUG_CELL_FORMAT_UINT8,
		.name = "deviceAddress",
		.data = (uint8_t*)&memory.uartProperty.deviceAddress,
	},
	{
		.number=3,//Тангаж [град.]
		.attribute=DEBUG_CELL_ATTR_READ|DEBUG_CELL_ATTR_WRITE,
		.format=DEBUG_CELL_FORMAT_INT32,//
		.name = "engineLeftPWMPulseWidth",
		.data = (uint8_t*)&deviceData.engineLeftPWMPulseWidth,
	},
	{
		.number=4,//Крен [град.]
		.attribute=DEBUG_CELL_ATTR_READ|DEBUG_CELL_ATTR_WRITE,
		.format=DEBUG_CELL_FORMAT_INT32,//
		.name = "engineRightPWMPulseWidth",
		.data = (uint8_t*)&deviceData.engineRightPWMPulseWidth,
	},	
};

const uint16_t debugCellsCount = sizeof(debugCells)/sizeof(DebugCell);

/*__attribute__((weak)) */void preReadDebugCellCallback(unsigned short debugCellNumber)
{//
	//if(debugCellNumber==1)uartBaudrate = memory->uartProperty.uartBaudrate;//Читать скорость UART
	//else if(debugCellNumber==2)devAddr = memory->uartProperty.deviceAddress;//Читать адрес датчика (адрес самого устройства - 128
	/*else if(debugCellNumber==6)servo_1_shift = memory->servoProp[0].shift;//Читать сдвиг первого серводвигателя (в единицах времени - длительность импульса)
	else if(debugCellNumber==7)servo_1_multiplier = memory->servoProp[0].multiplier;//Читать множитель первого серводвигателя 
	else if(debugCellNumber==8)servo_2_shift = memory->servoProp[1].shift;//Читать сдвиг второго серводвигателя (в единицах времени - длительность импульса)
	else if(debugCellNumber==9)servo_2_multiplier = memory->servoProp[1].multiplier;//Читать множитель второго серводвигателя 
	else if(debugCellNumber==10)servo_3_shift = memory->servoProp[2].shift;//Читать сдвиг третьего серводвигателя (в единицах времени - длительность импульса)
	else if(debugCellNumber==11)servo_3_multiplier = memory->servoProp[2].multiplier;//Читать множитель третьего серводвигателя */
}

/*__attribute__((weak)) */void writeDebugCellCallback(unsigned short debugCellNumber)
{
	/*if(debugCellNumber==1)servo1.setPosition(&servo1, servoAngles[0]);//Установить положение первого серводвигателя
	else if(debugCellNumber==2)servo2.setPosition(&servo2, servoAngles[1]);//Установить положение второго серводвигателя
	else if(debugCellNumber==3)servo3.setPosition(&servo3, servoAngles[2]);//Установить положение третьего серводвигателя
	else */if(debugCellNumber==1)memoryWriteHoldingRegisters((uint16_t*)&uartBaudrate,0,2);//Записать скорость UART во Flash
	else if(debugCellNumber==2)memoryWriteHoldingRegisters((uint16_t*)&devAddr,2,2);//Записать адрес устройства во Flash
	else if(debugCellNumber==3)
	{//Устанавливаем мощность и направление вращения левого двигателя
		
	}
	else if(debugCellNumber==4)
	{//Устанавливаем мощность и направление вращения правого двигателя
		
	}
}


unsigned short processUPDebugRequestDataCallback(unsigned char* packetData,unsigned short packetDataLenght, unsigned char* outBufferData)
{
	(void)packetDataLenght;//UNUSED
	if(packetData[1]==DEBUG_READ_CELL_LIST)
  {
	  outBufferData[0]=DEBUG_ANS_OK;//Ответ опять не совпадает с запросом UP_CMD_DEBUG
	  outBufferData[1]=DEBUG_READ_CELL_LIST;

	  for(int i =0;i<debugCellsCount;i++)
	  {
	  	outBufferData[2+i]=debugCells[i].number;
	  }

	  return 2 + debugCellsCount;
  }
	else if(packetData[1]==DEBUG_READ_CELL_TYPE)//DEBUG_READ_CELL_TYPE
  {
		//Ищем ячейку с этим номером, если не нашли - отправляем что не поддерживается
		for(int i =0;i<debugCellsCount;i++)
		{
			 if(packetData[2]==debugCells[i].number)//Найдена ячейка с этим номером
			 {
				 outBufferData[0]=DEBUG_ANS_OK;//Ответ опять не совпадает с запросом UP_CMD_DEBUG;
				 outBufferData[1]=DEBUG_READ_CELL_TYPE;
				 outBufferData[2]=debugCells[i].number;/*Номер ячейки*/
				 outBufferData[3]=debugCells[i].format;
				 outBufferData[4]=debugCells[i].attribute;

				 uint16_t dataLen = 5 + sprintf((char*)&outBufferData[5],"%s",debugCells[i].name);

				 return dataLen;
			 }
		}

		return 0;//Не нашли => не поддерживается
  }
	else if(packetData[1]==DEBUG_READ_CELL)
	{
		//Ищем ячейку с этим номером, если не нашли - отправляем что не поддерживается
		for(int i =0;i<debugCellsCount;i++)
		{
			if(packetData[2]==debugCells[i].number)//Найдена ячейка с этим номером
			{
				outBufferData[0]=DEBUG_ANS_OK;//Ответ опять не совпадает с запросом UP_CMD_DEBUG;
				outBufferData[1]=DEBUG_READ_CELL;
				outBufferData[2]=debugCells[i].number;/*Номер ячейки*/
				outBufferData[3]=debugCells[i].format;

				uint16_t dataLen = GetDebugCellLenght(debugCells[i].format);

				preReadDebugCellCallback(debugCells[i].number);
				memcpy(&outBufferData[4],debugCells[i].data,dataLen);


				return 4+dataLen;
			}
	  }

		return 0;//Не нашли => не поддерживается
	}
	else if(packetData[1]==DEBUG_WRITE_CELL)//DEBUG_GET_CELLS_DESCRIPT_FILE
	{
		//Ищем ячейку с этим номером, если не нашли - отправляем что не поддерживается
		for(int i =0;i<debugCellsCount;i++)
		{
			if(packetData[2]==debugCells[i].number)//Найдена ячейка с этим номером
			{
				uint16_t dataLen = GetDebugCellLenght(debugCells[i].format);
				memcpy(debugCells[i].data, &packetData[3],dataLen);

				outBufferData[0]=DEBUG_ANS_OK;//Ответ - все хорошо
				outBufferData[1]=UP_ANS_OK;//Команда на которую выдается ответ
				outBufferData[2]=packetData[1];//Команда на которую выдается ответ

				writeDebugCellCallback(debugCells[i].number);

				return 3;
			}
		}

		return 0;//Не нашли => не поддерживается
	}
	/*else if(packetData[1]==DEBUG_GET_CELLS_DESCRIPT_FILE)
	{
		struct fs_file file;
		if(fs_open(&file,"RotaryGraundControlSystemCells.zip")!=ERR_OK)
		{
			outBufferData[0]=DEBUG_ANS_OK;//Ответ - все хорошо
			outBufferData[1]=DEBUG_GET_CELLS_DESCRIPT_FILE;//Команда на которую выдается ответ
			outBufferData[2]=0x00;//размер файла
			outBufferData[3]=0x00;//размер файла
			outBufferData[4]=0x00;//размер файла
			outBufferData[5]=0x00;//размер файла

			return 6;
		}

		outBufferData[0]=DEBUG_ANS_OK;//Ответ - все хорошо
		outBufferData[1]=DEBUG_GET_CELLS_DESCRIPT_FILE;//Команда на которую выдается ответ
		memcpy(&outBufferData[2],&file.len,4);//размер файла

		return 6;//Не нашли => не поддерживается
	}
	else if(packetData[1]==DEBUG_READ_CELLS_DESCRIPT_FILE)
	{
		struct fs_file file;
		int startReadAddress =0;
		memcpy(&startReadAddress,&packetData[2],4);

		if(fs_open(&file,"RotaryGraundControlSystemCells.zip")!=ERR_OK || startReadAddress >= file.len)return 0;

		outBufferData[0]=DEBUG_ANS_OK;//Ответ - все хорошо
		outBufferData[1]=DEBUG_READ_CELLS_DESCRIPT_FILE;//Команда на которую выдается ответ

		if(file.len - startReadAddress > 240)
		{
			memcpy(&outBufferData[2],&file.data[startReadAddress],240);
			return 240+2;
		}
		else
		{
			memcpy(&outBufferData[2],&file.data[startReadAddress],file.len - startReadAddress);
			return (file.len - startReadAddress)+2;
		}
	}*/

	return 0;
}


