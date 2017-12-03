#ifndef PCC_H
#define PCC_H

#include <stdio.h>
#include <stdlib.h>

typedef struct _list_t{
	struct _list_t *next;
}list_t;

typedef struct _ins_t{
	int ins_len;
	int buf_len;
	unsigned char* buf;
}ins_t;

typedef struct _method_head_t{
	list_t list;
	int argc;
	char* name;
}method_head_t;

int trans_num(const char* yytext, int len);

void add_to_extend_ids(char * sym);

list_t* add_to_list(list_t *list, void* element);

method_head_t * new_method_head(char* sym, int argc);

void set_method_list(list_t *list);

void set_native_method_list(list_t *list);

void add_new_method(method_head_t* mh );

void add_local_var(char *sym);

#define TYPE_NONE 0 
#define TYPE_NUM 1
#define TYPE_ID  2
#define TYPE_LABEL 3
void add_cur_ins(char* ins, int param, int type);

void set_jmp_addr(char * label);

void save_to_file(FILE* f);


void save_to_cfile(FILE* f);

void clear_all_data();

#endif
