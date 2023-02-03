#ifndef BL_CONFIG_H__
#define BL_CONFIG_H__

#include <stdint.h>

#define Default_OutName "bl_cfg(mem_add=0xFE000).bin"

#define Default_HwId 0x0001
#define Default_HeVersion 0x0002
#define Default_SwID 0x0000
#define Default_SwModel 0x0003
#define Default_SwVersion 0x0003

#define Default_MbAddr 127
#define Default_IsEndPoint 1
#define Default_Protocol 1

#define Default_Bitrate 1152
#define Default_Parity 0
#define Default_StopBits 1

typedef struct
{
	uint16_t HwID;
	uint16_t HwVersion;
} __attribute__((aligned(2), packed)) HwUID;
typedef struct
{
	uint16_t SwID;
	uint16_t SwModel;
	uint16_t SwVersion;
} __attribute__((aligned(2), packed)) SwUID;
typedef struct
{
	HwUID mHwUID;
	SwUID mSwUID;
} __attribute__((aligned(2), packed)) SysUID;

typedef struct
{
	SwUID BLUID;
	SwUID AppUID;
	uint32_t BlackBoxAddress;
	uint32_t BlackBoxSize;
	uint32_t DeviceNumber;
	uint16_t ModbusAddress;
	uint16_t Bitrate;
	uint8_t Parity;
	uint8_t StopBits;
	uint8_t Reserved[12];
	uint16_t BootMode;
	uint32_t AppSize;
	uint32_t AppCrc;
} __attribute__((aligned(2), packed)) BLConfig;

typedef struct
{
	uint32_t mPasswdCRC;
	uint16_t mLock;
} __attribute__((aligned(2), packed)) LockCfg;

typedef struct
{
	BLConfig mBLConfig;
	LockCfg mLockCfg;
} __attribute__((aligned(4), packed)) BLParam;

typedef struct
{
	BLParam mBLParam;
	uint8_t mReserved[256 - sizeof(uint32_t) // version confit in Flash
							- sizeof(uint32_t)	  // crc
							- sizeof(BLParam)];
} __attribute__((aligned(4), packed)) BL_Storage;

typedef enum __attribute__((aligned(2)))
{
	bmApp = 0U,
	bmLoader = 1U
} BootMode;

typedef struct
{
	uint32_t mGeneration;
	uint32_t mCRC;
} __attribute__((aligned(4), packed)) StorHeader;

extern BL_Storage dBL_Storage;
extern StorHeader new_hdr;

uint8_t BLCfg_SetDefault(void);
uint8_t BLCfg_Save(char *arg);

uint32_t crc32_compute(uint8_t const *p_data, uint32_t size, uint32_t const *p_crc);

#endif