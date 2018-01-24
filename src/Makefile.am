TOP_DIR=..
AM_CPPFLAGS = -I. -I../include -I.. -I../ime/

LIBS+=-lanimate -lcommon_animates -lsoftkeywin -lminigui_procs

bin_PROGRAMS = mginit

datadir=$(prefix)/bin
data_DATA=iphone-like-demo.etc animates.pcc

SUBDIRS = agg calculator picture watch others ebctrl memo snake boxy weatherforecast iphone-animates calendar scrnsaver ex_wifi systemsetting res  

mginit_SOURCES= \
		dragmanager.c icon_layout.c desktop-layout.c layout.c \
		layouthlp.c mginit.c mgb_tswin.c phone_icon.c sence.c desktop.c \
		busywin.c busywin_api.c sharedbuff_api.c  sharedbuff.c \
		busywin.h  dragmanager.h  iphone-animates.h  layouthlp.h  \
		mlssize.h  quick_view.h  sfkbd_client.h  timer.h \
		desktop.h  icon_layout.h  layout.h  mlskbd.h phone_icon.h sence.h sharedbuff.h
		${mlskbd_SOURCES}

mginit_LDADD= iphone-animates/.libs/libiphoneanimate.a

EXTRA_DIST=${data_DATA}
