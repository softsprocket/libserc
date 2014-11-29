/** @file */
/**
* \mainpage 
* 	libserc - Serc data serialization language reference library
*    ==============================================================
* \code{.unparsed}   
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
* \endcode
*/

#ifndef SERC_H_
#define SERC_H_

#ifndef SOFTSPROCKET_CONTAINER_HEADER_INCLUDED_
#define SOFTSPROCKET_CONTAINER_HEADER_INCLUDED_
#include <softsprocket/container.h>
#endif // SOFTSPROCKET_CONTAINER_HEADER_INCLUDED_

/**
 * enumerates a literal_value.
 * @see literal_value
 */
typedef enum {
	LV_STRING,
	LV_INTEGER,
	LV_FLOAT,
	LV_HEREDOC,
	LV_CHARACTER,
	LV_ARRAY
} literal_value_type;

/**
 * storage type for values enumerated by literal_value_type.
 * @see literal_value_type
 */ 
typedef struct {
	literal_value_type type;
	void* value;
} literal_value;

/**
 * enumerates id_tuple types.
 * @see id_tuple
 */
typedef enum {
	TT_STRING,
	TT_HEREDOC,
	TT_INTEGER,
	TT_FLOAT,
	TT_CHARACTER,
	TT_ARRAY,
	TT_SET,
	TT_SEQUENCE,
	TT_MAP
} id_tuple_type;

/**
 * key value types enumerated by id_tuple_type.
 * @see id_tuple
 */
typedef struct {
	id_tuple_type type;
	char* id;
	void* data;
} id_tuple;

/**
 * enumerates storage_unit types.
 * @see storage_unit
 */
typedef enum {
	ST_ID_TUPLE,
	ST_LITERAL_VALUE,
	ST_SUBDOC,
	ST_SET
} stored_type;

/**
 * generic type container enumerated by stored_type.
 * @see stored_type
 */
typedef struct {
	stored_type type;
	union {
		id_tuple* tuple;
		literal_value* literal; 	
		hash_table* map;
		set* s;	
	};
} storage_unit;

/**
 * extracts hash_table from storage_unit.
 */
#define SU_TO_MAP(su) su->map
/**
 * extracts set from storage_unit.
 */
#define SU_TO_SET(su) su->s

/**
 * extracts char* from storage_unit id_tuple.
 */ 
#define SU_TUPLE_TO_STRING(su) su->tuple->data
/**
 * extracts long double from storage_unit id_tuple.
 */ 
#define SU_TUPLE_TO_LONG_DOUBLE(su) *(long double*)su->tuple->data 
/**
 * extracts long int from storage_unit id_tuple.
 */ 
#define SU_TUPLE_TO_LONG_INT(su) *(long int*)su->tuple->data
/**
 * extracts char from storage_unit id_tuple.
 */ 
#define SU_TUPLE_TO_CHAR(su) *(char*)su->tuple->data
/**
 * extracts auto_array* from storage_unit id_tuple.
 */ 
#define SU_TUPLE_TO_ARRAY(su) su->tuple->data
/**
 * extracts set* from storage_unit id_tuple.
 */ 
#define SU_TUPLE_TO_SET(su) su->tuple->data
/**
 * extracts hash_table* from storage_unit id_tuple.
 */ 
#define SU_TUPLE_TO_MAP(su) su->tuple->data

/**
 * extracts long int from storage_unit literal.
 */ 
#define SU_LITERAL_TO_LONG_INT(su) *(long int*)su->literal->value
/**
 * extracts long double from storage_unit literal.
 */ 
#define SU_LITERAL_TO_LONG_DOUBLE(su) *(long double*)su->literal->value
/**
 * extracts char* from storage_unit literal.
 */ 
#define SU_LITERAL_TO_STRING(su) su->literal->value
/**
 * extracts char from storage_unit literal.
 */ 
#define SU_LITERAL_TO_CHAR(su) *(char*)su->literal->value
/**
 * extracts auto_array* from storage_unit literal.
 */ 
#define SU_LITERAL_TO_ARRAY(su) su->literal->value

/**
 * extracts the id from a tuple
 */
#define SU_TUPLE_ID(su) su->tuple->id

/**
 * copies a string removing quotations from the front and back if they exist.
 * @param str the string to copy or NULL in the event of an error
 * @return a string
 */
char* str_return_value (char* str);

/**
 * creates a literal_value object on the heap.
 * @param type the type of the object
 * @param data a pointer to the value of the literal
 * @return a pointer to the literal_value or NULL in the event of an error
 * @see literal_value_type
 * @see literal_value
 */ 
literal_value* create_literal_value (literal_value_type type, void* data);

/**
 * creates a id_tuple object on the heap.
 * @param id the name of the tuple
 * @param data the value of the tuple
 * @param type the type of the object
 * @return a pointer to the id_tuple or NULL in the event of an error
 * @see id_tuple_type
 * @see id_tuple
 */ 
id_tuple* create_id_tuple (char* id, void* data, id_tuple_type type);

/**
 * creates an auto_array on the heap and adds a value to it. A Serc Array or Set can be
 * represented by an auto_array. In both cases the object stored should be a storage_unit.
 * auto_array is from libsscont
 * @param value a pointer to a value to store or NULL in the event of an error
 * @return a pointer to an auto_array
 */
auto_array* create_array (void* value);

/**
 * creates a set on the heap from the data of an auto_array of storage_units.
 * and deletes the auto_array. A Serc Set is represented by a set.
 * set is from libsscont
 * @param aa an auto_array of storage_unit pointers
 * @return a pointer to a set or NULL in the event of an error
 * @see storage_unit
 */
set* array_to_set (auto_array* aa);

/**
 * creates an auto_array on the heap and adds a hash_table pointer to it.
 * A Serc sequence is reprsented by an auto_array. The hash_table represents
 * a Serc document or subdocument.
 * auto_array is from libsscont
 * @param value a hash_table to store
 * @return a pointer to an auto_array or NULL in the event of an error
 */
auto_array* create_sequence (hash_table* value);

/**
 * creates a hash_table on the heap and stores a value in it.
 * It is expected that the storage_unit contains an id_tuple
 * and its namewill be the key.
 * hash_table is from libsscont
 * @param key the key to store value under
 * @param value the value to be stored
 * @return a pointer to a hash_table or NULL in the event of an error
 */
hash_table* create_map (char* key, storage_unit* value);

/**
 * allocates a char* on the heap and coppies a value to it.
 * @param value the value to assign 
 * @return a char* or NULL in the event of an error
 */
char* str_ptr (char* value);

/**
 * allocates a long int on the heap and assigns a value to it.
 * @param value the value to assign 
 * @return a pointer to a long int or NULL in the event of an error
 */
long int* int_ptr (long int value);

/**
 * allocates a long double on the heap and assigns a value to it.
 * @param value the value to assign 
 * @return a pointer to a long double or NULL in the event of an error
 */
long double* double_ptr (long double value);

/**
 * allocates a char on the heap and assigns a value to it.
 * @param value the value to assign 
 * @return a pointer to a char or NULL in the event of an error
 */
char* char_ptr (char value);

/**
 * allocates a storage_unit on the heap and assigns a value and type to it.
 * @param type the type to assign
 * @param value the value to assign 
 * @return a pointer to a storage_unit or NULL in the event of an error
 * @see storage_unit
 */
storage_unit* storage_unit_ptr (stored_type type, void* value);

/**
 * converts a Serc document stored as a hash_table into an auto_string.
 * hash_table and auto_string are from libsscont
 * @param document a hash_table containing the Serc document elements
 * @param str a pointer to an auto_str that will contain the converted document
 * @return a pointer to the nul terminated buf element of the auto_string or NULL in the event of an error
 */ 
char* document_to_auto_string (hash_table* document, auto_string* str);

/**
 * converts a Serc Array, stored as an auto_array, to an auto_string.
 * auto_array is from libserc
 * @param aa the auto_array to convert
 * @param str a pointer to the auto_string that will contain the converted array
 * @return a pointer to the nul terminated buf element of the auto_string or NULL in the event of an error
 */ 
char* array_to_auto_string (auto_array* aa, auto_string* str);

/**
 * converts a Serc set, stored as an libsscont set, to an auto_string.
 * @param s the set to convert
 * @param str a pointer to the auto_string that will contain the converted set
 * @return a pointer to the nul terminated buf element of the auto_string or NULL in the event of an error
 */ 
char* set_to_auto_string (set* s, auto_string* str);

/**
 * converts a Serc id_tuple to an auto_string.
 * @param it the id_tuple to convert
 * @param str a pointer to the auto_string that will contain the converted id_tuple
 * @return a pointer to the nul terminated buf element of the auto_string or NULL in the event of an error
 */ 
char* id_tuple_to_auto_string (id_tuple* it, auto_string* str);

/**
 * converts a Serc literal_value to an auto_string.
 * @param lit the literal_value to convert
 * @param str a pointer to the auto_string that will contain the converted literal_value
 * @return a pointer to the nul terminated buf element of the auto_string or NULL in the event of an error
 */ 
char* literal_to_auto_string (literal_value* lit, auto_string* str);

/**
 * converts a hash_table representing a Serc map to an auto_string.
 * @param ht the hash_table to convert
 * @param str a pointer to the auto_string that will contain the converted map 
 * @return a pointer to the nul terminated buf element of the auto_string or NULL in the event of an error
 */ 
char* map_to_auto_string (hash_table* ht, auto_string* str);

/**
 * free the memory from a Serc document (hash_table of storage_unit types).
 * Parameter is a void* to match libsscont hash_table_delete function pointer specification
 * but a hash_table* is expcted.
 * @param v the document to free
 */
void delete_document (void* v);

/**
 * free the memory from a Serc id_tuple. 
 * Parameter is void* for use as function pointer to libsscont delete functions
 * but an id_tuple* is the expected type.
 * @param v the id_tuple to free
 */
void delete_id_tuple (void* v);

/**
 * free the memory from a Serc storage_unit.
 * Parameter is void* for use as function pointer to libsscont delete functions
 * but a storage_unit* is the expected type.
 * @param v the storage_unit to free
 */
void delete_storage_unit (void* v);

/**
 * free the memory from a Serc literal_value.
 * Parameter is void* for use as function pointer to libsscont delete functions
 * but a literal_value* is the expected type
 * @param v the literal_value to free
 */
void delete_literal_value (void* v);

/**
 * convenience function - creates an auto_string and calls document_to_auto_string.
 * auto_string and hash_table are from libsscont
 * @param document the Serc document stored as a hash_table
 * @return a pointer to an auto_string containing the document or NULL in the eventof an error
 */
auto_string* serc_document_to_auto_string (hash_table* document);

/**
 * parses a Serc document into a form useable by a C program.
 * The buffer must terminated with two nul bytes.
 * hash_table is from libsscont
 * @param buffer the string to parse
 * @param buffer_size the size of the buffer
 * @return a pointer to hash_table representing the Serc document or NULL in the event of an error
 */  
hash_table* serc_parse_buffer (char* buffer, size_t buffer_size);

/**
 * reads a file and converts it to a buffer suitable for Serc parse buffer
 * @param fname the path of the file to read
 * @param buffer_size pointer to an int which will be populated with the returned buffer size
 * @return buffer containing the Serc document to be parsed or NULL in the eventof an error
 */ 
char* serc_read_file (char* fname, size_t* buffer_size);

/** 
 * convert a nul terminated c string into a double terminated parse buffer.
 * Makes a copy of the string
 * @param str a c string containing the Serc data
 * @param buffer_size an int pointer for the return buffer size
 * @return the parseable buffer or NULL in the event of an error
 */
char* parseable_buffer_from_string (char* str, size_t* buffer_size);

/**
 * creates a storage_unit of type ST_ID_TUPLE that contains an id_tuple* of
 * type TT_INTEGER. A convenience function that allocates the memory required
 * for the id, name and the tuple and makes the appropriate assignments.
 * A copy is made of the string name.
 * @param value the value to assign the id_tuple
 * @param name the name to assign the tuple
 * @return a storage_unit* or NULL if an error occurs
 */
storage_unit* storage_unit_named_integer (long int value, char* name);

/**
 * creates a storage_unit of type ST_ID_TUPLE that contains an id_tuple* of
 * type TT_FLOAT. A convenience function that allocates the memory required
 * for the id, name and the tuple and makes the appropriate assignments.
 * A copy is made of the string name.
 * @param value the value to assign the id_tuple
 * @param name the name to assign the tuple
 * @return a storage_unit* or NULL if an error occurs
 */
storage_unit* storage_unit_named_float (long double value, char* name);

/**
 * creates a storage_unit of type ST_ID_TUPLE that contains an id_tuple* of
 * type TT_CHARACTER. A convenience function that allocates the memory required
 * for the id, name and the tuple and makes the appropriate assignments.
 * A copy is made of the string name.
 * @param value the value to assign the id_tuple
 * @param name the name to assign the tuple
 * @return a storage_unit* or NULL if an error occurs
 */
storage_unit* storage_unit_named_char (char value, char* name);

/**
 * creates a storage_unit of type ST_ID_TUPLE that contains an id_tuple* of
 * type TT_STRING. A convenience function that allocates the memory required
 * for the id, and the tuple and makes the appropriate assignments.
 * A copy is made of the string value. A copy is made of the string name.
 * @param value the value to assign the id_tuple
 * @param name the name to assign the tuple
 * @return a storage_unit* or NULL if an error occurs
 */
storage_unit* storage_unit_named_string (char* value, char* name);

/**
 * creates a storage_unit of type ST_ID_TUPLE that contains an id_tuple* of
 * type TT_HEREDOC. A convenience function that allocates the memory required
 * for the id, name and the tuple and makes the appropriate assignments.
 * The char* value should be on the heap (no copy is made). A copy is made of the string name.
 * @param value the value to assign the id_tuple
 * @param name the name to assign the tuple
 * @return a storage_unit* or NULL if an error occurs
 */
storage_unit* storage_unit_named_heredoc (char* value, char* name);

/**
 * creates a storage_unit of type ST_ID_TUPLE that contains an id_tuple* of
 * type TT_ARRAY. A convenience function that allocates the memory required
 * for the id, name and the tuple and makes the appropriate assignments.
 * The auto_array* value should be on the heap (no copy is made). A copy is made of the string name.
 * @param value the value to assign the id_tuple
 * @param name the name to assign the tuple
 * @return a storage_unit* or NULL if an error occurs
 */
storage_unit* storage_unit_named_array (auto_array* value, char* name);

/**
 * creates a storage_unit of type ST_ID_TUPLE that contains an id_tuple* of
 * type TT_SET. A convenience function that allocates the memory required
 * for the id, name and the tuple and makes the appropriate assignments.
 * The set* value should be on the heap (no copy is made). A copy is made of the string name.
 * @param value the value to assign the id_tuple
 * @param name the name to assign the tuple
 * @return a storage_unit* or NULL if an error occurs
 */
storage_unit* storage_unit_named_set (set* value, char* name);

/**
 * creates a storage_unit of type ST_ID_TUPLE that contains an id_tuple* of
 * type TT_SEQUENCE. A convenience function that allocates the memory required
 * for the id, name and the tuple and makes the appropriate assignments.
 * The auto_array* value should be on the heap (no copy is made). A copy is made of the string name.
 * @param value the value to assign the id_tuple
 * @param name the name to assign the tuple
 * @return a storage_unit* or NULL if an error occurs
 */
storage_unit* storage_unit_named_sequence (auto_array* value, char* name);

/**
 * creates a storage_unit of type ST_ID_TUPLE that contains an id_tuple* of
 * type TT_MAP. A convenience function that allocates the memory required
 * for the id, name and the tuple and makes the appropriate assignments.
 * The hash_table* value should be on the heap (no copy is made). A copy is made of the string name.
 * @param value the value to assign the id_tuple
 * @param name the name to assign the tuple
 * @return a storage_unit* or NULL if an error occurs
 */
storage_unit* storage_unit_named_map (hash_table* value, char* name);


#endif // SERC_H_
