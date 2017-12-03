#!/bin/sh
export CFLAGS=$TARGET_CFLAGS
export LDFLAGS=$TARGET_LDFLAGS
OPTIONS="${CFLAGS} -I.. ${LDFLAGS} -L.libs -L../.libs -lanimate -lcommon_animates -lminigui_procs -lpthread -lpng -ljpeg"

gcc  -o alpha_test alpha_animate_test.c ${OPTIONS}

