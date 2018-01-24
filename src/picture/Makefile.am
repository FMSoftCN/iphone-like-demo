AUTOMAKE_OPTIONS=subdir-objects

AM_CPPFLAGS = -I../ -I../../include -I../../animate -I../../animate -I./../agg/

bin_PROGRAMS = picture

picture_LDADD = ../agg/.libs/libagg.al ../../animate/.libs/libanimate.al -lstdc++

picture_SOURCES = picture.c picture.h ../sharedbuff_api.c  

SUBDIRS=view
