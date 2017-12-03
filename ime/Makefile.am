TOP_DIR=..
INCLUDES=-I../include -I../animate
LIBS=-lanimate -lcommon_animates

lib_LTLIBRARIES = libsoftkeywin.la

libsoftkeywin_la_SOURCES= softkeywindow.c tooltip.c softkeywindow.h tooltip.h \
					     softkeyboard/pinyin_kbd.c softkeyboard/en_kbd.c \
						 softkeyboard/punct_kbd.c softkeyboard/num_kbd.c\
						 softkeyboard/common.c \
						 softkeyboard/resource.c \
						 libime/ime_pinyin.c \
						 libime/mgpti_dict_ops.c \
						 libime/mgpti_user_dict.c \
						 libime/mgpti_helper.c \
						 libime/mgpti_dict.c \
						 libime/mgpti.c \
						 libime/ime.c

SUBDIRS=libime softkeyboard
