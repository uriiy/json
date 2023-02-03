#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "bl_cfg.h"

static void gzip(char *arg);

static char name[] = "1_23-0.1-27.dat";
static uint8_t nsize = 0, tfsize = 0;

uint8_t find_char(char *name, uint8_t *ret_nsize, uint8_t *ret_tfsize)
{
	if (name == NULL ||
		 ret_nsize == NULL ||
		 ret_tfsize == NULL)
	{
		printf("error pointer");
		return 0;
	}

	*ret_nsize = strlen(name);

	do
	{
		if (*(char *)(name + *ret_nsize) == 0x2e)
			break;
	} while ((*ret_nsize)--);

	// ret_ftype = (char *)(name + (*ret_nsize) + 1);
	*ret_tfsize = strlen(name + (*ret_nsize) + 1);
	return 1;
}

int main(int argc, char *argv[])
{

	BLCfg_Save(argv[1]);

	printf("enter to exit\n");
	while (1)
	{
		if ('\n' == getc(stdin))
		{
			break;
		}
	}
}

#include "../uzlib/uzlib.h"

uint32_t bw;
FILE *fil;
FILE *fil_in;

#define FNAME_ENABLE 1

static void gzip(char *arg)
{
	const char *destination_name = "demo.gz";

	struct uzlib_comp comp = {0};
	unsigned long file_size = 0;
	unsigned int len;
	unsigned char *source;

	comp.dict_size = 32768;
	comp.hash_bits = 10;

	fil_in = fopen((const char *)arg, (const char *)"rb");

	fseek(fil_in, 0, SEEK_END);
	len = ftell(fil_in);
	fseek(fil_in, 0, SEEK_SET);

	source = (unsigned char *)malloc(len);
	memset(source, 0, len);
	if (fread(source, len, 1, fil_in) != len)
		printf("error read");

	fclose(fil_in);

	size_t hash_size = sizeof(uzlib_hash_entry_t) * (1 << comp.hash_bits);
	comp.hash_table = (uzlib_hash_entry_t *)malloc(hash_size);

	if (comp.hash_table == NULL)
	{
		printf("\n Memory allocation error \n");
		return;
	}

	memset(comp.hash_table, 0, hash_size);

	zlib_start_block(&comp);
	uzlib_compress(&comp, source, len);
	zlib_finish_block(&comp);

	printf("Data size %d", len);
	printf("\n Compressed data to : to %u  bytes \n", comp.outlen);

	fil = fopen(destination_name, (const char *)"wb");

	uint32_t mtime = 720489600; // 	Initial release GZIP 31 October 1992;

	fputc(0x1f, fil); //	Magic Number
	fputc(0x8b, fil); //	Magic Number
	fputc(0x08, fil); //	Compression Method
#ifdef FNAME_ENABLE
	fputc(0x08, fil); //	File Flags  FNAME : 0x08 ->	The file contains an original file name string
#else
	fputc(0x00, fil); //	File Flags
#endif
	fwrite(&mtime, sizeof(mtime), 1, fil);
	fputc(0x04, fil); //	XFL
	fputc(0x03, fil); //	OS

#ifdef FNAME_ENABLE
	// Create Payload
	const char *orginal_name = "demo.txt";
	fwrite(orginal_name, strlen(orginal_name), 1, fil);
	fputc(0x00, fil); //	EOF
#endif

	fwrite(comp.outbuf, comp.outlen, 1, fil);
	uint32_t crc = ~uzlib_crc32(source, len, ~0); // Calculate CRC

	fwrite(&crc, sizeof(crc), 1, fil);
	fwrite(&len, sizeof(len), 1, fil);

	fclose(fil);
	printf("\n Successfully generated!  %s ( %u bytes ) \n", destination_name, file_size);
}
