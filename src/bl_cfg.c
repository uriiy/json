#include "bl_cfg.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const SysUID gBLSysUID = {Default_HwId, Default_HeVersion, Default_SwID, Default_SwModel, Default_SwVersion};
SysUID *gBLSysUID_Addr = (SysUID *)&gBLSysUID;

BL_Storage dBL_Storage;
StorHeader new_hdr;

#define sBLCfg dBL_Storage.mBLParam.mBLConfig
#define sLockCfg dBL_Storage.mBLParam.mLockCfg

uint8_t BLCfg_SetDefault(void)
{
	sLockCfg.mLock = 0;
	sBLCfg.BootMode = bmApp;
	sBLCfg.ModbusAddress = Default_MbAddr;

	sBLCfg.Bitrate = Default_Bitrate;
	sBLCfg.Parity = Default_Parity;
	sBLCfg.StopBits = Default_StopBits;

	sBLCfg.DeviceNumber = 0;

	memcpy(&sBLCfg.BLUID, &gBLSysUID_Addr->mSwUID, sizeof(SwUID));

	return 0;
}

uint8_t BLCfg_Save(char *arg)
{
	FILE *fil_in, *fil_in2;

	BLCfg_SetDefault();

	fil_in2 = fopen((const char *)arg, (const char *)"rb");
	printf("Open file: %s\n", arg);

	unsigned int len, len2;
	unsigned char *source;

	fseek(fil_in2, 0, SEEK_END);
	len = ftell(fil_in2);
	fseek(fil_in2, 0, SEEK_SET);

	source = (unsigned char *)malloc(len);
	memset(source, 0, len);

	len2 = fread(source, 1, len, fil_in2);
	if (len2 != len)
	{
		printf("error read, %d\n", len2);
		return 0;
	}
	fclose(fil_in2);

	dBL_Storage.mBLParam.mBLConfig.AppSize = len;
	dBL_Storage.mBLParam.mBLConfig.AppCrc = usMBCRC16((uint8_t *)source, len);

	fil_in = fopen((const char *)Default_OutName, (const char *)"w");

	fwrite(&dBL_Storage, sizeof(BL_Storage), 1, fil_in);

	new_hdr.mGeneration = 1;
	new_hdr.mCRC = crc32_compute((uint8_t const *)&dBL_Storage, sizeof(BL_Storage), NULL);

	printf("LEN=%d, CRC=0x%x\n", dBL_Storage.mBLParam.mBLConfig.AppSize,
			 dBL_Storage.mBLParam.mBLConfig.AppCrc);

	fwrite(&new_hdr, sizeof(StorHeader), 1, fil_in);

	printf("Save BLCfg file: %s\n", Default_OutName);

	fclose(fil_in);
	return 1;
}

uint32_t crc32_compute(uint8_t const *p_data, uint32_t size, uint32_t const *p_crc)
{
	uint32_t crc;

	crc = (p_crc == NULL) ? 0xFFFFFFFF : ~(*p_crc);
	for (uint32_t i = 0; i < size; i++)
	{
		crc = crc ^ p_data[i];
		for (uint32_t j = 8; j > 0; j--)
		{
			crc = (crc >> 1) ^ (0xEDB88320U & ((crc & 1) ? 0xFFFFFFFF : 0));
		}
	}
	return ~crc;
}