#include "serc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <softsprocket/container.h>
#include <softsprocket/debug_utils.h>


char* str_return_value (char* str) {
	char* pstr;
	if (str[0] == '"') {
		pstr = &str[1];
	} else {
		pstr = &str[0];
	}
	
	int len = strlen (pstr);

	if (pstr[len - 1] == '"') {
		pstr[len - 1] = '\0';
		--len;
	}
	
	char* rv = malloc (len + 1);

	if (rv == NULL) {
		perror ("malloc");
		return NULL;
	}

	strcpy (rv, pstr);

	return rv;
}

literal_value* create_literal_value (literal_value_type type, void* data) {
	literal_value* dv = malloc (sizeof (literal_value));
	if (dv == NULL) {
		PERR ("malloc");
		return NULL;
	}

	dv->type = type;
	dv->value = data;
	return dv;
}

auto_array* create_array (void* value) {
	auto_array* aa = auto_array_create (1024);
	if (aa == NULL) {
		PMSG ("auto_array");
		return NULL;
	}

	auto_array_add (aa, value);

	return aa;
}

id_tuple* create_id_tuple (char* id, void* data, id_tuple_type type) {
	id_tuple* it = malloc (sizeof (id_tuple));
	if (it == NULL) {
		PERR ("malloc");
		return NULL;
	}

	it->id = id;
	it->data = data;
	it->type = type;

	return it;
}

int long_int_equals (void* this, void* other) {
	long int *l = this;
	long int *r = other;
	
	return *l == *r;
}

int long_double_equals (void* this, void* other) {
	long double *l = this;
	long double *r = other;
	
	long double al = fabsl (*l);
	long double ar = fabsl (*r);

	long double aor1 = (al > 1.0) ? al : 1.0;

	long double max = (aor1 > ar) ? aor1 : ar;
	long double diff = fabsl (*l - *r) / max;

	return diff < LDBL_EPSILON;
}

int set_equals (void* this, void* other) {
	set* l = this;
	set* r = other;

	if (l->count != r->count) {
		return 0;
	}

	for (int i = 0; i < l->count; ++i) {
		storage_unit* lsu = set_get_item (l, i);

		int match = 0;

		for (int j = 0; j < r->count; ++j) {
			storage_unit* rsu = set_get_item (r, j);
			
			if (lsu->type != rsu->type) {
				continue;
			}

			if (lsu->type == ST_SET) {
				if ((match = set_equals (lsu->s, rsu->s))) {
					break;
				}

			} else {
				if (lsu->literal->type == LV_INTEGER) {
					if ((match = long_int_equals (lsu->literal->value, rsu->literal->value))) {
						break;
					}			
				} else {
					if ((match = long_double_equals (lsu->literal->value, lsu->literal->value))) {
						break;
					}
				}
			}
		}

		if (!match) {
			return 0;
		}
			
	}

	return 1;
}

int storage_unit_equals (void* this, void* other) {
	storage_unit* lsu = this;
	storage_unit* rsu = other;

	switch (lsu->type) {
		case ST_SET: {
			set* l = lsu->s;
			set* r = rsu->s;
			return set_equals (l, r);
		}
		case ST_LITERAL_VALUE:

			if (lsu->literal->type == LV_INTEGER) {
				long int* l = lsu->literal->value;
				long int* r = rsu->literal->value;
				return long_int_equals (l, r);
			} else {
				long double* l = lsu->literal->value;
				long double* r = rsu->literal->value;
				return long_double_equals (l, r);
			}
		case ST_ID_TUPLE:
		case ST_SUBDOC:
			break; // not currently implemented
	}

	return 0;	
}

set* array_to_set (auto_array* aa) {

	set* s = set_create (aa->count, storage_unit_equals);
	if (s == NULL) {
		PMSG ("set");
		return NULL;
	}

	set_add_items (s, aa->data, aa->count);

	auto_array_delete (aa, NULL);

	return s;
}

auto_array* create_sequence (hash_table* value) {
	auto_array* seq = auto_array_create (100);
	if (seq == NULL) {
		PMSG ("auto_array");
		return NULL;
	}

	auto_array_add (seq, value);

	return seq;
}

hash_table* create_map (char* key, storage_unit* value) {
	hash_table* ht = hash_table_create (100);
	if (ht == NULL) {
		PMSG ("hash_table");
		return NULL;
	}
	
	hash_table_put (ht, key, value);

	if (value->type != ST_ID_TUPLE) {
		free (key);
	}

	return ht;
}

long int* int_ptr (long int value) {
	long int* p = malloc (sizeof (long int));
	if (p == NULL) {
		PERR ("malloc");
		return NULL;
	}

	*p = value;

	return p;
}

long double* double_ptr (long double value) {
	long double* p = malloc (sizeof (long double));
	if (p == NULL) {
		PERR ("malloc");
		return NULL;
	}

	*p = value;

	return p;
}

char* char_ptr (char value) {
	char* p = malloc (sizeof (char));
	if (p == NULL) {
		PERR ("malloc");
		return NULL;
	}

	*p = value;

	return p;
}

char* str_ptr (char* value) {
	char* p = malloc (strlen (value) + 1);
	if (p == NULL) {
		PERR ("malloc");
		return NULL;
	}

	strcpy (p, value);

	return p;
}

storage_unit* storage_unit_ptr (stored_type type, void* value) {
	storage_unit* su = malloc (sizeof (storage_unit));
	if (su == NULL) {
		PERR ("malloc");
		return NULL;
	}

	su->type = type;

	if (type == ST_ID_TUPLE) {
		su->tuple = value;
	} else if (type == ST_SUBDOC) {
		su->map = value;
	} else if (type == ST_SET) {
		su->s = value;
	} else {
		su->literal = value;
	}

	return su;
}

char* array_to_auto_string (auto_array* aa, auto_string* str) {
	char* begin = "[";
	auto_string_append (str, begin);
	char* comma = ",";
	int use_comma = 0;

	for (int i = 0; i < aa->count; ++i) {

		if (use_comma) {
			auto_string_append (str, comma);
		}

		literal_value* lv = auto_array_get (aa, i);
		literal_to_auto_string (lv, str);

		use_comma = 1;

	}

	char* term = "]";
	auto_string_append (str, term);

	return str->buf;
}

char* set_to_auto_string (set* s, auto_string* str) {
	auto_string_append (str, "(");

	char* comma = ",";
	int use_comma = 0;

	for (size_t i = 0; i < s->count; ++i) {
		if (use_comma) {
			auto_string_append (str, comma);
		}

		storage_unit* su = set_get_item (s, i);
		if (su->type == ST_ID_TUPLE) {
			id_tuple_to_auto_string (su->tuple, str);
		} else if (su->type == ST_SET) {
			set_to_auto_string (su->s, str);
		} else {
			literal_to_auto_string (su->literal, str);
		}

		use_comma = 1;
	}

	auto_string_append (str, ")");
	return str->buf;
}

char* map_to_auto_string (hash_table* ht, auto_string* str) {
	auto_string_append (str, "{");

	char* comma = ",";
	int use_comma = 0;

	auto_array* keys = hash_table_keys (ht);

	for (int i = 0; i < keys->count; ++i) {
		if (use_comma) {
			auto_string_append (str, comma);
		}

		char* key = auto_array_get (keys, i); 
		storage_unit* su = hash_table_get (ht, key);
		auto_string_append (str, key);
		auto_string_append (str, ":");

		if (su->type == ST_ID_TUPLE) {
			id_tuple_to_auto_string (su->tuple, str);
		} else if (su->type == ST_LITERAL_VALUE) {
			literal_to_auto_string (su->literal, str);
		} else {
			document_to_auto_string (su->map, str);
		}

		use_comma = 1;
	}	

	auto_string_append (str, "}");
	
	auto_array_delete (keys, NULL);

	return str->buf;
}

char* literal_to_auto_string (literal_value* lit, auto_string* str) {
	int sz = 256;
	char* val = malloc (sz);
	if (val == NULL) {
		PERR ("malloc");
		return NULL;
	}

	int append_val = 1;

	int rv = 0;
	switch (lit->type) {
		case LV_STRING:
			while ((rv = snprintf (val, sz, "\"%s\"", (char*) lit->value)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}

			break;
		case LV_INTEGER:
			while ((rv = snprintf (val, sz, "%ld", *(long int*) lit->value)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}

			break;
		case LV_FLOAT:
			while ((rv = snprintf (val, sz, "%Lf", *(long double*) lit->value)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}

			break;
		case LV_HEREDOC:
			while ((rv = snprintf (val, sz, "```%s```", (char*) lit->value)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}

			break;
		case LV_CHARACTER:
			while ((rv = snprintf (val, sz, "'%c'", *(char*) lit->value)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}

			break;
		case LV_ARRAY:
			array_to_auto_string (lit->value, str);
			append_val = 0;
			break;
		default:
			free (val);
			PMSG ("Unknown literal value type");
			return NULL;
	}

	if (append_val) {
		auto_string_append (str, val);
	}

	free (val);

	return str->buf;	
}

char* id_tuple_to_auto_string (id_tuple* it, auto_string* str) {

	int sz = 256;
	char* val = malloc (sz);
	if (val == NULL) {
		PERR ("malloc");
		return NULL;
	}

	int rv = 0;
	switch (it->type) {
		case TT_STRING:
			while ((rv = snprintf (val, sz, "String %s=\"%s\"", it->id, (char*) it->data)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}
			auto_string_append (str, val);

			break;
		case TT_HEREDOC:
			while ((rv = snprintf (val, sz, "String %s=```%s```", it->id, (char*) it->data)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}
			auto_string_append (str, val);
			break;
		case TT_INTEGER:
			while ((rv = snprintf (val, sz, "Integer %s=%ld ", it->id, *(long int*) it->data)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}
			auto_string_append (str, val);
			break;
		case TT_FLOAT:
			while ((rv = snprintf (val, sz, "Float %s=%Lg ", it->id, *(long double*) it->data)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}
			auto_string_append (str, val);
			break;
		case TT_CHARACTER:
			while ((rv = snprintf (val, sz, "Character %s='%c' ", it->id, *(char*) it->data)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}
			auto_string_append (str, val);
			break;
		case TT_ARRAY:
			while ((rv = snprintf (val, sz, "Array %s=", (char*) it->id)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}
			auto_string_append (str, val);
			array_to_auto_string (it->data, str);
			//auto_string_append (str, " ");
			break;
		case TT_SET:
			while ((rv = snprintf (val, sz, "Set %s=", (char*) it->id)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}

			auto_string_append (str, val);
			set_to_auto_string (it->data, str);
			//auto_string_append (str, " ");
			break;
		case TT_SEQUENCE:
			while ((rv = snprintf (val, sz, "Sequence %s={", (char*) it->id)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}
			auto_string_append (str, val);
			auto_array* sq = it->data;
			char* comma = ",";
			int use_comma = 0;

			for (int i = 0; i < sq->count; ++i) {
				if (use_comma) {
					auto_string_append (str, comma);
				}
				hash_table* doc = auto_array_get (sq, i);
				document_to_auto_string (doc, str);
				use_comma = 1;
			}
			auto_string_append (str, "}");
			break;
		case TT_MAP:
			while ((rv = snprintf (val, sz, "Map %s=", (char*) it->id)) > sz) {
				sz = rv;
				char* tmp = realloc (val, sz);
				if (tmp == NULL) {
					PERR ("malloc");
					free (val);
					return NULL;
				}

				val = tmp;
			}
			auto_string_append (str, val);
			map_to_auto_string (it->data, str);
			break;
		default:
			PMSG ("Unknown tuple type");
			free (val);
			return NULL;
	}

	free (val);

	return str->buf;	
}

char* document_to_auto_string (hash_table* document, auto_string* str) {
	auto_array* keys = hash_table_keys (document);
	if (keys == NULL) {
		PERR ("auto_array");
		return NULL;
	}

	auto_string_append (str, "{");

	for (int i = 0; i < keys->count; ++i) {
		char* key = auto_array_get (keys, i);
		id_tuple* it = ((storage_unit*) hash_table_get (document, key))->tuple;
		id_tuple_to_auto_string (it, str);
	}

	auto_string_append (str, "}");

	auto_array_delete (keys, NULL);

	return str->buf;	
}

void delete_id_tuple (void* v) {
	id_tuple* tuple = v;
	switch (tuple->type) {
		case TT_STRING:
		case TT_HEREDOC:
		case TT_INTEGER:
		case TT_FLOAT:
		case TT_CHARACTER:
			free (tuple->data);
			break;
		case TT_ARRAY: {
				auto_array* aa = tuple->data;
				auto_array_delete (aa, delete_literal_value);
			}
			break;
		case TT_SET: {
				set* s = tuple->data;
				set_delete (s, delete_storage_unit);
			}
			break;
		case TT_SEQUENCE: {
				auto_array* seq = tuple->data;
				auto_array_delete (seq, delete_document);
			}
			break;
		case TT_MAP: {
				hash_table* ht = tuple->data;
				hash_table_delete (ht, delete_storage_unit);
			}
			break;
		default:
			PMSG ("Unknown id_tuple type");
			break;

	}

	free (tuple->id);
	free (tuple);
}

void delete_literal_value (void* v) {
	literal_value* literal = v;
	if (literal->type == LV_ARRAY) {
		auto_array_delete (literal->value, delete_literal_value);
	} else {
		free (literal->value);
	}

	free (literal);
}

void delete_storage_unit (void* v) {
	storage_unit* su = v;
	switch (su->type) {
		case ST_ID_TUPLE:
			delete_id_tuple (su->tuple);
			break;
		case ST_LITERAL_VALUE:
			delete_literal_value (su->literal);
			break;
		case ST_SUBDOC:
			delete_document (su->map);
			break;
		case ST_SET:
			set_delete (su->s, delete_storage_unit);
			break;
		default:
			PMSG ("Unknown storage unit type");
			break;
	}

	free (su);
}

void delete_document (void* v) {
	hash_table_delete (v, delete_storage_unit);
}


auto_string* serc_document_to_auto_string (hash_table* document) {
	auto_string* the_str = auto_string_create (32); 
	document_to_auto_string (document, the_str);

	return the_str;
}

