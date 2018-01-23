AUTOMAKE_OPTIONS=subdir-objects

AM_CPPFLAGS = -I../ -I../../ -I./../ebctrl/ -I./../ex_wifi/ -I../../include -I../../animate/

bin_PROGRAMS=sysmain
sysmain_LDADD= ./../ebctrl/libebctrl.a ./../ex_wifi/libex_wifi.a ./../agg/.libs/libagg.a ./../../animate/common_animates/.libs/libcommon_animates.a ./../../animate/.libs/libanimate.a -lstdc++
sysmain_SOURCES= sysmain.c sysmain.h \
		 ./screen_protect/screen_protect.h ./screen_protect/screen_protect.c \
		 ./render/colordlg.c  \
		 ./render/render.c ./render/render.h \
		 ./date/timeset.c ./date/timeset.h \
		 ./date/moncalendar.c ./date/moncalendar_impl.h \
		 ./date/moncal.h \
		 ./localnetwork/netcardset.c ./localnetwork/netcardset.h \
		 ./wifisetting/wifimanual.c ./wifisetting/wifiset.c ./wifisetting/wifiset.h \
		 ../sharedbuff_api.c 

SUBDIRS=res screen_protect render date localnetwork wifisetting 
