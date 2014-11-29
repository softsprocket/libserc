
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
#include "serc_parse.h"
#include <softsprocket/container.h>
#include <softsprocket/debug_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t here_doc_size = 0;
char* here_doc_str = NULL;

extern int fileno (FILE*);
#define YY_DECL int yylex (YYSTYPE* yylval_param, yyscan_t yyscanner)

%}

%option outfile="serc_scan.c" header-file="../include/serc_lexer.h"
%option warn nodefault
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

%x	HEREDOC
%x	COMMENT

/* Universal Character Name */
UCN (\\u[0-9a-fA-F]{4}|\\U[0-9a-fA-F]{8})
/* float exponent */
EXP ([Ee][-+]?[0-9]+)
/* integer length */
ILEN ([Uu](L|l|LL|ll)?|(L|l|LL|ll)[Uu]?)


%%
\`\`\`										{ BEGIN (HEREDOC); }

<HEREDOC>\`\`\`									{ 
											BEGIN (INITIAL); 
											yylval->str = here_doc_str;
											here_doc_size = 0;
											return HEREDOC_VAL;
										}

<HEREDOC>([^`]|\n)+|.								{ 
											if (here_doc_size == 0) {
												here_doc_size = strlen (yytext) + 1;
												here_doc_str = malloc (here_doc_size);
												if (here_doc_str == NULL) {
													perror ("malloc");
													exit (EXIT_FAILURE);
												}
												
												strcpy (here_doc_str, yytext);
											} else {
												int new_size = here_doc_size + strlen (yytext);
												void* pstr = realloc (here_doc_str, new_size);
												if (pstr == NULL) {
													perror ("realloc");
													exit (EXIT_FAILURE);
												}
												
												here_doc_str = pstr;
												strcpy (here_doc_str + (here_doc_size - 1), yytext); 
											}
										}

<HEREDOC><<EOF>>								{ 
											printf ("%d: Unterminated heredoc\n", yylineno); 
											return 0; 
										}

\/\*										{ BEGIN (COMMENT); }

<COMMENT>\*\/									{ BEGIN (INITIAL); }

<COMMENT>([^\*]|\n)+|.								{ /* Do nothing */ }

<COMMENT><<EOF>>								{ 
											printf ("%d: Unterminated comment\n", yylineno); 
											return 0; 
										}

;										{ /* return yytext[0];*/  }

:										{ return yytext[0]; }

\{										{ return yytext[0]; }

\}										{ return yytext[0]; }

\[										{ return yytext[0]; }

\]										{ return yytext[0]; }

=										{ return yytext[0]; }


\(										{ return yytext[0]; }

\)										{ return yytext[0]; }

0[0-7]*{ILEN}?									{ 
											yylval->f = strtol (yytext, NULL, 8); 
											return INTEGER; 
										}

[-+]?[1-9][0-9]*{ILEN}?								{ 
											yylval->i = strtol (yytext, NULL, 0); 
											return INTEGER; 
										}

0[Xx][0-9a-fA-F]+{ILEN}?							{ 
											yylval->i = strtol (yytext, NULL, 16); 
											return INTEGER; 
										}
	
inf|nan										{				 
											yylval->f = strtold (yytext, NULL); 
											return FLOAT; 
										}

([0-9]*\.[0-9]+|[0-9]+\.){EXP}?[flFL]?						{ 
											yylval->f = strtold (yytext, NULL); 
											return FLOAT; 
										}

[0-9]+{EXP}[flFL]?								{ 
											yylval->f = strtold (yytext, NULL); 
											return FLOAT; 
										}

0[Xx]([0-9a-fA-F]*\.[0-9a-fA-F]+|[0-9a-fA-F]+\.?)[Pp][-+]?[0-9]+[flFL]? 	{ 
											yylval->f = strtold (yytext, NULL); 
											return FLOAT; 
										}

Integer										{ return INTEGER_TYPE; } 

Float										{ return FLOAT_TYPE; }

String										{ return STRING_TYPE; }

Map										{ return MAP_TYPE; }

Set										{ return SET_TYPE; }

Sequence									{ return SEQUENCE_TYPE; }

Array										{ return ARRAY_TYPE; }

Character									{ return CHARACTER_TYPE; }

\"										{ return yytext[0]; }
 
'										{ return yytext[0]; }

,										{ return yytext[0]; }

([_a-zA-Z]|{UCN})([_a-zA-Z0-9]|{UCN})* 						{ 
											yylval->str = str_return_value (yytext); 
 
											if (yylval->str == 0) {
												return 0;
											}
											return IDENTIFIER; 
										}

\'([^'\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+|{UCN})+\'		{
											yylval->c = yytext[1]; 
											return CHARACTER; 
										}

L?\"([^"\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+|{UCN})*\"		{ 
											yylval->str = str_return_value (yytext); 
											if (yylval->str == 0) {
												return 0;
											}
											return STRING; 
										}

[\n\t ]										{/* Do nothing */ }

.										{ 
											printf ("%d: Unhandled Character %s\n", yylineno, yytext); 
										}
%%


