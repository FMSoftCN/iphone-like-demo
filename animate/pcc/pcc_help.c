
#include "../p-code.h"
#include "pcc.h"

static const char* defIns[]={
	"nop",
	"iconst",
	"iload",
	"iload_ptr",
	"isave",
	"isave_ptr",
	"nullpush",
	"pop",
	"ret",
	"retv",
	"jmp",
	"iadd",
	"isub",
	"imul",
	"idiv",
	"imod",
	"intr",
	"call",
	"ncall",
	"cmp",
	"jz",
	"jl",
	"jg"
};

static char** extend_ins = NULL;
static int extend_ins_count = 0;

static method_head_t* mh_method = NULL;
static int mh_method_count = 0;

static method_head_t* mh_native_method = NULL;
static int mh_native_method_count = 0;

static ins_t cur_ins;

typedef struct _method_list_t{
	list_t list;
	char * name;
	PCODE_METHOD *method;
}method_list_t;

static method_list_t* methods = NULL;
static int methods_count = 0;

typedef struct _local_var_list_t{
	list_t list;
	char *sym;
}local_var_list_t;

static local_var_list_t * local_vars = NULL;
static int local_var_count = 0;
static int is_args = 1;

typedef struct _jmp_addr_list_t{
	list_t list;
	char *label;
	int addr;
}jmp_addr_list_t;

static jmp_addr_list_t* jmp_addr_list;
static int jmp_addr_list_count = 0;

static int find_index_of_ins(const char* ins, const char** ins_list, int count)
{
	int i;
	for(i=0;i<count;i++)
		if(strcmp(ins_list[i], ins) == 0)
			return i;

	return -1;
}

int trans_num(const char* yytext, int len)
{
	int scale=10;
	int i;
	int r = 0;
	if(yytext == NULL)
		return 0;
	
	switch(yytext[len-1])
	{
	case 'D': case 'd':
		len --;
		scale = 10; break;
	case 'O': case 'o':
		len --;
		scale = 8; break;
	case 'B': case 'b':
		len --;
		scale = 2; break;
	case 'H': case 'h':
		len --;
		scale = 16; break;
	}

	for(i=0; i<len; i++)
	{
		r = r*scale + (yytext[i]-'0');
	}

	return r;	
}

void add_to_extend_ids(char * sym)
{
	extend_ins_count ++;
	if(extend_ins == NULL)
	{
		extend_ins = (char**)malloc(sizeof(char*));
	}
	else{
		extend_ins = (char**)realloc(extend_ins, extend_ins_count*sizeof(char*));
	}
	extend_ins[extend_ins_count-1] = strdup(sym);
}

list_t* add_to_list(list_t *list, void* element)
{
	list_t* lstElement = (list_t*)element;
	if(element == NULL)
		return NULL;

	lstElement->next = list;
	return lstElement;
	
}

method_head_t * new_method_head(char* sym, int argc)
{
	method_head_t * mh = (method_head_t*)malloc(sizeof(method_head_t));
	mh->argc = argc == -1?local_var_count:argc;
	mh->name = sym;
	if(argc == -1){
		add_local_var(NULL);
		add_local_var(NULL);
		add_local_var(NULL);
	}
	is_args = 0;
	return mh;
}

void set_method_list(list_t *list)
{
	mh_method = list;
	mh_method_count = 0;
	while(list){
		mh_method_count ++;
		list = list->next;
	}
	is_args = 1;
}

void set_native_method_list(list_t *list)
{
	mh_native_method = list;
	mh_native_method_count = 0;
	while(list){
		mh_native_method_count ++;
		list = list->next;
	}
	is_args = 1;
}

void add_new_method(method_head_t* mh )
{
	method_list_t *method_list = (method_list_t*)malloc(sizeof(method_list_t));

	method_list->method = (PCODE_METHOD*)malloc(sizeof(PCODE_METHOD));
	method_list->method->argc = mh->argc;
	method_list->name = mh->name;
	method_list->method->code_len = cur_ins.ins_len;

	if(cur_ins.buf_len != cur_ins.ins_len)
	{
		method_list->method->codes = (unsigned char*)realloc(cur_ins.buf,cur_ins.ins_len);
	}
	else
	{
		method_list->method->codes = cur_ins.buf;
	}

	cur_ins.buf = NULL;
	cur_ins.ins_len = cur_ins.buf_len = 0;

	method_list->list.next = methods;
	methods = method_list;
	methods_count ++;

	is_args = 1;

	//insert into the method head list
	mh->list.next =  mh_method;
	mh_method = mh;
	mh_method_count ++;


	//delete local vars
	while(local_vars){
		local_var_list_t* vl = local_vars;
		local_vars = local_vars->list.next;
		if(vl->sym)
			free(vl->sym);
		free(vl);
	}
	local_var_count = 0;
	//delete jmp addrs
	while(jmp_addr_list){
		jmp_addr_list_t * jal = jmp_addr_list;
		jmp_addr_list = jmp_addr_list->list.next;
		free(jal->label);
		free(jal);
	}
	jmp_addr_list_count = 0;

}

void add_local_var(char *sym)
{
	local_var_list_t * vl = (local_var_list_t*)malloc(sizeof(local_var_list_t));

	vl->sym = sym;
	vl->list.next = local_vars;
	local_vars = vl;
	local_var_count ++;
	if(!is_args)
		add_cur_ins("nullpush", 0,TYPE_NONE);
}

static unsigned char* get_ptr(int size)
{
	int len;
	if(cur_ins.buf==NULL){
		cur_ins.buf = (char*) malloc(128);
	}
	else{
		if( (cur_ins.ins_len + size) >= cur_ins.buf_len)
		{
			cur_ins.buf_len += 128;
			cur_ins.buf = (char*)realloc(cur_ins.buf, cur_ins.buf_len);
		}
	}
	len = cur_ins.ins_len;
	cur_ins.ins_len += size;
	return cur_ins.buf + len;
}

unsigned short get_method_head_index(char * sym, method_head_t* mh, int count)
{
	int idx = 0;
	if(mh == NULL)
		return -1;
	while(mh)
	{
		if( strcmp(mh->name, sym) == 0)
			return count -1 - idx;
		idx ++;
		mh = mh->list.next;
	}
	return -1;
}

unsigned short get_jmp_addr(char * label)
{
	jmp_addr_list_t * jal = jmp_addr_list;
	int idx;
	while(jal)
	{
		if( strcmp(jal->label, label) == 0)
			return (unsigned short)jal->addr;
		idx ++;
		jal = jal->list.next;
	}
	return -1;
}



unsigned short get_local_var(char* var)
{	
	int idx = 0;
	local_var_list_t* lvl = local_vars;
	while(lvl){
		if(lvl->sym && strcmp(lvl->sym, var) == 0){
			return local_var_count - 1 - idx;
		}
		idx ++;
		lvl = lvl->list.next;
	}

	//try treat as jmp label
	idx = get_jmp_addr(var);
	if(idx != 0xFFFF){
		return idx;
	}

	//try get the method
	idx = get_method_head_index(var, mh_method, mh_method_count);
	if(idx == 0xFFFF)
		idx = get_method_head_index(var, mh_native_method, mh_native_method_count);

	return idx;
}

void add_cur_ins(char* ins, int param, int type)
{
	int insidx = -1;
	unsigned char* p;
	//find the ins
	insidx = find_index_of_ins(ins, defIns, sizeof(defIns)/sizeof(char*));
	if(insidx == -1){
		insidx = find_index_of_ins(ins, extend_ins, extend_ins_count);
		
		if(insidx == -1)
		{
			fprintf(stderr, "Invalidate Ins: %s\n", ins);
			return ;
		}
		insidx += max_pre_def_ins;
	}

	switch(type)
	{
	case TYPE_NONE:
		p = get_ptr(1);
		*p = (unsigned char)insidx;
		p ++;
		break;
	case TYPE_NUM:
		p = get_ptr(1+sizeof(int));
		*p = (unsigned char)insidx;
		p ++;
		*(unsigned int*)p = (unsigned int)param;
		break;
	case TYPE_ID:
		p = get_ptr(1+sizeof(short));
		*p = (unsigned char)insidx;
		p ++;
		*(unsigned short*)p = get_local_var((char*)param);
		free((char*)param);
		break;
	}
	
}

void set_jmp_addr(char* label)
{
	jmp_addr_list_t * lst = (jmp_addr_list_t*)malloc(sizeof(jmp_addr_list_t));
	lst->label = label;
	lst->addr = cur_ins.ins_len;
	lst->list.next = jmp_addr_list;
	jmp_addr_list = lst;
	jmp_addr_list_count ++;
}

void clear_all_data()
{
	if(extend_ins)
		free(extend_ins);
	
	while(mh_method){
		method_head_t *mh = mh_method;
		mh_method = mh_method->list.next;
		free(mh->name);
		free(mh);
	}

	while(mh_native_method){
		method_head_t *mh = mh_native_method;
		mh_native_method = mh_native_method->list.next;
		free(mh->name);
		free(mh);
	}
	
	while(methods)
	{
		method_list_t* ml = methods;
		methods = methods->list.next;
		free(ml->name);
		free(ml->method->codes);
		free(ml->method);
		free(ml);
	}

}

void save_to_file(FILE* f)
{
	method_list_t * ml = methods;
	fwrite(&methods_count, 1, sizeof(int),f);
	while(ml)
	{
		fwrite(ml->name, 1, strlen(ml->name)+1, f);
		fwrite(&ml->method->argc, 1, sizeof(int), f);
		fwrite(&ml->method->code_len,1, sizeof(int),f);
		fwrite(ml->method->codes, 1, ml->method->code_len, f);
		ml = ml->list.next;
	}
}

void fprint_binery(FILE* f, const char* pre, unsigned char* bin, int len)
{
	int i = 0;
	fprintf(f,"\n%s",pre);
	fprintf(f,"0x%02x",(unsigned int)bin[0]);
	i = 1;
	while(i<len){
		if(i%12 == 0){
			fprintf(f,"\n%s",pre);
		}
		fprintf(f,",0x%02x",(unsigned int)bin[i]);
		i ++;
	}
	if(i%12 != 0)
		fprintf(f,"\n");
}

void save_to_cfile(FILE* f)
{
	method_list_t * ml = methods;
	fprintf(f, "#include \"p-code.h\"\n\n");
	while(ml)
	{
		fprintf(f,"//pcode method : %s\n", ml->name);
		fprintf(f,"PCODE_METHOD %s = { \n", ml->name);
		fprintf(f,"\t%d,//argc\n", ml->method->argc);
		fprintf(f,"\t{");
		fprint_binery(f,"\t\t", ml->method->codes, ml->method->code_len);
		fprintf(f,"\t},\n");
		fprintf(f,"\t%d //code len\n",ml->method->code_len);
		fprintf(f,"};\n");
		ml = ml->list.next;
	}
}

