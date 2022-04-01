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

	ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, "shop");
	if (ttk != NULL)
		m_position.Shop = ttk->u.num_int;
	else
		m_position.Shop = Shop_default;

	ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, "operator");
	if (ttk != NULL)
		m_position.Operator = ttk->u.num_int;
	else
		m_position.Operator = Operator_default;

	ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, "field");
	if (ttk != NULL)
		m_position.Field = ttk->u.num_int;
	else
		m_position.Field = Field_default;

	ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, json, "cluster");
	if (ttk != NULL)
		lwjson_string_cpy_n(ttk, m_position.Cluster, sizeof(m_position.Cluster));
	else
		memcpy(m_position.Cluster, Cluster_default, sizeof(Cluster_default));
}

void get_type_device(lwjson_token_t *json)
{
	lwjson_token_t *ttk;

	lwjson_token_t *cild_token = (lwjson_token_t *)lwjson_get_first_child(json);

	for (uint8_t i; i < DEVICES; i++, cild_token = cild_token->next)
	{
		ttk = (lwjson_token_t *)lwjson_find_ex(&lwjson, cild_token, "name");
		if (ttk == NULL)
			return;
		printf("NAME: %.*s \n", (int)ttk->u.str.token_value_len, ttk->u.str.token_value);
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