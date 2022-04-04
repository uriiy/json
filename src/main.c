#include <stdio.h>
#include <stdlib.h>
#include "main.h"

#include "lwjson/lwjson.h"

static lwjson_token_t token[50];
static lwjson_t lwjson;

position_t m_position;
devices_t m_devices[DEVICES];

FILE *f;

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

void get_position(lwjson_token_t *json)
{
	lwjson_token_t *ttk;

	if (json == NULL)
		return;

#ifdef _DEBUG
	printf("Print Position struct:\n");
#endif

	ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, "shop");
	if (ttk != NULL)
		m_position.Shop = ttk->u.num_int;
	else
		m_position.Shop = Shop_default;
#ifdef _DEBUG
	printf("#  SHOP: %d\n", m_position.Shop);
#endif

	ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, "operator");
	if (ttk != NULL)
		m_position.Operator = ttk->u.num_int;
	else
		m_position.Operator = Operator_default;
#ifdef _DEBUG
	printf("#  OPERATOR: %d\n", m_position.Operator);
#endif

	ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, "field");
	if (ttk != NULL)
		m_position.Field = ttk->u.num_int;
	else
		m_position.Field = Field_default;
#ifdef _DEBUG
	printf("#  FIELD: %d\n", m_position.Field);
#endif

	ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, "cluster");
	if (ttk != NULL)
		lwjson_string_cpy_n(ttk, m_position.Cluster, sizeof(m_position.Cluster));
	else
		memcpy(m_position.Cluster, Cluster_default, sizeof(Cluster_default));
#ifdef _DEBUG
	printf("#  CLUSTER: %.*s\n", strlen((const char *)m_position.Cluster), m_position.Cluster);
#endif
#ifdef _DEBUG
	printf("\n");
#endif
}

void get_type_device(lwjson_token_t *json)
{
	lwjson_token_t *ttk;

	if (json == NULL)
		return;

	lwjson_token_t *cild_token = (lwjson_token_t *)lwjson_get_first_child(json);

	for (uint8_t i; i < DEVICES; i++, cild_token = cild_token->next)
	{
		if (cild_token == NULL)
			return;
#ifdef _DEBUG
		printf("Print Devices %d:\n", i + 1);
#endif

		ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, cild_token, "name");
		if (ttk != NULL)
			lwjson_string_cpy_n(ttk, m_devices[i].Name, NAME_CHARS);
		else
			memcpy(m_devices[i].Name, NAME_default, sizeof(NAME_default));
#ifdef _DEBUG
		printf("#  NAME: %.*s \n", strlen((const char *)m_devices[i].Name), m_devices[i].Name);
#endif

		ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, cild_token, "num");
		if (ttk != NULL)
			m_devices[i].Num = ttk->u.num_int;
		else
			m_devices[i].Num = NUM_default;
#ifdef _DEBUG
		printf("#  NUM: %04d\n", m_devices[i].Num);
#endif

		ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, cild_token, "mac");
		if (ttk != NULL)
		{
			if (17 <= ttk->u.str.token_value_len)
			{
				for (uint8_t k = 0; k < MAC_CHARS; k++)
				{
					m_devices[i].mac_addr[k] = strtol((const char *)(ttk->u.str.token_value + k * 3), NULL, 16);
				}
#ifdef _DEBUG
				printf("#  MAC: %X:%X:%X:%X:%X:%X\n",
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
				memset(m_devices[i].mac_addr, 0, sizeof(m_devices[i].mac_addr));
#ifdef _DEBUG
				printf("#  MAC: err format\n");
#endif
			}
		}

		ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, cild_token, "well");
		if (ttk != NULL)
		{
			lwjson_string_cpy_n(ttk, m_devices[i].Well, sizeof(m_devices[i].Well));
		}
		else
		{
			memset(m_devices[i].Well, 0, sizeof(m_devices[i].Well));
		}
#ifdef _DEBUG
		printf("#  WELL: %.*s \n", strlen((const char *)m_devices[i].Well), m_devices[i].Well);
#endif
// 		ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, cild_token, "type");
// 		if (ttk != NULL)
// 		{
// 			if (6 <= ttk->u.str.token_value_len)
// 				m_devices[i].Type = strtol(ttk->u.str.token_value, NULL, 0);
// 			else
// 			{
// 				m_devices[i].Type = 0;
// #ifdef _DEBUG
// 				printf("#  TYPE: err format\n");
// #endif
// 			}
// 		}
// 		else
// 			m_devices[i].Type = 0;
// #ifdef _DEBUG
// 		printf("#  TYPE %04X", m_devices[i].Type);
// #endif

//*********************************************************************
#ifdef _DEBUG
		printf("\n");
#endif
	}
}

int main(void)
{
	if ((f = fopen((const char *)"C:/dev/projects/test/1.json", (const char *)"rb")) == NULL)
	{
		printf("Cannot open file.\n");
		exit(1);
	}

	getc(f);
	lwjson_init(&lwjson, token, LWJSON_ARRAYSIZE(token));

	if (lwjson_parse(&lwjson, (const char *)f->_base) == lwjsonOK)
	{
		lwjson_token_t *t;
		printf("JSON parsed..\n\n");

		t = (lwjson_token_t *)lwjson_find(&lwjson, "position");
		get_position(t);
		t = (lwjson_token_t *)lwjson_find(&lwjson, "devicelist");
		get_type_device(t);

		lwjson_free(&lwjson);
	}
	fclose(f);
}