
 	libserc - Serc data serialization language reference library
    ==============================================================
    Copyright (C) 2014 Gregory Ralph Martin
    info at softsprocket dot com

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

For details on libserc and the Serc data serialization language see: http://www.softsprocket.com/

SYNOPSIS
       #include <softsprocket/serc.h>
       #include <softsprocket/container.h>

       char* array_to_auto_string (auto_array* aa, auto_string* str);
       set* array_to_set (auto_array* aa);
       char* char_ptr (char value);
       auto_array* create_array (void* value);
       id_tuple* create_id_tuple (char* id, void* data, id_tuple_type type);
       literal_value* create_literal_value (literal_value_type type, void* data);
       hash_table* create_map (char* key, storage_unit* value);
       auto_array* create_sequence (hash_table* value);
       void delete_document (void* v);
       void delete_id_tuple (void* v);
       void delete_literal_value (void* v);
       void delete_storage_unit (void* v);
       char* document_to_auto_string (hash_table* document, auto_string* str);
       long double* double_ptr (long double value);
       char* id_tuple_to_auto_string (id_tuple* it, auto_string* str);
       long int* int_ptr (long int value);
       char* literal_to_auto_string (literal_value* lit, auto_string* str);
       char* map_to_auto_string (hash_table* ht, auto_string* str);
       char* parseable_buffer_from_string (char* str, size_t* buffer_size);
       auto_string* serc_document_to_auto_string (hash_table* document);
       hash_table* serc_parse_buffer (char* buffer, size_t buffer_size);
       char* serc_read_file (char* fname, size_t* buffer_size);
       char* set_to_auto_string (set* s, auto_string* str);
       storage_unit* storage_unit_named_float (long double value, char* name);
       storage_unit* storage_unit_named_integer (long int value, char* name);
       storage_unit* storage_unit_ptr (stored_type type, void* value);
       char* str_ptr (char* value);
       char* str_return_value (char* str);

       Link with -lserc -lsscont.


DESCRIPTION
       Libserc  is  the reference library for the Serc data serialization lan-
       guage. Its functions allow the programmer to parse or serialize C  data
       types. For information on Serc see http://www.softsprocket.com/


       The enumerations and types used by libserc:

       typedef enum {
               LV_STRING,
               LV_INTEGER,
               LV_FLOAT,
               LV_HEREDOC,
               LV_CHARACTER,
               LV_ARRAY
       } literal_value_type;

       typedef struct {
               literal_value_type type;
               void* value;
       } literal_value;

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

       typedef struct {
               id_tuple_type type;
               char* id;
               void* data;
       } id_tuple;

       typedef enum {
               ST_ID_TUPLE,
               ST_LITERAL_VALUE,
               ST_SUBDOC,
               ST_SET
       } stored_type;

       typedef struct {
               stored_type type;
               union {
                       id_tuple* tuple;
                       literal_value* literal;
                       hash_table* map;
                       set* s;
               };
       } storage_unit;


       char* array_to_auto_string (auto_array* aa, auto_string* str);
           converts  a Serc Array, stored as an auto_array, to an auto_string.
           auto_string is from libsscont.
           aa - the auto_array to convert
           str - a pointer to the auto_string that will contain the  converted
           array
           returns  -  a  pointer  to  the  nul  terminated buf element of the
           auto_string or NULL in the event of an error

       set* array_to_set (auto_array* aa);
           creates a set on the heap from the data of an auto_array  of  stor-
           age_units.   and  deletes the auto_array. A Serc Set is represented
           by a set.  set is from libsscont
           aa - an auto_array of storage_unit pointers
           returns - a pointer to a set or NULL in the event of an error

       char* char_ptr (char value);
            allocates a char on the heap and assigns a value to it.
            value - the value to assign
            returns -  a pointer to a char or NULL in the event of an error

       auto_array* create_array (void* value);
           creates an auto_array on the heap and adds a value to  it.  A  Serc
           Array or Set can be represented by an auto_array. In both cases the
           object stored should be a storage_unit.  auto_array is from  libss-
           cont
           value  -  a  pointer to a value to store or NULL in the event of an
           error
           a - pointer to an auto_array

       id_tuple* create_id_tuple (char* id, void* data, id_tuple_type type);
           creates a id_tuple object on the heap.
           id - the name of the tuple
           data - a pointer to the value of the tuple
           type - the type of the object
           returns - a pointer to the id_tuple or NULL  in  the  event  of  an
           error

       literal_value*  create_literal_value  (literal_value_type  type,  void*
       data);
           creates a literal_value object on the heap.
           type - the type of the object
           data - a pointer to the value of the literal
           returns - a pointer to the literal_value or NULL in the event of an
           error

       hash_table* create_map (char* key, storage_unit* value);
           creates  a  hash_table on the heap and stores a value in it.  It is
           expected  that  the  storage_unit  contains  an  id_tuple  and  its
           namewill be the key.  hash_table is from libsscont
           key - the key to store value under
           value - the value to be stored
           returns  -   a  pointer  to a hash_table or NULL in the event of an
           error

       auto_array* create_sequence (hash_table* value);
           creates an auto_array on the heap and adds a hash_table pointer  to
           it.  A Serc sequence is reprsented by an auto_array. The hash_table
           represents a Serc document or subdocument.  auto_array is from lib-
           sscont
           value - a hash_table to store
           returns  -  a  pointer  to an auto_array or NULL in the event of an
           error

       void delete_document (void* v);
           free the memory from a Serc document  (hash_table  of  storage_unit
           types).   Parameter is a void* to match libsscont hash_table_delete
           function pointer specification but a hash_table* is expcted.
           v - the document to free

       void delete_id_tuple (void* v);
           free the memory from a Serc id_tuple.  Parameter is void*  for  use
           as  function pointer to libsscont delete functions but an id_tuple*
           is the expected type.
           v - the id_tuple to free

       void delete_literal_value (void* v);
           free the memory from a Serc literal_value.  Parameter is void*  for
           use  as  function  pointer to libsscont delete functions but a lit-
           eral_value* is the expected type.
           v - the storage_unit to free

       void delete_storage_unit (void* v);
           free the memory from a Serc storage_unit.  Parameter is  void*  for
           use  as  function pointer to libsscont delete functions but a stor-
           age_unit* is the expected type.
           v - the storage_unit to free

       char* document_to_auto_string (hash_table* document, auto_string* str);
            converts  a  serc  document  stored  as  a  hash_table   into   an
           auto_string.
            hash_table and auto_string are from libsscont
            document - a hash_table containing the serc document elements
            str  -  a  pointer  to an auto_str that will contain the converted
           document
            returns - a pointer to the  nul  terminated  buf  element  of  the
           auto_string or NULL in the event of an error

       long double* double_ptr (long double value);
           allocates a long double on the heap and assigns a value to it.
           value - the value to assign
           returns  -  a  pointer  to a long double or NULL in the event of an
           error

       char* id_tuple_to_auto_string (id_tuple* it, auto_string* str);
           converts a Serc id_tuple to an auto_string.
           it - the id_tuple to convert
           str - a pointer to the auto_string that will contain the  converted
           id_tuple
           returns  -  a  pointer  to  the  nul  terminated buf element of the
           auto_string or NULL in the event of an error

       long int* int_ptr (long int value);
           allocates a long int on the heap and assigns a value to it.
           value - the value to assign
           returns - a pointer to a long int or NULL in the event of an error

       char* literal_to_auto_string (literal_value* lit, auto_string* str);
           converts a Serc literal_value to an auto_string.
           lit - the literal_value to convert
           str - a pointer to the auto_string that will contain the  converted
           literal_value
           returns  -  a  pointer  to  the  nul  terminated buf element of the
           auto_string or NULL in the event of an error

       char* map_to_auto_string (hash_table* ht, auto_string* str);
           converts a hash_table representing a Serc map to an auto_string.
           ht - the hash_table to convert
           str - a pointer to the auto_string that will contain the  converted
           map
           returns  -  a  pointer  to  the  nul  terminated buf element of the
           auto_string or NULL in the event of an error

       char* parseable_buffer_from_string (char* str, size_t* buffer_size);
           convert a nul terminated c string into a  double  terminated  parse
           buffer.  Makes a copy of the string
           str - a c string containing the Serc data
           buffer_size - an int pointer for the return buffer size
           returns - the parseable buffer or NULL in the event of an error

       auto_string* serc_document_to_auto_string (hash_table* document);
           convenience  function  -  creates  an  auto_string  and calls docu-
           ment_to_auto_string.  auto_string and hash_table are from libsscont
           document - the Serc document stored as a hash_table
           returns - a pointer to an auto_string containing  the  document  or
           NULL in the eventof an error

       hash_table* serc_parse_buffer (char* buffer, size_t buffer_size);
           parses  a  Serc  document  into a form useable by a C program.  The
           buffer must terminated with two nul bytes.  hash_table is from lib-
           sscont
           buffer -  the string to parse
           buffer_size - the size of the buffer
           returns - a pointer to hash_table representing the Serc document or
           NULL in the event of an error

       char* serc_read_file (char* fname, size_t* buffer_size);
           reads a file and converts it to a buffer suitable  for  Serc  parse
           buffer
           fname - the path of the file to read
           point  - to an int which will be populated with the returned buffer
           size
           returns - buffer containing the Serc document to be parsed or  NULL
           in the eventof an error

       char* set_to_auto_string (set* s, auto_string* str);
           converts a Serc set, stored as an libsscont set, to an auto_string.
           s - the set to convert
           str  - a pointer to the auto_string that will contain the converted
           set
           returns - a pointer to  the  nul  terminated  buf  element  of  the
           auto_string or NULL in the event of an error

       storage_unit* storage_unit_named_float (long double value, char* name);
           creates  a  storage_unit  of  type  ST_ID_TUPLE  that  contains  an
           id_tuple* of type TT_FLOAT. A convenience function  that  allocates
           the  memory  required  for the id, name and the tuple and makes the
           appropriate assignments.  A copy is made of the string name.
           value - the value to assign the id_tuple
           name - the name to assign the tuple
           returns - a storage_unit* or NULL if an error occurs

       storage_unit* storage_unit_named_integer (long int value, char* name);
           creates  a  storage_unit  of  type  ST_ID_TUPLE  that  contains  an
           id_tuple* of type TT_INTEGER. A convenience function that allocates
           the memory required for the id, name and the tuple  and  makes  the
           appropriate assignments.  A copy is made of the string name.
           value - the value to assign the id_tuple
           name - the name to assign the tuple
           returns - a storage_unit* or NULL if an error occurs

       storage_unit* storage_unit_named_char (char value, char* name);
           creates  a  storage_unit  of  type  ST_ID_TUPLE  that  contains  an
           id_tuple* of type TT_CHARACTER. A convenience function  that  allo-
           cates  the memory required for the id, name and the tuple and makes
           the appropriate assignments.  A copy is made of the string name.
           value - the value to assign the id_tuple
           name - the name to assign the tuple
           returns - a storage_unit* or NULL if an error occurs

       storage_unit* storage_unit_named_string (char* value, char* name);
           creates  a  storage_unit  of  type  ST_ID_TUPLE  that  contains  an
           id_tuple*  of type TT_STRING. A convenience function that allocates
           the memory required for the id, and the tuple and makes the  appro-
           priate  assignments.  A copy is made of the string value. A copy is
           made of the string name.
           value - the value to assign the id_tuple
           name - the name to assign the tuple
           returns - a storage_unit* or NULL if an error occurs

       storage_unit* storage_unit_named_heredoc (char* value, char* name);
           creates  a  storage_unit  of  type  ST_ID_TUPLE  that  contains  an
           id_tuple* of type TT_HEREDOC. A convenience function that allocates
           the memory required for the id, name and the tuple  and  makes  the
           appropriate assignments.  The char* value should be on the heap (no
           copy is made). A copy is made of the string name.
           value - the value to assign the id_tuple
           name - the name to assign the tuple
           returns - a storage_unit* or NULL if an error occurs

       storage_unit* storage_unit_named_array (auto_array* value, char* name);
           creates  a  storage_unit  of  type  ST_ID_TUPLE  that  contains  an
           id_tuple*  of  type TT_ARRAY. A convenience function that allocates
           the memory required for the id, name and the tuple  and  makes  the
           appropriate  assignments.   The  auto_array* value should be on the
           heap (no copy is made). A copy is made of the string name.
           value - the value to assign the id_tuple
           name - the name to assign the tuple
           returns - a storage_unit* or NULL if an error occurs

       storage_unit* storage_unit_named_set (set* value, char* name);
           creates  a  storage_unit  of  type  ST_ID_TUPLE  that  contains  an
           id_tuple* of type TT_SET. A convenience function that allocates the
           memory required for the id, name and the tuple and makes the appro-
           priate  assignments.  The set* value should be on the heap (no copy
           is made). A copy is made of the string name.
           value - the value to assign the id_tuple
           name - the name to assign the tuple
           returns - a storage_unit* or NULL if an error occurs

       storage_unit*  storage_unit_named_sequence  (auto_array*  value,  char*
       name);
           creates  a  storage_unit  of  type  ST_ID_TUPLE  that  contains  an
           id_tuple* of type TT_SEQUENCE. A convenience  function  that  allo-
           cates  the memory required for the id, name and the tuple and makes
           the appropriate assignments.  The auto_array* value  should  be  on
           the heap (no copy is made). A copy is made of the string name.
           value - the value to assign the id_tuple
           name - the name to assign the tuple
           returns - a storage_unit* or NULL if an error occurs

       storage_unit* storage_unit_named_map (hash_table* value, char* name);
           creates  a  storage_unit  of  type  ST_ID_TUPLE  that  contains  an
           id_tuple* of type TT_MAP. A convenience function that allocates the
           memory required for the id, name and the tuple and makes the appro-
           priate assignments.  The hash_table* value should be  on  the  heap
           (no copy is made). A copy is made of the string name.
           value - the value to assign the id_tuple
           name - the name to assign the tuple
           returns - a storage_unit* or NULL if an error occurs

       storage_unit* storage_unit_ptr (stored_type type, void* value);
           allocates  a  storage_unit on the heap and assigns a value and type
           to it.
           type - the type to assign
           value - the value to assign
           returns - a pointer to a storage_unit or NULL in the  event  of  an
           error

       char* str_ptr (char* value);
           allocates a char* on the heap and coppies a value to it.
           value - the value to assign
           returns - a char* or NULL in the event of an error

       char* str_return_value (char* str);
           copies a string removing quotations from the front and back if they
           exist.
           str - the string to copy or NULL in the event of an error
           returns - a string

