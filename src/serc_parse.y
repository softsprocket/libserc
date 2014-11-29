
%{
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
#include <softsprocket/container.h>
#include <softsprocket/debug_utils.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
/*
#define YYERROR_VERBOSE 1
*/

extern int yylex();


void yyerror (hash_table** the_results, yyscan_t scanner, char *s) {
	fprintf (stderr, "error: %s\n", s);
}


%}

%code requires {
 
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
 
}

%output "serc_parse.c"
%defines "../include/serc_parse.h"

%define api.pure 
%lex-param   { yyscan_t scanner }
%parse-param { hash_table** the_results }
%parse-param { yyscan_t scanner }

%union {
	char* str;
	long int i;
	long double f;
	char c;
	literal_value* lv;
	id_tuple* it;
	auto_array* aa;
	hash_table* map;
}

%token MAP_TYPE
%token ARRAY_TYPE
%token INTEGER_TYPE
%token SET_TYPE
%token STRING_TYPE
%token SEQUENCE_TYPE
%token FLOAT_TYPE
%token CHARACTER_TYPE

%token <str> HEREDOC_VAL
%token <str> IDENTIFIER 
%token <str> TYPE
%token <i> INTEGER
%token <str> STRING
%token <f> FLOAT
%token <c> CHARACTER

%type <lv> literal numeric
%type <it> string integer float character array set sequence map typed_data
%type <aa> array_data set_data array_data_array sequence_data set_data_set
%type <map> mapped_data subdocuments document

%%

document: '{' subdocuments '}'  	{ $$ = $2; *the_results = $$; }
	;

subdocuments: typed_data		{ $$ = create_map ($1->id, storage_unit_ptr (ST_ID_TUPLE, $1)); } 
	| subdocuments typed_data	{ hash_table_put ($1, $2->id, storage_unit_ptr (ST_ID_TUPLE, $2)); $$ = $1; } 
	;

typed_data: map		{ $$ = $1; } 
	| sequence	{ $$ = $1; }	
	| set		{ $$ = $1; }
	| array 	{ $$ = $1; }
	| string	{ $$ = $1; }
	| integer	{ $$ = $1; }
	| float		{ $$ = $1; }
	| character 	{ $$ = $1; }
	;


map: MAP_TYPE IDENTIFIER '=' '{' mapped_data '}' { $$ = create_id_tuple ($2, $5, TT_MAP); }
	;

mapped_data: IDENTIFIER ':' literal 				{ $$ = create_map ($1, storage_unit_ptr (ST_LITERAL_VALUE, $3)); }
	| IDENTIFIER ':' '{' subdocuments '}'			{ $$ = create_map ($1, storage_unit_ptr (ST_SUBDOC, $4)); }
	| mapped_data ',' IDENTIFIER ':' literal		{ hash_table_put ($1, $3, storage_unit_ptr (ST_LITERAL_VALUE, $5)); $$ = $1; free ($3); }
	| mapped_data ',' IDENTIFIER ':' '{' subdocuments '}'   { hash_table_put ($1, $3, storage_unit_ptr (ST_SUBDOC, $6)); $$ = $1; free ($3); }
	;

sequence:  SEQUENCE_TYPE IDENTIFIER '=' '{' sequence_data '}' { $$ = create_id_tuple ($2, $5, TT_SEQUENCE); } 
	;

sequence_data: '{' subdocuments '}'			{ $$ = create_sequence ($2); }
	| sequence_data ',' '{' subdocuments '}' 	{ auto_array_add ($1, $4); $$ = $1; }
	;

set: SET_TYPE IDENTIFIER '=' '(' set_data ')'  		{ $$ = create_id_tuple ($2, array_to_set ($5), TT_SET); }
   	| SET_TYPE IDENTIFIER '=' '(' set_data_set ')'  { $$ = create_id_tuple ($2, array_to_set ($5), TT_SET); }
	;

set_data_set: '(' set_data ')'				{ $$ = create_array (storage_unit_ptr (ST_SET, array_to_set ($2))); }			
	| set_data_set ',' '(' set_data ')'		{ auto_array_add ($1, storage_unit_ptr (ST_SET, array_to_set ($4))); $$ = $1; }
	;

set_data: numeric			{ $$ = create_array (storage_unit_ptr (ST_LITERAL_VALUE, $1)); } 
	| set_data ',' numeric		{ auto_array_add ($1, storage_unit_ptr (ST_LITERAL_VALUE, $3)); $$ = $1; }
	;

array: ARRAY_TYPE IDENTIFIER '=' '[' array_data ']'  		{ $$ = create_id_tuple ($2, $5, TT_ARRAY); }
	| ARRAY_TYPE IDENTIFIER '=' '[' array_data_array ']'  	{ $$ = create_id_tuple ($2, $5, TT_ARRAY); }
	;

array_data_array: '[' array_data ']'			{ $$ = create_array (create_literal_value (LV_ARRAY, $2)); }
	| array_data_array ',' '[' array_data ']'	{ auto_array_add ($1, create_literal_value (LV_ARRAY, $4)); $$ = $1; }

array_data: literal 				{ $$ = create_array ($1); }
	| array_data ',' literal		{ auto_array_add ($1, $3); $$ = $1; }	
	;


string: STRING_TYPE IDENTIFIER '=' STRING 		{ $$ = create_id_tuple ($2, $4, TT_STRING); }
      	| STRING_TYPE IDENTIFIER '=' HEREDOC_VAL 	{ $$ = create_id_tuple ($2, $4, TT_HEREDOC); }
      	;

integer: INTEGER_TYPE IDENTIFIER '=' INTEGER  { $$ = create_id_tuple ($2, int_ptr ($4), TT_INTEGER); }
       	;

float: FLOAT_TYPE IDENTIFIER '=' FLOAT  	{ $$ = create_id_tuple ($2, double_ptr ($4), TT_FLOAT); }
     	| FLOAT_TYPE IDENTIFIER '=' INTEGER 	{ $$ = create_id_tuple ($2, double_ptr ($4), TT_FLOAT); }
     	;

character: CHARACTER_TYPE IDENTIFIER '=' CHARACTER  { $$ = create_id_tuple ($2, char_ptr ($4), TT_CHARACTER); }
	;

literal: STRING 	{ $$ = create_literal_value (LV_STRING, $1); }
	| CHARACTER	{ $$ = create_literal_value (LV_CHARACTER, char_ptr ($1)); }
	| HEREDOC_VAL   { $$ = create_literal_value (LV_HEREDOC, $1); } 
	| numeric	{ $$ = $1; }
	;

numeric: INTEGER        { $$ = create_literal_value (LV_INTEGER, int_ptr ($1)); }
       	| FLOAT         { $$ = create_literal_value (LV_FLOAT, double_ptr ($1)); }
	;

%%



