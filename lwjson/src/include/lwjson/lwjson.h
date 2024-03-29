/**
 * \file            lwjson.h
 * \brief           LwJSON - Lightweight JSON format parser
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwJSON - Lightweight JSON format parser.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.5.0
 */
#ifndef LWJSON_HDR_H
#define LWJSON_HDR_H

#include <string.h>
#include <stdint.h>
#include "lwjson/lwjson_opt.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * \defgroup        LWJSON Lightweight JSON format parser
 * \brief           LwJSON - Lightweight JSON format parser
 * \{
 */

/**
 * \brief           Get size of statically allocated array
 * \param[in]       x: Object to get array size of
 * \return          Number of elements in array
 */
#define LWJSON_ARRAYSIZE(x) (sizeof(x) / sizeof((x)[0]))

	/**
	 * \brief           List of supported JSON types
	 */
	typedef enum
	{
		LWJSON_TYPE_STRING,	 /*!< String/Text format. Everything that has beginning and ending quote character */
		LWJSON_TYPE_NUM_INT,	 /*!< Number type for integer */
		LWJSON_TYPE_NUM_REAL, /*!< Number type for real number */
		LWJSON_TYPE_OBJECT,	 /*!< Object data type */
		LWJSON_TYPE_ARRAY,	 /*!< Array data type */
		LWJSON_TYPE_TRUE,		 /*!< True boolean value */
		LWJSON_TYPE_FALSE,	 /*!< False boolean value */
		LWJSON_TYPE_NULL,		 /*!< Null value */
	} lwjson_type_t;

	/**
	 * \brief           Real data type
	 */
	typedef LWJSON_CFG_REAL_TYPE lwjson_real_t;

	/**
	 * \brief           Integer data type
	 */
	typedef LWJSON_CFG_INT_TYPE lwjson_int_t;

	/**
	 * \brief           JSON token
	 */
	typedef struct lwjson_token
	{
		struct lwjson_token *next; /*!< Next token on a list */
		lwjson_type_t type;			/*!< Token type */
		const char *token_name;		/*!< Token name (if exists) */
		size_t token_name_len;		/*!< Length of token name (this is needed to support const input strings to parse) */
		union
		{
			struct
			{
				const char *token_value;		 /*!< Pointer to the beginning of the string */
				size_t token_value_len;			 /*!< Length of token value (this is needed to support const input strings to parse) */
			} str;									 /*!< String data */
			lwjson_real_t num_real;				 /*!< Real number format */
			lwjson_int_t num_int;				 /*!< Int number format */
			struct lwjson_token *first_child; /*!< First children object for object or array type */
		} u;											 /*!< Union with different data types */
	} lwjson_token_t;

	/**
	 * \brief           JSON result enumeration
	 */
	typedef enum
	{
		lwjsonOK = 0x00, /*!< Function returns successfully */
		lwjsonERR,		  /*!< Generic error message */
		lwjsonERRJSON,	  /*!< Error JSON format */
		lwjsonERRMEM,	  /*!< Memory error */
		lwjsonERRPAR,	  /*!< Parameter error */
	} lwjsonr_t;

	/**
	 * \brief           LwJSON instance
	 */
	typedef struct
	{
		lwjson_token_t *tokens;		 /*!< Pointer to array of tokens */
		size_t tokens_len;			 /*!< Size of all tokens */
		size_t next_free_token_pos; /*!< Position of next free token instance */
		lwjson_token_t first_token; /*!< First token on a list */
		struct
		{
			uint8_t parsed : 1; /*!< Flag indicating JSON parsing has finished successfully */
		} flags;					  /*!< List of flags */
	} lwjson_t;

	lwjsonr_t lwjson_init(lwjson_t *lw, lwjson_token_t *tokens, size_t tokens_len);
	lwjsonr_t lwjson_parse_ex(lwjson_t *lw, const void *json_data, size_t len);
	lwjsonr_t lwjson_parse(lwjson_t *lw, const char *json_str);
	const lwjson_token_t *lwjson_find(lwjson_t *lw, const char *path);
	const lwjson_token_t *lwjson_find_ex(lwjson_t *lw, const lwjson_token_t *token, const char *path);
	lwjsonr_t lwjson_free(lwjson_t *lw);

	void lwjson_print_token(const lwjson_token_t *token);
	void lwjson_print_json(const lwjson_t *lw);

/**
 * \brief           Get number of tokens used to parse JSON
 * \param[in]       lw: Pointer to LwJSON instance
 * \return          Number of tokens used to parse JSON
 */
#define lwjson_get_tokens_used(lw) (((lw) != NULL) ? ((lw)->next_free_token_pos + 1) : 0)

/**
 * \brief           Get very first token of LwJSON instance
 * \param[in]       lw: Pointer to LwJSON instance
 * \return          Pointer to first token
 */
#define lwjson_get_first_token(lw) (((lw) != NULL) ? (&(lw)->first_token) : NULL)

/**
 * \brief           Get token value for \ref LWJSON_TYPE_NUM_INT type
 * \param[in]       token: token with integer type
 * \return          Int number if type is integer, `0` otherwise
 */
#define lwjson_get_val_int(token) ((lwjson_int_t)(((token) != NULL && (token)->type == LWJSON_TYPE_NUM_INT) ? (token)->u.num_int : 0))

/**
 * \brief           Get token value for \ref LWJSON_TYPE_NUM_REAL type
 * \param[in]       token: token with real type
 * \return          Real numbeer if type is real, `0` otherwise
 */
#define lwjson_get_val_real(token) ((lwjson_real_t)(((token) != NULL && (token)->type == LWJSON_TYPE_NUM_REAL) ? (token)->u.num_real : 0))

/**
 * \brief           Get first child token for \ref LWJSON_TYPE_OBJECT or \ref LWJSON_TYPE_ARRAY types
 * \param[in]       token: token with integer type
 * \return          Pointer to first child or `NULL` if parent token is not object or array
 */
#define lwjson_get_first_child(token) (const void *)(((token) != NULL && ((token)->type == LWJSON_TYPE_OBJECT || (token)->type == LWJSON_TYPE_ARRAY)) ? (token)->u.first_child : NULL)

	/**
	 * \brief           Get string value from JSON token
	 * \param[in]       token: Token with string type
	 * \param[out]      str_len: Pointer to variable holding length of string.
	 *                      Set to `NULL` if not used
	 * \return          Pointer to string or `NULL` if invalid token type
	 */
	static inline const char *
	lwjson_get_val_string(const lwjson_token_t *token, size_t *str_len)
	{
		if (token != NULL && token->type == LWJSON_TYPE_STRING)
		{
			if (str_len != NULL)
			{
				*str_len = token->u.str.token_value_len;
			}
			return token->u.str.token_value;
		}
		return NULL;
	}

/**
 * \brief           Get length of string for \ref LWJSON_TYPE_STRING token type
 * \param[in]       token: token with string type
 * \return          Length of string in units of bytes
 */
#define lwjson_get_val_string_length(token) ((size_t)(((token) != NULL && (token)->type == LWJSON_TYPE_STRING) ? (token)->u.str.token_value_len : 0))

	/**
	 * \brief           Compare string token with user input string for a case-sensitive match
	 * \param[in]       token: Token with string type
	 * \param[in]       str: NULL-terminated string to compare
	 * \return          `1` if equal, `0` otherwise
	 */
	static inline uint8_t
	lwjson_string_compare(const lwjson_token_t *token, const char *str)
	{
		if (token != NULL && token->type == LWJSON_TYPE_STRING)
		{
			return strncmp(token->u.str.token_value, str, token->u.str.token_value_len) == 0;
		}
		return 0;
	}

	/**
	 * \brief           Compare string token with user input string for a case-sensitive match
	 * \param[in]       token: Token with string type
	 * \param[in]       str: NULL-terminated string to compare
	 * \param[in]       len: Length of the string in bytes
	 * \return          `1` if equal, `0` otherwise
	 */
	static inline uint8_t
	lwjson_string_compare_n(const lwjson_token_t *token, const char *str, size_t len)
	{
		if (token != NULL && token->type == LWJSON_TYPE_STRING && len <= token->u.str.token_value_len)
		{
			return strncmp(token->u.str.token_value, str, len) == 0;
		}
		return 0;
	}

	/**
	 * \brief           Copy the string token to a string, given the size of the token and the string
	 * \param[in]       token: Token with string type
	 * \param[in]       str: String pointer
	 * \param[in]       len: Length of the string in bytes
	 * \return          str size;
	 */
	static inline size_t
	lwjson_string_cpy_n(lwjson_token_t *token, uint8_t *str, size_t len)
	{
		if (token != NULL && token->type == LWJSON_TYPE_STRING)
		{
			if (len >= token->u.str.token_value_len)
			{
				memcpy((void *)str, (const void *)token->u.str.token_value, token->u.str.token_value_len);
				return token->u.str.token_value_len;
			}
			else
			{
				memcpy((void *)str, (const void *)token->u.str.token_value, len);
				return len;
			}
		}
		return 0;
	}

	/**
	 * \}
	 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWJSON_HDR_H */
