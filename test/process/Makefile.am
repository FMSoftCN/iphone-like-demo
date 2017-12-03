INCLUDES =  -I../../ime -I../../ime/libime

if MGRM_PROCESSES
bin_PROGRAMS = mginit notebook
else
bin_PROGRAMS = 
endif
LIBS=-lanimate -lcommon_animates

if COMPILE_MLSSHELL
mlskbd_SOURCES = mlskbd.c
else
mlskbd_SOURCES =
endif

mginit_SOURCES = mginit.c ${mlskbd_SOURCES}

mginit_LDADD = ../../ime/.libs/libsoftkeywin.a

notebook_SOURCES = notebook.c 
