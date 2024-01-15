#ifndef SPSK_H__
#define SPSK_H__

#include <stdint.h>

#pragma pack(1)
typedef struct
{
	uint16_t Ver;		// версия ПО датчика
	uint16_t Type;		// тип датчика
	uint32_t Num;		// номер датчика
	uint32_t FixNum;	// заводской номер датчика
	uint16_t PlaceId; // место установки датчика,
	int32_t Depth;		// глубина установки датчика, см
} SENSOR_FILE;

#pragma pack(1)
typedef struct
{
	uint32_t FileType;		  // тип файла
	char FileDescription[40]; // описание файла с нулевым терминатором
	uint8_t Year;				  // год
	uint8_t Month;				  // месяц
	uint8_t Day;				  // число
	uint8_t NotUsed;			  // нулевой байт
	uint16_t Shop;				  // номер цеха
	uint16_t Field;			  // код месторождения
	char Cluster[6];			  // номер куста
	char Well[6];				  // номер скважины

	SENSOR_FILE Sensor[2];

	uint16_t RegType; // тип регистратора
	uint16_t RegNum;	// заводской номер регистратора
	uint16_t RegVer;	// версия ПО регистратора
	uint16_t crc;
} SPSK_FILE_V1_2;

// Тип регистратора
#define SPSK_SIAM_1 0x0031 // Омега САМТ модель 1

#endif