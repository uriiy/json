#include "umt.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spsk.h"

UMT_DATA mUMT_DATA;
SPSK_FILE_V1_2 mSPSK_DATA;

void read_file(char *arg)
{
	FILE *fil_in, *fil_in2;
	size_t size;
	uint16_t crc = 0;
	uint32_t old_time = 0;
	fil_in = fopen((const char *)arg, (const char *)"r+");
	printf("Open file: %s\n", arg);

	size = fread(&mSPSK_DATA, 1, sizeof(mSPSK_DATA), fil_in);
	if (size == sizeof(mSPSK_DATA))
		printf("Read Header OK\n");
	else
	{
		printf("Read Header Err\n");
		return;
	}

	printf("Enter sensor number: ");
	scanf("%d", &mSPSK_DATA.Sensor[0].FixNum);

	uint32_t offset = offsetof(SPSK_FILE_V1_2, Sensor[0].FixNum);
	fseek(fil_in, offset, 0);

	size = fwrite(&mSPSK_DATA.Sensor[0].FixNum, 1, sizeof(uint32_t), fil_in);
	if (size == sizeof(uint32_t))
		printf("Write Header OK\n");
	else
	{
		printf("Write Header Err\n");
		return;
	}

	fclose(fil_in);
}