#ifndef UMT_H__
#define UMT_H__

#include <stdint.h>

typedef struct
{
	uint16_t Shop;
	uint16_t Field;
	uint8_t Cluster[6];
	uint8_t Well[6];
	uint16_t PlaceId;
	int32_t Depth;
} __attribute__((aligned(2), packed)) FILES_RESEARCH_ID_V1_0;

typedef struct
{
	uint32_t FileType;
	char FileDescription[40];
	uint8_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t NotUsed;
	FILES_RESEARCH_ID_V1_0 id;
	uint16_t RegType;
	uint16_t RegNum;
	uint16_t RegVer;
	uint16_t SensType;
	uint32_t SensNum;
	uint16_t SensVer;
	uint16_t crc;
} __attribute__((aligned(2), packed)) SPSK_FILE_V1_1;

typedef struct
{
	uint32_t Time; // время измерения от начала дня, мс
	int32_t Press; // давление, 0.001 атм
	int32_t Temp;	// температура, 0.001 °С
	uint16_t Vbat; // напряжение батареи,
	uint16_t crc;	// CRC16
} __attribute__((aligned(2), packed)) OMEGA_DATA_FILE_V1_1;

#define numdata 120
typedef struct
{
	SPSK_FILE_V1_1 header;
	OMEGA_DATA_FILE_V1_1 data[numdata];
} __attribute__((aligned(2), packed)) UMT_DATA;

void read_file(char *arg);

#endif