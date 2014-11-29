
/**
* 	libserc - Serc data serialization language reference library
*    ==============================================================
*    Copyright (C) 2014 Gregory Ralph Martin
*    info at softsprocket dot com
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Lesser General Public
*    License as published by the Free Software Foundation; either
*    version 2.1 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public
*    License along with this library; if not, write to the Free Software
*    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
*    USA
*/

#include "serc.h"
#include "serc_parse.h"
#include "serc_lexer.h"
#include <softsprocket/container.h>
#include <softsprocket/debug_utils.h>


hash_table* serc_parse_buffer (char* buffer, size_t buffer_size) {

	yyscan_t scanner;
	yylex_init (&scanner);
	
	YY_BUFFER_STATE state = yy_scan_buffer (buffer, buffer_size, scanner);

	if (state == NULL) {
		PMSG ("yy_scan_buffer failed\n");
		return NULL;
	}

	hash_table* results;
	if (yyparse (&results, scanner) != 0) {
		PMSG ("Parse failed - exiting\n");
		exit (EXIT_FAILURE);
	}

	yy_delete_buffer (state, scanner);

	yylex_destroy (scanner);

	return results;
}


char* serc_read_file (char* fname, size_t* buffer_size) {

	char* read_buf = NULL;

	FILE* f = fopen (fname, "r");
	if (f == NULL) {
		PERR ("fopen");
		return NULL;
	}

	size_t current_size = 1024;
	size_t copied_bytes = 0;
	read_buf = malloc (current_size);
	if (read_buf == NULL) {
		PERR ("malloc");
		fclose (f);
		return NULL;
	}

	char str[256];
	size_t rv = 0;
	while ((rv = fread (str, 1, 256, f)) != 0) {
		if (ferror (f)) {
			PERR ("fread");
			free (read_buf);
			fclose (f);
			return NULL;
		}

		if ((copied_bytes + rv) > current_size) {
			current_size *= 2;
			char* tmp = realloc (read_buf, current_size);
			if (tmp == NULL) {
				PERR ("realloc");
				free (read_buf);
				fclose (f);
				return NULL;
			}

			read_buf = tmp;
		}

		memcpy (read_buf + copied_bytes, str, rv);
		copied_bytes += rv;

		if (rv < 256) {
			break;
		}
	}

	if (ferror (f)) {
		PERR ("fread");
		free (read_buf);
		fclose (f);
		return NULL;
	}

	fclose (f);

	if ((copied_bytes + 2) > current_size) {
		current_size += 2;
		char* tmp = realloc (read_buf, current_size);
		if (tmp == NULL) {
			PERR ("realloc");
			free (read_buf);
			fclose (f);
			return NULL;
		}

		read_buf = tmp;
	}

	read_buf[copied_bytes] = '\0';
	++copied_bytes;
	read_buf[copied_bytes] = '\0';
	++copied_bytes;

	*buffer_size = copied_bytes;
	return read_buf;
}

char* parseable_buffer_from_string (char* str, size_t* buffer_size) {
	int len = strlen (str) + 2;
	char* buf = malloc (len);
	if (buf == NULL) {
		PERR ("malloc");
		return NULL;
	}

	strcpy (buf, str);
	buf[len - 1] = '\0';

	*buffer_size = len;

	return buf;
}

storage_unit* storage_unit_named_integer (long int value, char* name) {
	long int* p = int_ptr (value);
	char* str = str_ptr (name);

	id_tuple* tuple = create_id_tuple (str, p, TT_INTEGER);
	return storage_unit_ptr (ST_ID_TUPLE, tuple);
}

storage_unit* storage_unit_named_float (long double value, char* name) {
	long double* p = double_ptr (value);
	char* str = str_ptr (name);

	id_tuple* tuple = create_id_tuple (str, p, TT_FLOAT);
	return storage_unit_ptr (ST_ID_TUPLE, tuple);
}

storage_unit* storage_unit_named_char (char value, char* name) {
	char* p = char_ptr (value);
	char* str = str_ptr (name);

	id_tuple* tuple = create_id_tuple (str, p, TT_CHARACTER);
	return storage_unit_ptr (ST_ID_TUPLE, tuple);
}

storage_unit* storage_unit_named_string (char* value, char* name) {
	char* p = str_ptr (value);
	char* str = str_ptr (name);

	id_tuple* tuple = create_id_tuple (str, p, TT_STRING);
	return storage_unit_ptr (ST_ID_TUPLE, tuple);
}

storage_unit* storage_unit_named_heredoc (char* value, char* name) {
	char* p = str_ptr (value);
	char* str = str_ptr (name);

	id_tuple* tuple = create_id_tuple (str, p, TT_HEREDOC);
	return storage_unit_ptr (ST_ID_TUPLE, tuple);
}

storage_unit* storage_unit_named_array (auto_array* value, char* name) {
	char* str = str_ptr (name);

	id_tuple* tuple = create_id_tuple (str, value, TT_ARRAY);
	return storage_unit_ptr (ST_ID_TUPLE, tuple);
}

storage_unit* storage_unit_named_set (set* value, char* name) {
	char* str = str_ptr (name);

	id_tuple* tuple = create_id_tuple (str, value, TT_SET);
	return storage_unit_ptr (ST_ID_TUPLE, tuple);
}

storage_unit* storage_unit_named_sequence (auto_array* value, char* name) {
	char* str = str_ptr (name);

	id_tuple* tuple = create_id_tuple (str, value, TT_SEQUENCE);
	return storage_unit_ptr (ST_ID_TUPLE, tuple);
}

storage_unit* storage_unit_named_map (hash_table* value, char* name) {
	char* str = str_ptr (name);

	id_tuple* tuple = create_id_tuple (str, value, TT_MAP);
	return storage_unit_ptr (ST_ID_TUPLE, tuple);
}

