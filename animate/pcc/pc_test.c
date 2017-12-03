#include <stdio.h>
#include <stdlib.h>
#include "pcc.h"
#include "../p-code.h"

static int get_int_value(int *param, int param_count)
{
	int v;
	scanf("%d",&v);
	return v;
}

static int print_int_value(int* param, int param_count)
{
	printf("%d\n",*param);
	return 0;
}

static PCODE_NATIVE_METHOD native_methods[] = {
	{get_int_value, 0},
	{print_int_value, 1}
};

static PCODE_NATIVE_METHOD_TABLE native_table={
	native_methods,
	sizeof(native_methods)/sizeof(PCODE_NATIVE_METHOD)
};

static PCODE_METHOD_TABLE method_table;

static PCODE pcode = { NULL, &method_table, &native_table, NULL, NULL };

PCODE_METHOD *pmain;

static void get_methods(const char* pcfile)
{
	FILE* f = fopen(pcfile,"r");
	PCODE_METHOD ** methods;
	//get the count of method
	if(f == NULL)
		return;
	
	int count;
	int idx;
	fread(&count, 1, sizeof(int), f);
	if(count <= 0)
	{
		fclose(f);
		return;
	}
	
	methods = (PCODE_METHOD**)malloc(sizeof(PCODE_METHOD*)*count);
	
	for(idx=count-1; idx>=0; idx--){
		char szname[256];
		int i;
		PCODE_METHOD *pm;
		for(i=0;;i++){
			szname[i] = fgetc(f);
			if(szname[i] == 0)
				break;
		}
		
		pm = (PCODE_METHOD*)malloc(sizeof(PCODE_METHOD));
		fread(&pm->argc, 1, sizeof(int),f);
		fread(&pm->code_len, 1, sizeof(int),f);
		pm->codes = (unsigned char*)malloc(pm->code_len);
		fread(pm->codes, pm->code_len, 1, f);
		if(strcmp(szname, "main") == 0)
			pmain = pm;
		methods[idx] = pm;
	}
	method_table.method_count = count;
	method_table.methods = methods;
	fclose(f);
}

static void Run(int argc, char* argv[])
{
	int *args = NULL;
	if(argc > 2){
		int n = argc - 2;
		int i = 0;
		args = (int*)alloca(sizeof(int)*n);
		for(i=0;i<n;i++)
			args[i] = atoi(argv[i+2]);
	}
	PCODE_ENV* env = CreatePCodeEnv(pmain, 1024*10, args);
	ExecutePCode(&pcode, env);
	DeletePCodeEnv(env);
}

static void DeleteAll()
{
	int i;
	for(i=0;i<method_table.method_count; i++)
	{
		DeletePCodeMethod(method_table.methods[i]);
	}
	free(method_table.methods);
}

int main(int argc, char* argv[])
{
	get_methods(argv[1]);
	Run(argc, argv);
	DeleteAll();
	return 0;
}
