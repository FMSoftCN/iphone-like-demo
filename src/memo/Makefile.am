AUTOMAKE_OPTIONS=subdir-objects

AM_CPPFLAGS = -I../ -I../../ -I../ebctrl -I../../include -I../../animate -I../../ime -I./../agg/

bin_PROGRAMS=memo

memo_LDADD = ../agg/.libs/libagg.al ../../animate/.libs/libanimate.al ../ebctrl/libebctrl.a -lstdc++

memo_SOURCES= memo.c custom_scroll.c memo.h ../sharedbuff_api.c ../sfkbd_client.c

SUBDIRS=res
