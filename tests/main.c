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

#include <softsprocket/debug_utils.h>
#include <softsprocket/container.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>

int test_conversions (char* fname);
int test_loaded_objects (char* fname);
int test_limits ();
int separation_tests ();
int float_test ();

int main (int argc, char** argv) {

	if (argc != 2) {
		fprintf (stderr, "usage: %s <serc filename>\n", argv[0]);
		exit (EXIT_FAILURE);
	}


	int rv = EXIT_SUCCESS;
	rv = rv | test_conversions (argv[1]);
	rv = rv | test_loaded_objects (argv[1]);	
	rv = rv | test_limits ();
	rv = rv | separation_tests ();

	return rv;
}

int test_conversions (char* fname) {
	
	size_t buffer_size = 0;
	char* read_buf = serc_read_file (fname, &buffer_size);
	
	if (read_buf == NULL) {
		PMSG ("serc_read_file");
		return EXIT_FAILURE;
	}

	hash_table* the_results = serc_parse_buffer (read_buf, buffer_size);
	if (the_results == NULL) {
		PMSG ("serc_parse_buffer");
		free (read_buf);
		return EXIT_FAILURE;
	}

	free (read_buf);

	auto_string* the_str = serc_document_to_auto_string (the_results);
	if (the_str == NULL) {
		PMSG ("serc_document_to_auto_string");
		delete_document (the_results);
		return EXIT_FAILURE;
	}

	delete_document (the_results);

	size_t len;

	char* new_buf = parseable_buffer_from_string (the_str->buf, &len);
	if (new_buf == NULL) {
		PERR (" parseable_buffer_from_string");
		auto_string_delete (the_str);
		return EXIT_FAILURE;
	}

	the_results = serc_parse_buffer (new_buf, len);
	if (the_results == NULL) {
		PMSG ("serc_parse_buffer");
		auto_string_delete (the_str);
		free (new_buf);
		return EXIT_FAILURE;
	}

	free (new_buf);

	auto_string* new_str = serc_document_to_auto_string (the_results);
	if (new_str == NULL) {
		PMSG ("serc_document_to_auto_string");
		delete_document (the_results);
		return EXIT_FAILURE;
	}

	delete_document (the_results);

	if (strcmp (the_str->buf, new_str->buf) != 0) {
		PMSG ("conversion buffers are not equal");
		auto_string_delete (the_str);	
		auto_string_delete (new_str);
		return EXIT_FAILURE;	
	}

	auto_string_delete (the_str);	
	auto_string_delete (new_str);
	printf ("conversion test passed\n");

	return EXIT_SUCCESS;

}

const long double SS_EPSILON = 1.3e-6;

int long_double_equality_test (long double l, long double r) {

	long double al = fabsl (l);
	long double ar = fabsl (r);

	long double aor1 = (al > 1.0) ? al : 1.0;

	long double max = (aor1 > ar) ? aor1 : ar;

	long double diff = fabsl (l - r) / max;

	return diff < SS_EPSILON;
}

int test_loaded_objects (char* fname) {

	size_t buffer_size = 0;
	char* read_buf = serc_read_file (fname, &buffer_size);
	
	if (read_buf == NULL) {
		PMSG ("serc_read_file");
		return EXIT_FAILURE;
	}

	hash_table* the_results = serc_parse_buffer (read_buf, buffer_size);
	if (the_results == NULL) {
		PMSG ("serc_parse_buffer");
		free (read_buf);
		return EXIT_FAILURE;
	}

	free (read_buf);

	auto_array* keys = hash_table_keys (the_results);
	if (keys == NULL) {
		PMSG ("hash_table_keys returned NULL");
		delete_document (the_results);
		return EXIT_FAILURE;
	}

	int expected_keys_num = 6;

	if (keys->count != expected_keys_num) {
		PDEC ();
	        fprintf (stderr, "%lu != 6", keys->count);
		auto_array_delete (keys, NULL);
		delete_document (the_results);
		return EXIT_FAILURE;
	}

	auto_array_delete (keys, NULL);

        char* exp_str = "Hello, World!";
	storage_unit* su = hash_table_get (the_results, "str"); 
	if (su == NULL) {
		PMSG ("hash_table_get \"str\" returned NULL"); 
		delete_document (the_results);
		return EXIT_FAILURE;
	}
	char* str = SU_TUPLE_TO_STRING (su);

	if (strcmp (exp_str, str) != 0) {
		PDEC (); 
		fprintf (stderr, "%s != %s\n", exp_str, str);
		delete_document (the_results);
		return EXIT_FAILURE;
	}

        long double exp_fl = 1.23e6;
	su = hash_table_get (the_results, "fl"); 
	if (su == NULL) {
		PMSG ("hash_table_get \"fl\" returned NULL"); 
		delete_document (the_results);
		return EXIT_FAILURE;
	}

	long double fl = SU_TUPLE_TO_LONG_DOUBLE (su);

	if (!long_double_equality_test(exp_fl, fl)) {
		PDEC (); 
		fprintf (stderr, "%Lg != %Lg\n", exp_fl, fl);
		delete_document (the_results);
		return EXIT_FAILURE;
	}

        long int exp_i = 956;
	su = hash_table_get (the_results, "i"); 
	if (su == NULL) {
		PMSG ("hash_table_get \"i\" returned NULL"); 
		delete_document (the_results);
		return EXIT_FAILURE;
	}

	long int i = SU_TUPLE_TO_LONG_INT (su);

	if (exp_i != i) {
		PDEC (); 
		fprintf (stderr, "%ld != %ld\n", exp_i, i);
		delete_document (the_results);
		return EXIT_FAILURE;
	}


        char exp_c = 'c';
	su = hash_table_get (the_results, "c"); 
	if (su == NULL) {
		PMSG ("hash_table_get \"c\" returned NULL"); 
		delete_document (the_results);
		return EXIT_FAILURE;
	}

	char c = SU_TUPLE_TO_CHAR (su);

	if (exp_c != c) {
		PDEC (); 
		fprintf (stderr, "%c != %c\n", exp_c, c);
		delete_document (the_results);
		return EXIT_FAILURE;
	}

        int exp_ar[2][4] = {
		{1, 2, 3, 4},
		{'a', 'b', 'c', 'd'}
	};

	su = hash_table_get (the_results, "ar"); 
	if (su == NULL) {
		PMSG ("hash_table_get \"ar\" returned NULL"); 
		delete_document (the_results);
		return EXIT_FAILURE;
	}

	auto_array *ar = SU_TUPLE_TO_ARRAY (su);

	if (ar->count != 2) {
		PDEC();
		fprintf (stderr, "%lu != 2\n", ar->count);
		delete_document (the_results);
		return EXIT_FAILURE;
		
	}

	for (int i = 0; i < 2; ++i) { 
		literal_value* la = auto_array_get (ar, i);
		if (la == NULL) {
			PMSG ("auto_array_get returned NULL");
			delete_document (the_results);
			return EXIT_FAILURE;
		}

		auto_array* iar = la->value;
		if (iar->count != 4) {
			PDEC();
			fprintf (stderr, "%lu != 4\n", iar->count);
			delete_document (the_results);
			return EXIT_FAILURE;
		}

		for (int j = 0; j < 4; ++j) {
			literal_value* lv = auto_array_get (iar, j);
			if (lv == NULL) {
				PMSG ("auto_array_get return NULL");
				delete_document (the_results);
				return EXIT_FAILURE;
			}

			int exp_n = exp_ar[i][j];
			if (i == 0) {
				long int* n = lv->value;

				if (exp_n != *n) {
					PDEC (); 
					fprintf (stderr, "%d != %ld\n", exp_n, *n);
					delete_document (the_results);
					return EXIT_FAILURE;
				}
			} else {
				char* c = lv->value;

				if (exp_n != *c) {
					PDEC (); 
					fprintf (stderr, "%d != %c\n", exp_n, *c);
					delete_document (the_results);
					return EXIT_FAILURE;
				}	
			}
		}

	}

	su = hash_table_get (the_results, "vals"); 
	if (su == NULL) {
		PMSG ("hash_table_get \"vals\" returned NULL"); 
		delete_document (the_results);
		return EXIT_FAILURE;
	}

	auto_array *seq = SU_TUPLE_TO_ARRAY (su);

	if (seq->count != 3) {
		PDEC ();
		fprintf (stderr, "%lu != 3\n", seq->count);
		delete_document (the_results);
		return EXIT_FAILURE;
	}

	int exp_counts[3] = {
		2, 1, 2
	};

	for (int i = 0; i < seq->count; ++i) {
		hash_table* sub_doc = auto_array_get (seq, i);
		if (sub_doc == NULL) {
			PMSG ("auto_aray_get returned NULL"); 
			delete_document (the_results);
			return EXIT_FAILURE;
		}

		keys = hash_table_keys (sub_doc);
		if (keys->count != exp_counts[i]) {
			PDEC ();
			fprintf (stderr, "%lu != %d\n", keys->count, exp_counts[i]);
			delete_document (the_results);
			return EXIT_FAILURE;
		}
		
		auto_array_delete (keys, NULL);

		switch (i) {
			case 0: {
				storage_unit* su = hash_table_get (sub_doc, "inums");
				if (su == NULL) {
					PMSG ("hash_table_get \"inums\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				set* inums = SU_TUPLE_TO_SET (su);
				if (inums->count != 2) {
					PDEC ();
					fprintf (stderr, "%lu != 2\n", inums->count);
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				int exp_inums[2][4] = {
					{ 1, 2, 3, 4 },
					{ 2, 3, 4, 5 }
				};

				for (int j = 0; j < 2; ++j) {
					storage_unit* set_su = set_get_item (inums, j);
					if (set_su == NULL) {
						PMSG ("set_get_item returned NULL");
						delete_document (the_results);
						return EXIT_FAILURE;
					}

					set* sn = SU_TO_SET (set_su);
					if (sn->count != 4) {
						PDEC ();
						fprintf (stderr, "%lu != 4\n", sn->count);
						delete_document (the_results);
						return EXIT_FAILURE;
					}

					for (int k = 0; k < 4; ++k) {
						storage_unit* set_n = set_get_item (sn, k);
						if (set_n == NULL) {
							PMSG ("set_get_item returned NULL");
							delete_document (the_results);
							return EXIT_FAILURE;
						}

						long int li = SU_LITERAL_TO_LONG_INT (set_n);

						if (li != exp_inums[j][k]) {
							PDEC ();
							fprintf (stderr, "%lu != %d\n", li, exp_inums[j][k]);
							delete_document (the_results);
							return EXIT_FAILURE;
						}

					}

				}

				su = hash_table_get (sub_doc, "fnums");
				if (su == NULL) {
					PMSG ("hash_table_get \"fnums\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				set* fnums = SU_TUPLE_TO_SET (su);
				long double exp_fnums[2] = {
					1.2, 3.45
				};

				for (int j = 0; j < 2; ++j) {
					storage_unit* su_f = set_get_item (fnums, j);
					if (su_f == NULL) {
						PMSG ("set_get_item returned NULL");
						delete_document (the_results);
						return EXIT_FAILURE;
					}

					long double f = SU_LITERAL_TO_LONG_DOUBLE (su_f);
					if (!long_double_equality_test(exp_fnums[j], f)) {
						PDEC ();
						fprintf (stderr, "%Lg != %Lg\n", exp_fnums[j], f);
						return EXIT_FAILURE;
					}
				}


			}

			break;
			case 1: {

				storage_unit* su = hash_table_get (sub_doc, "heredoc");
				if (su == NULL) {
					PMSG ("hash_table_get \"heredoc\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}
				
				char* hd = SU_TUPLE_TO_STRING (su);
				
				char* exp_hd = "\n"
					"	Space will be 'preserved'\n"
					"	and \"quotes\" are ok\n";

				if (strcmp (exp_hd, hd) != 0) {
					PDEC ();
					fprintf (stderr, "<%s> != <%s>\n", exp_hd, hd);
				      	return EXIT_FAILURE;	
				}

			}

			break;
			case 2: {
				
				storage_unit* su = hash_table_get (sub_doc, "home");
				if (su == NULL) {
					PMSG ("hash_table_get \"home\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				hash_table* home = SU_TUPLE_TO_MAP (su);
				
				storage_unit* addr_su = hash_table_get (home, "Address");
				if (addr_su == NULL) {
					PMSG ("hash_table_get \"Address\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				char* exp_addr = "localhost";
				char* addr = SU_LITERAL_TO_STRING (addr_su);

				if (strcmp (exp_addr, addr) != 0) {
					PDEC ();
					fprintf (stderr, "%s != %s\n", exp_addr, addr);
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				storage_unit* port_su = hash_table_get (home, "Port");
				if (port_su == NULL) {
					PMSG ("hash_table_get \"Port\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				int exp_port = 80;
				long int port = SU_LITERAL_TO_LONG_INT (port_su);
				
				if (exp_port != port) {
					PDEC ();
					fprintf (stderr, "%ld != %d\n", port, exp_port);
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				storage_unit* msg_su = hash_table_get (home, "Message");
				if (msg_su == NULL) {
					PMSG ("hash_table_get \"Message\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				hash_table* msg = SU_TO_MAP (msg_su);

				storage_unit* msgstr_su = hash_table_get (msg, "str");
				if (msgstr_su == NULL) {
					PMSG ("hash_table_get \"str\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}
		
				char* msgstr = SU_TUPLE_TO_STRING (msgstr_su);
				char* exp_msgstr = "Hello, Localhost!";

				if (strcmp (msgstr, exp_msgstr) != 0) {
					PDEC ();
					fprintf (stderr, "%s != %s\n", msgstr, exp_msgstr);
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				storage_unit* msgi_su = hash_table_get (msg, "i");
				if (msgi_su == NULL) {
					PMSG ("hash_table_get \"str\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}
		
				long int msgi = SU_TUPLE_TO_LONG_INT (msgi_su);
				int exp_i = 123;

				if (msgi != exp_i) {
					PDEC ();
					fprintf (stderr, "%ld != %d\n", msgi, exp_i);
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				su = hash_table_get (sub_doc, "away");
				if (su == NULL) {
					PMSG ("hash_table_get \"away\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				hash_table* away = SU_TUPLE_TO_MAP (su);

				addr_su = hash_table_get (away, "Address");
				if (addr_su == NULL) {
					PMSG ("hash_table_get \"Address\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				char* exp_addr2 = "google.ca";
				addr = SU_LITERAL_TO_STRING (addr_su);

				if (strcmp (exp_addr2, addr) != 0) {
					PDEC ();
					fprintf (stderr, "%s != %s\n", exp_addr2, addr);
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				port_su = hash_table_get (away, "Port");
				if (port_su == NULL) {
					PMSG ("hash_table_get \"Port\" returned NULL");
					delete_document (the_results);
					return EXIT_FAILURE;
				}

				exp_port = 80;
				port = SU_LITERAL_TO_LONG_INT (port_su);
				
				if (exp_port != port) {
					PDEC ();
					fprintf (stderr, "%ld != %d\n", port, exp_port);
					delete_document (the_results);
					return EXIT_FAILURE;
				}

			}

			break;
			default:
				PMSG ("Unhandled test\n");
				return EXIT_FAILURE;
		
		}
	}

	delete_document (the_results);

	printf ("loaded objects test passed\n");

	return EXIT_SUCCESS;
}

int test_limits () {
	long int max = LONG_MAX;
	long int min = LONG_MIN;

	long double dmin = LDBL_MIN;
	long double dmax = LDBL_MAX;

	hash_table* document = hash_table_create (100);
	if (document == NULL) {
		PMSG ("hash_table_create failed");
		return EXIT_FAILURE;
	}


	storage_unit* max_su = storage_unit_named_integer (max, "long_int_max");
	hash_table_put (document, SU_TUPLE_ID(max_su), max_su);

	storage_unit* min_su = storage_unit_named_integer (min, "long_int_min");
	hash_table_put (document, SU_TUPLE_ID(min_su), min_su);

	storage_unit* dmax_su = storage_unit_named_float (dmax, "long_double_max");
	hash_table_put (document, SU_TUPLE_ID(dmax_su), dmax_su);
	
	storage_unit* dmin_su = storage_unit_named_float (dmin, "long_double_min");
	hash_table_put (document, SU_TUPLE_ID(dmin_su), dmin_su);
	
	auto_string* as = serc_document_to_auto_string (document);

	delete_document (document);

	size_t len = 0;
	char* new_buf = parseable_buffer_from_string (as->buf, &len);
	if (new_buf == NULL) {
		PERR (" parseable_buffer_from_string");
		return EXIT_FAILURE;
	}

	auto_string_delete (as);

	hash_table* the_results = serc_parse_buffer (new_buf, len);
	if (the_results == NULL) {
		PMSG ("serc_parse_buffer");
		return EXIT_FAILURE;
	}

	free (new_buf);	

	storage_unit* recovered_max_su = hash_table_get (the_results, "long_int_max");
	if (recovered_max_su == NULL) {
		PMSG ("hash_table_get returned NULL");
		return EXIT_FAILURE;
	}

	storage_unit* recovered_min_su = hash_table_get (the_results, "long_int_min");
	if (recovered_min_su == NULL) {
		PMSG ("hash_table_get returned NULL");
		return EXIT_FAILURE;
	}

	storage_unit* recovered_dmax_su = hash_table_get (the_results, "long_double_max");
	if (recovered_dmax_su == NULL) {
		PMSG ("hash_table_get returned NULL");
		return EXIT_FAILURE;
	}

	storage_unit* recovered_dmin_su = hash_table_get (the_results, "long_double_min");
	if (recovered_dmin_su == NULL) {
		PMSG ("hash_table_get returned NULL");
		return EXIT_FAILURE;
	}

	int rv = EXIT_SUCCESS;

	if (SU_TUPLE_TO_LONG_INT (recovered_max_su) != max) {
		fprintf (stderr, "long int max : %ld != %ld\n", SU_TUPLE_TO_LONG_INT (recovered_max_su), max);
		rv = EXIT_FAILURE;
	}
	
	if (SU_TUPLE_TO_LONG_INT (recovered_min_su) != min) {
		fprintf (stderr, "long int min : %ld != %ld\n", SU_TUPLE_TO_LONG_INT (recovered_min_su), min);
		rv = EXIT_FAILURE;
	}
	
	if (!long_double_equality_test (SU_TUPLE_TO_LONG_DOUBLE (recovered_dmax_su), dmax)) {
		fprintf (stderr, "long double max : %Lg != %Lg\n", SU_TUPLE_TO_LONG_DOUBLE (recovered_dmax_su), dmax);
		rv = EXIT_FAILURE;
	}

	if (!long_double_equality_test (SU_TUPLE_TO_LONG_DOUBLE (recovered_dmin_su), dmin)) {
		fprintf (stderr, "long double min : %Lg != %Lg\n", SU_TUPLE_TO_LONG_DOUBLE (recovered_dmin_su), dmin);
		rv = EXIT_FAILURE;
	}



	as = serc_document_to_auto_string (the_results);
	printf ("%s\n", as->buf);

	auto_string_delete (as);

	delete_document (the_results);


	if (rv == EXIT_SUCCESS) {
		printf ("limits test passed\n");
	}

	return rv;
}

int separation_tests () {
	char* doc_str = "{Integer num=1234 Map map={hello:\"Hello\",world:\"World\"}}";

	size_t len = 0;
	char* new_buf = parseable_buffer_from_string (doc_str, &len);
	if (new_buf == NULL) {
		PMSG (" parseable_buffer_from_string");
		return EXIT_FAILURE;
	}

	hash_table* the_results = serc_parse_buffer (new_buf, len);
	if (the_results == NULL) {
		PMSG ("serc_parse_buffer");
		free (new_buf);
		return EXIT_FAILURE;
	}

	free (new_buf);

	auto_string* new_str = serc_document_to_auto_string (the_results);
	if (new_str == NULL) {
		PMSG ("serc_document_to_auto_string");
		delete_document (the_results);
		return EXIT_FAILURE;
	}

	auto_string_delete (new_str);
	delete_document (the_results);

	printf ("separation tests succeeded\n"); 
	
	return EXIT_SUCCESS;
}

