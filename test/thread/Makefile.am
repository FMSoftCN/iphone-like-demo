
INCLUDES =  -I../../ime

if MGRM_PROCESSES
bin_PROGRAMS = 
else
bin_PROGRAMS = notebook
endif

notebook_SOURCES = notebook.c notebook.h
notebook_LDADD = ../../ime/libsoftkeywin.a

LIBS =  -lm -lpng -ljpeg -lfreetype -lpthread -lminigui_ths -lmgutils
