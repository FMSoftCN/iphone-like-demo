AUTOMAKE_OPTIONS = subdir-objects

TOP_DIR=..
AM_CPPFLAGS=-I../include

noinst_LTLIBRARIES=libanimate.la

SRC_FILES=animate_sence.c timeline.c p-code.c \
    common_animates/common_animates.c common_animates/alpha_animate.c

HDR_FILES=animate.h p-code.h instruction.h my_debug.h \
    common_animates/common_animates.h

libanimate_la_SOURCES=$(SRC_FILES) $(HDR_FILES)

SUBDIRS=common_animates

