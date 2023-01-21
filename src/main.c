#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"

#include "lwjson/lwjson.h"

static void gzip(char *arg);

static lwjson_token_t token[200];
static lwjson_t lwjson;

position_t m_position;
devices_t m_devices[DEVICES];
param_dua_t m_param_dua[DEVICES];
param_ddim_t m_param_ddim[DEVICES];

TIME m_time;
INI_TIME_V1_0 m_ini_time;

FILE *f;

/**
 * @brief печать типа токена
 *
 * @param tkn указатель на токен
 */
void print_type(const lwjson_token_t *tkn)
{
	switch (tkn->type)
	{
	case LWJSON_TYPE_STRING: /*!< String/Text format. Everything that has beginning and ending quote character */
		printf("Type: String ");
		break;
	case LWJSON_TYPE_NUM_INT: /*!< Number type for integer */
		printf("Type: Int ");
		break;
	case LWJSON_TYPE_NUM_REAL: /*!< Number type for real number */
		printf("Type: Real ");
		break;
	case LWJSON_TYPE_OBJECT: /*!< Object data type */
		printf("Type: Object ");
		break;
	case LWJSON_TYPE_ARRAY: /*!< Array data type */
		printf("Array ");
		break;
	case LWJSON_TYPE_TRUE: /*!< True boolean value */
		printf("Bool true ");
		break;
	case LWJSON_TYPE_FALSE: /*!< False boolean value */
		printf("Bool false ");
		break;
	case LWJSON_TYPE_NULL: /*!< Null value */
		printf("Null ");
		break;
	}
}

/**
 * @brief печать значения токена
 *
 * @param tkn указатель на токен
 */
void print_value(const lwjson_token_t *tkn)
{
	switch (tkn->type)
	{
	case LWJSON_TYPE_STRING: /*!< String/Text format. Everything that has beginning and ending quote character */
		printf("Valeu: %.*s ", (int)tkn->u.str.token_value_len, tkn->u.str.token_value);
		break;
	case LWJSON_TYPE_NUM_INT: /*!< Number type for integer */
		printf("Value: %d ", tkn->u.num_int);
		break;
	case LWJSON_TYPE_NUM_REAL: /*!< Number type for real number */
		printf("Value: %0.2f ", tkn->u.num_real);
		break;
	default:
		break;
	}
}

/**
 * @brief поиск дочернего токена и присвоение его значения int
 *
 * @param json указатель на токен
 * @param path маска поиска
 * @param param указатель на переменную, куда присваивать найденное значение
 * @param def значения по умолчанию, если токен не найден
 * @return find_return_t (FIND_ERROR_NULL_PARAM, FIND_PARAM_TRUE, FIND_PARAM_DEFAULT, FIND_ERROR_TYPE)
 */
find_return_t find_param_int(lwjson_token_t *json, const char *path, uint32_t *param, uint32_t def)
{
	if (NULL == json && NULL == path && NULL == param)
		return FIND_ERROR_NULL_PARAM;

	lwjson_token_t *ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, path);
	if (NULL != ttk && LWJSON_TYPE_NUM_INT == ttk->type)
	{
		*param = ttk->u.num_int;
		return FIND_PARAM_TRUE;
	}
	else
	{
		*param = def;
		return FIND_PARAM_DEFAULT;
	}
}

/**
 * @brief поиск дочернего токена и присвоение его значения float
 *
 * @param json указатель на токен
 * @param path маска поиска
 * @param param указатель на переменную, куда присваивать найденное значение
 * @param def значения по умолчанию, если токен не найден
 * @return find_return_t (FIND_ERROR_NULL_PARAM, FIND_PARAM_TRUE, FIND_PARAM_DEFAULT, FIND_ERROR_TYPE)
 */
find_return_t find_param_float(lwjson_token_t *json, const char *path, uint32_t *param, uint32_t def)
{
	if (NULL == json && NULL == path && NULL == param)
		return FIND_ERROR_NULL_PARAM;

	lwjson_token_t *ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, path);
	if (NULL != ttk && LWJSON_TYPE_NUM_REAL == ttk->type)
	{
		*param = ttk->u.num_real;
		return FIND_PARAM_TRUE;
	}
	else
	{
		*param = def;
		return FIND_PARAM_DEFAULT;
	}
}

/**
 * @brief поиск дочернего токена и присвоение его значения string
 *
 * @param json указатель на токен
 * @param path маска поиска
 * @param param указатель на строку, куда присваивать найденное значение
 * @param len_param размер строки
 * @param def значение по умолчанию, если токен не найден
 * @param len_def размер значения по умолчанию
 * @return find_return_t (FIND_ERROR_NULL_PARAM, FIND_PARAM_TRUE, FIND_PARAM_DEFAULT, FIND_ERROR_TYPE)
 */
find_return_t find_param_string(lwjson_token_t *json, const char *path, uint32_t *param, size_t len_param, uint32_t *def, size_t len_def)
{
	if (NULL == json && NULL == path && NULL == param)
		return FIND_ERROR_NULL_PARAM;

	lwjson_token_t *ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, path);
	if (NULL != ttk && LWJSON_TYPE_STRING == ttk->type)
	{
		lwjson_string_cpy_n(ttk, (uint8_t *)param, len_param);
		return FIND_PARAM_TRUE;
	}
	else
	{
		memcpy(param, def, len_def);
		return FIND_PARAM_DEFAULT;
	}
}

/**
 * @brief поиск дочернего токена и присвоение его значения по битовой маске
 *
 * @param json указатель на токен
 * @param path маска поиска
 * @param param указатель на переменную, куда присваивать найденное значение
 * @param bit_mask битовая маска
 * @return find_return_t (FIND_ERROR_NULL_PARAM, FIND_PARAM_TRUE, FIND_PARAM_DEFAULT, FIND_ERROR_TYPE)
 */
find_return_t find_param_bool(lwjson_token_t *json, const char *path, uint32_t *param, uint32_t bit_mask)
{
	if (NULL == json && NULL == path && NULL == param)
		return FIND_ERROR_NULL_PARAM;

	lwjson_token_t *ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, path);
	if (NULL == ttk)
	{
		switch (ttk->type)
		{
		case LWJSON_TYPE_TRUE:
			*param |= bit_mask;
			return FIND_PARAM_TRUE;
		case LWJSON_TYPE_FALSE:
			*param &= ~bit_mask;
			return FIND_PARAM_TRUE;
		default:
			*param &= ~bit_mask;
			return FIND_ERROR_TYPE;
		}
	}
	else
	{
		*param &= ~bit_mask;
		return FIND_ERROR_NULL_PARAM;
	}
}

/**
 * @brief парсинг положение модема
 *
 * @param json указатель на токен
 */
void get_position(lwjson_token_t *json)
{
	lwjson_token_t *ttk;

	if (NULL == json)
		return;

	find_param_int(json, "shop", (uint32_t *)&m_position.shop, SHOP_DEFAULT);
	find_param_int(json, "operator", (uint32_t *)&m_position.oper, OPERATOR_DEFAULT);
	find_param_int(json, "field", (uint32_t *)&m_position.field, FIELD_DEFAULT);
	find_param_string(json, "cluster", (uint32_t *)&m_position.cluster, sizeof(m_position.cluster),
							(uint32_t *)CLUSTER_DEFAULT, sizeof(CLUSTER_DEFAULT));

#ifdef _DEBUG
	printf("Print Position struct:\n");
	printf("#-SHOP: %d\n", m_position.shop);
	printf("#-OPERATOR: %d\n", m_position.oper);
	printf("#-FIELD: %d\n", m_position.field);
	printf("#-CLUSTER: %.*s\n", strlen((const char *)m_position.cluster), m_position.cluster);
	printf("\n");
#endif
}

/**
 * @brief парсинг параметры работы динамографа
 *
 * @param json указатель на токен
 * @param i номер датчика
 */
void get_param_ddim(lwjson_token_t *json, uint8_t i)
{
	if (NULL == json)
		return;
	lwjson_token_t *ttk;

	find_param_int(json, "period", (uint32_t *)&m_param_ddim[i].dyn_period, DYN_PERIOD_DEFAULT);
	find_param_int(json, "apertur", (uint32_t *)&m_param_ddim[i].dyn_aperture, DYN_APERTURE_DEFAULT);
	find_param_int(json, "travel", (uint32_t *)&m_param_ddim[i].dyn_travel, DYN_TRAVEL_DEFAULT);
	find_param_int(json, "pump", (uint32_t *)&m_param_ddim[i].dyn_pu, DYN_PU_DEFAULT);
	find_param_int(json, "rod", (uint32_t *)&m_param_ddim[i].dyn_rod, DYN_ROD_DEFAULT);
	m_devices[i].param_device = (uint32_t *)&m_param_ddim[i];

//*********************************************************************
#ifdef _DEBUG
	printf("#-PARAM DEVICES:\n");
	printf("#--PERIOD: %d\n", m_param_ddim[i].dyn_period);
	printf("#--APERTUR: %d\n", m_param_ddim[i].dyn_aperture);
	printf("#--TRAVEL: %d\n", m_param_ddim[i].dyn_travel);
	printf("#--PUMP: %d\n", m_param_ddim[i].dyn_pu);
	printf("#--ROD: %d\n", m_param_ddim[i].dyn_rod);
#endif
}

/**
 * @brief парсинг параметры работы уровнемера
 *
 * @param json указатель на токен
 * @param i номер датчика
 */
void get_param_dua(lwjson_token_t *json, uint8_t i)
{
	if (NULL == json)
		return;

	lwjson_token_t *ttk;

	find_param_int(json, "echotype", (uint32_t *)&m_param_dua[i].echotype, STATIC_ECHO_RESEARCH_TYPE);

	find_param_bool(json, "auto", (uint32_t *)&m_param_dua[i].echomode, AUTO);
	find_param_bool(json, "gasout", (uint32_t *)&m_param_dua[i].echomode, INLET);
	find_param_bool(json, "addgain", (uint32_t *)&m_param_dua[i].echomode, GAIN);
	find_param_bool(json, "maxlevel", (uint32_t *)&m_param_dua[i].echomode, LEVEL6000);
	find_param_bool(json, "durauto", (uint32_t *)&m_param_dua[i].echomode, DUR_AUTO);

	find_param_int(json, "table", (uint32_t *)&m_param_dua[i].table, DUA_TABLE_DEFAULT);
	find_param_int(json, "speed", (uint32_t *)&m_param_dua[i].speed, DUA_SPEED_DEFAULT);

#ifdef _DEBUG
	printf("#-PARAM DEVICES:\n");
	printf("#--ECHOTYPE: %d\n", m_param_dua[i].echotype);
	printf("#--ECHOMODE: %d\n", m_param_dua[i].echomode);
	printf("#--TABLE: %d\n", m_param_dua[i].table);
	printf("#--SPEED: %d\n", m_param_dua[i].speed);
#endif
}

/**
 * @brief список датчиков
 *
 * @param json указатель на токен
 */
void get_type_device(lwjson_token_t *json)
{
	lwjson_token_t *ttk;

	if (NULL == json)
		return;

	lwjson_token_t *cild_token = (lwjson_token_t *)lwjson_get_first_child(json);

	for (uint8_t i; i < DEVICES; i++, cild_token = cild_token->next)
	{
		if (NULL == cild_token)
			return;

		find_param_string(cild_token, "name", (uint32_t *)&m_devices[i].name, sizeof(m_devices->name), (uint32_t *)NAME_DEFAULT, sizeof(NAME_DEFAULT));
		find_param_int(cild_token, "num", (uint32_t *)&m_devices[i].num, NUM_DEFAULT);

#ifdef _DEBUG
		printf("Print Devices %d:\n", i + 1);
		printf("#-NAME: %.*s \n", strlen((const char *)m_devices[i].name), m_devices[i].name);
		printf("#-NUM: %04d\n", m_devices[i].num);
#endif

		ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, cild_token, "mac");
		if (NULL != ttk)
		{
			if (17 <= ttk->u.str.token_value_len)
			{
				for (uint8_t k = 0; k < MAC_CHARS; k++)
				{
					m_devices[i].mac_addr[k] = strtol((const char *)(ttk->u.str.token_value + k * 3), NULL, 16);
				}
#ifdef _DEBUG
				printf("#-MAC: %X:%X:%X:%X:%X:%X\n",
						 m_devices[i].mac_addr[0],
						 m_devices[i].mac_addr[1],
						 m_devices[i].mac_addr[2],
						 m_devices[i].mac_addr[3],
						 m_devices[i].mac_addr[4],
						 m_devices[i].mac_addr[5]);
#endif
			}
			else
			{
				memcpy(m_devices[i].mac_addr, MAC_DEFAULT, sizeof(MAC_DEFAULT));
#ifdef _DEBUG
				printf("#-MAC: err format\n");
#endif
			}
		}
		else
			memcpy(m_devices[i].mac_addr, MAC_DEFAULT, sizeof(MAC_DEFAULT));

		find_param_string(cild_token, "well", (uint32_t *)&m_devices[i].well, sizeof(m_devices->well),
								(uint32_t *)WELL_DEFAULT, sizeof(WELL_DEFAULT));
		find_param_int(cild_token, "holepress", (uint32_t *)&m_devices[i].hole_press, HOLE_PRESSURE_DEFAULT);
		find_param_int(cild_token, "bufpress", (uint32_t *)&m_devices[i].buf_press, BUF_PRESSURE_DEFAULT);
		find_param_int(cild_token, "linepress", (uint32_t *)&m_devices[i].line_press, LINE_PRESSURE_DEFAULT);
		find_param_int(cild_token, "ontime", (uint32_t *)&m_devices[i].ontime, ONTIME_DEFAULT);

#ifdef _DEBUG
		printf("#-WELL: %.*s \n", strlen((const char *)m_devices[i].well), m_devices[i].well);
		printf("#-HOLE_PRESSURE: %d \n", m_devices[i].hole_press);
		printf("#-BUF_PRESSURE: %d \n", m_devices[i].buf_press);
		printf("#-LINE_PRESSURE: %d \n", m_devices[i].line_press);
		printf("#-ON_TIME: %d \n", m_devices[i].ontime);
#endif

		ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, cild_token, "param");
		if (NULL != ttk)
		{
			if (0 == memcmp(m_devices[i].name, "dua", 3))
				get_param_dua(ttk, i);
			if (0 == memcmp(m_devices[i].name, "ddim", 4))
				get_param_ddim(ttk, i);
		}

//*********************************************************************
#ifdef _DEBUG
		printf("\n");
#endif
	}
}

void file_tamplate(void)
{
	f = fopen((const char *)FILE_JSON, (const char *)"w");

	fprintf(f, "{\n");
	fprintf(f, "	\"position\": {\n");
	fprintf(f, "		\"shop\": %d,\n", SHOP_DEFAULT);
	fprintf(f, "		\"operator\": %d,\n", OPERATOR_DEFAULT);
	fprintf(f, "		\"field\": %d,\n", FIELD_DEFAULT);
	fprintf(f, "		\"cluster\": \"%s\"\n", CLUSTER_DEFAULT);
	fprintf(f, "	},\n");
	fprintf(f, "	\"devicelist\": [\n");
	fprintf(f, "		{\n");
	fprintf(f, "			\"name\": \"ddim\",\n");
	fprintf(f, "			\"num\": %d,\n", NUM_DEFAULT);
	fprintf(f, "			\"mac\": \"%s\",\n", MAC_DEFAULT);
	fprintf(f, "			\"well\": \"%s\",\n", WELL_DEFAULT);
	fprintf(f, "			\"ontime\": %d,\n", ONTIME_DEFAULT);
	fprintf(f, "			\"holepress\": %d,\n", HOLE_PRESSURE_DEFAULT);
	fprintf(f, "			\"bufpress\": %d,\n", BUF_PRESSURE_DEFAULT);
	fprintf(f, "			\"linepress\": %d,\n", LINE_PRESSURE_DEFAULT);
	fprintf(f, "			\"param\": {\n");
	fprintf(f, "				\"period\": %d,\n", DYN_PERIOD_DEFAULT);
	fprintf(f, "				\"apertur\": %d,\n", DYN_APERTURE_DEFAULT);
	fprintf(f, "				\"travel\": %d,\n", DYN_TRAVEL_DEFAULT);
	fprintf(f, "				\"pump\": %d,\n", DYN_PU_DEFAULT);
	fprintf(f, "				\"rod\": %d\n", DYN_ROD_DEFAULT);
	fprintf(f, "			}\n");
	fprintf(f, "		},\n");
	fprintf(f, "		{\n");
	fprintf(f, "			\"name\": \"dua\",\n");
	fprintf(f, "			\"num\": %d,\n", NUM_DEFAULT);
	fprintf(f, "			\"mac\": \"%s\",\n", MAC_DEFAULT);
	fprintf(f, "			\"well\": \"%s\",\n", WELL_DEFAULT);
	fprintf(f, "			\"ontime\": %d,\n", ONTIME_DEFAULT);
	fprintf(f, "			\"holepress\": %d,\n", HOLE_PRESSURE_DEFAULT);
	fprintf(f, "			\"bufpress\": %d,\n", BUF_PRESSURE_DEFAULT);
	fprintf(f, "			\"linepress\": %d,\n", LINE_PRESSURE_DEFAULT);
	fprintf(f, "			\"param\": {\n");
	fprintf(f, "				\"echotype\": %d,\n", STATIC_ECHO_RESEARCH_TYPE);
	fprintf(f, "				\"auto\": true,\n");
	fprintf(f, "				\"gasout\": false,\n");
	fprintf(f, "				\"addgain\": false,\n");
	fprintf(f, "				\"maxlevel\": false,\n");
	fprintf(f, "				\"durauto\": false,\n");
	fprintf(f, "				\"table\": %d,\n", DUA_TABLE_DEFAULT);
	fprintf(f, "				\"speed\": %d\n", DUA_SPEED_DEFAULT);
	fprintf(f, "			}\n");
	fprintf(f, "		}\n");
	fprintf(f, "	]\n");
	fprintf(f, "}");
	fclose(f);
}

static uint8_t IsLeapYear(uint8_t year)
{
	return (((year % 4 == 0) && (year % 100 != 0)) /* || (year % 400 == 0)*/) ? 1 : 0;
}
void Time_Correct(TIME *pTime)
{
	if (pTime->month == 0)
		pTime->month = 1;
	else
	{
		if (pTime->month > 12)
			pTime->month = 12;
	}

	if (pTime->day == 0)
		pTime->day = 1;
	else
	{
		int ThisMonthDays = MonthDays[pTime->month - 1];
		if (pTime->month == 2 && IsLeapYear(pTime->year))
			ThisMonthDays += 1; // фефраль в високосный год
		if (pTime->day > ThisMonthDays)
			pTime->day = ThisMonthDays;
	}

	if (pTime->hour > 23)
		pTime->hour = 23;
	if (pTime->min > 59)
		pTime->min = 59;
	if (pTime->sec > 59)
		pTime->sec = 59;
}

static uint32_t CountRTC(TIME *pTime)
{
	uint32_t days = pTime->day - 1 + (pTime->year - 1) * 365 + (pTime->year - 1) / 4 - (pTime->year - 1) / 100; // + (pTime->year - 1)/400;
	for (int month = 1; month < pTime->month; month++)
	{
		days += MonthDays[month - 1];
		if (month == 2 && IsLeapYear(pTime->year))
			days += 1;
	}
	return days * 86400 + pTime->hour * 3600 + pTime->min * 60 + pTime->sec;
}

int main(int argc, char *argv[])
{
	// if ((f = fopen((const char *)FILE_JSON, (const char *)"rb")) == NULL)
	// {
	// 	printf("Cannot open file.\n");
	// 	file_tamplate();
	// }
	// if ((f = fopen((const char *)FILE_JSON, (const char *)"rb")) == NULL)
	// {
	// 	printf("Cannot open file.\n");
	// 	exit(1);
	// }

	// getc(f);
	// lwjson_init(&lwjson, token, LWJSON_ARRAYSIZE(token));

	// if (lwjson_parse(&lwjson, (const char *)f->_base) == lwjsonOK)
	// {
	// 	lwjson_token_t *t;
	// 	printf("JSON parsed..\n\n");

	// 	t = (lwjson_token_t *)lwjson_find(&lwjson, "position");
	// 	get_position(t);
	// 	t = (lwjson_token_t *)lwjson_find(&lwjson, "devicelist");
	// 	get_type_device(t);

	// 	lwjson_free(&lwjson);
	// }
	// fclose(f);

	// struct tm local_time, utc_time;

	// time_t t = time(NULL);
	// local_time = *localtime(&t);
	// utc_time = *gmtime(&t);

	// m_time.day = local_time.tm_mday;
	// m_time.hour = local_time.tm_hour;
	// m_time.min = local_time.tm_min;
	// m_time.month = local_time.tm_mon + 1;
	// m_time.sec = local_time.tm_sec;
	// m_time.year = local_time.tm_year - 100;

	// Time_Correct(&m_time);

	// m_ini_time.Ver = 0x0100;
	// m_ini_time.Time = CountRTC(&m_time);
	// m_ini_time.TimeZone = local_time.tm_hour - utc_time.tm_hour;

	// m_ini_time.crc = usMBCRC16((uint8_t *)&m_ini_time, sizeof(m_ini_time) - sizeof(m_ini_time.crc));

	// f = fopen((const char *)"initime", (const char *)"w");
	// fwrite((const void *)&m_ini_time, sizeof(m_ini_time), 1, f);
	// fclose(f);

	// printf("Time: %s\n", ctime(&t));

	gzip(argv[1]);

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
