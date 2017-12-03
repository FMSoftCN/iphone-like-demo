INCLUDES = -I../ -I../../include -I../../animate -I../../animate -I./../agg/

bin_PROGRAMS = picture

picture_LDFLAGS = ./../agg/.libs/libagg.a 
picture_LDADD = -lanimate -lcommon_animates -lstdc++

picture_SOURCES = picture.c picture.h ../sharedbuff_api.c

SUBDIRS=view
