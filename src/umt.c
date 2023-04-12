#include "umt.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UMT_DATA mUMT_DATA;

void read_file(char *arg)
{
	FILE *fil_in, *fil_in2;
	size_t size;
	uint32_t old_time = 0;
	fil_in = fopen((const char *)arg, (const char *)"rb");
	printf("Open file: %s\n", arg);

	size = fread(&mUMT_DATA.header, 1, sizeof(mUMT_DATA.header), fil_in);
	if (size == sizeof(mUMT_DATA.header))
		printf("Read Header OK\n");
	else
	{
		printf("Read Header Err\n");
		return;
	}
	printf("%s\n", mUMT_DATA.header.FileDescription);

	size = fread(&mUMT_DATA.data, numdata, sizeof(OMEGA_DATA_FILE_V1_1), fil_in);

	for (size_t i = 0; i < numdata; i++)
	{
		printf("%d) Time %d (%d)\n", i + 1, mUMT_DATA.data[i].Time, mUMT_DATA.data[i].Time - old_time);
		old_time = mUMT_DATA.data[i].Time;
	}

	fclose(fil_in);
}