%{
#include <stdio.h>
#include <stdlib.h>
#include "pcc.h"
%}

%union{
	int number;
	char *sym;
	list_t *list;
	ins_t * ins;
	method_head_t *mht;
}

%token<sym> ID
%token<number> NUM
%token INS  DECLEAR END NATIVE METHOD VAR

%type<mht> msection_head method_list
%type<list> method_lists

%%

sections:
	sections section
	| section
	;

section:
	declear_ins_section 
	| declear_method_section
	| declear_native_section
	| method_section
	;

declear_ins_section:
	DECLEAR INS '\n' decins_list END '\n'
	;

decins_list:
	decins_list decins
	| decins
	;

decins:
	ID '\n' 	{ add_to_extend_ids($1); }
	;

declear_method_section:
	DECLEAR METHOD '\n' method_lists END '\n' { set_method_list($4); }
	;

declear_native_section:
	DECLEAR NATIVE METHOD '\n' method_lists END '\n' { set_native_method_list($5); }
	;

method_lists:
	method_lists method_list { $$ = add_to_list($1,$2); }
	| method_list			 { $$ = add_to_list(NULL, $1); }
	;

method_list:
	ID ':' NUM '\n'  { $$ = new_method_head($1, $3); }
	;

method_section:
	msection_head local_var_table ins_list END '\n' { add_new_method($1); }
	;

msection_head:
	METHOD ID '\n'			{ $$ = new_method_head($2,-1); }
	| METHOD ID ':'  var_list '\n'	{ $$ = new_method_head($2,-1); }
	;

local_var_table:

	| local_var_table decvar_list
	| decvar_list
	;

decvar_list:
	VAR var_list  '\n'
	;

var_list:
	var_list ',' ID { add_local_var($3); }
	| ID			{ add_local_var($1); }
	;

ins_list:
	ins_list ins
	| ins
	;

ins:
	ID '\n'				{ add_cur_ins($1, 0, TYPE_NONE); }
	| ID NUM '\n'		{ add_cur_ins($1, $2, TYPE_NUM); }
	| ID ID '\n'		{ add_cur_ins($1, (int)$2, TYPE_ID); }
	| jmp_addr
	;

jmp_addr:
	ID ':' '\n'		{ set_jmp_addr($1); }
	;

%%

extern int yylineno;
void yyerror(char* s)
{
	fprintf(stderr, "error:%d: %s\n",yylineno+1,s);
}

int main(int argc, char* argv[])
{
	FILE *f;
	yyparse();
	save_to_cfile(stdout);
	f = fopen("tmp.pcc","wb");
	save_to_file(f);
	fclose(f);
	clear_all_data();
	return 0;
}
