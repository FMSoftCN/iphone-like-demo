AUTOMAKE_OPTIONS=subdir-objects

AM_CPPFLAGS = -I../ -I../../ -I../ebctrl -I../../include -I../../animate -I../../ime -I./../agg/

bin_PROGRAMS=memo

memo_LDFLAGS = -L../ebctrl -L../../ime ./../agg/.libs/libagg.a 
memo_LDADD = -lebctrl -lanimate -lcommon_animates -lstdc++

memo_SOURCES= memo.c custom_scroll.c memo.h ../sharedbuff_api.c ../sfkbd_client.c

SUBDIRS=res
