#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.61)
AC_INIT(iphone-like-demo, 1.0.1)
AC_CONFIG_SRCDIR(src/mginit.c)

dnl needed for cross-compiling
AC_CANONICAL_SYSTEM

dnl checks for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LD
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h) 

#user define macores
build_landscape="no"
build_close_corner="yes"

build_softkbd_240_320="yes"
build_softkbd_480_272="no"

#Check user define macores.
AC_ARG_ENABLE(landscape,
        [  --enable-landscape   build support landscape <default=yes>],
        build_landscape=$enableval)

AC_ARG_ENABLE(close_corner,
        [  --enable-close_corner   build support close corner <default=yes>],
        build_close_corner=$enableval)

AC_ARG_ENABLE(softkbd240320,
        [  --enable-softkbd240320   build support soft keyboard 240x320 <default=no>],
        build_softkbd_240_320=$enableval)

AC_ARG_ENABLE(softkbd480272,
        [  --enable-softkbd480272   build support soft keyboard 480x272 <default=no>],
        build_softkbd_480_272=$enableval)

AC_ARG_ENABLE([mlsshell],
        [AS_HELP_STRING(--enable-mlsshell,enable mlshadow keyboard <default=no>)],
        build_mlsshell=$enableval)

AC_ARG_ENABLE([tooltip],
        [AS_HELP_STRING(--enable-tooltip,enable tooltip for softkeyboard <default=no>)],
        build_tooltip=$enableval)

AM_CONDITIONAL(COMPILE_MLSSHELL, test "x$build_mlsshell" = "xyes")

#user define macores
if test "x$build_mlsshell" = "xyes" ; then
    AC_DEFINE(ENABLE_MLSSHELL, 1, [Define if MLSSHELL is supported.])
fi

if test "x$build_landscape" = "xyes"; then
    AC_DEFINE(ENABLE_LANDSCAPE, 1, [Define if LANDSCAPE is supported.])
fi

if test "x$build_close_corner" = "xyes"; then
    AC_DEFINE(ENABLE_CLOSE_CORNER, 1, [Define if CLOSE_CORNER is supported.])
fi

if test "x$build_softkbd_480_272" = "xyes"; then
    AC_DEFINE(SOFTKBD_480_272, 1, [Define if SOFTKBD_480_272 is supported.])
else
    if test "x$build_softkbd_240_320" = "xyes"; then
        AC_DEFINE(SOFTKBD_240_320, 1, [Define if SOFTKBD_240_320 is supported.])
    else
        AC_DEFINE(SOFTKBD_320_240, 1, [Define if SOFTKBD_320_240 is supported.])
    fi
fi

if test "x$build_tooltip" = "xyes" ; then
    AC_DEFINE(KBD_TOOLTIP, 1, [Define if TOOLTIP is supported.])
fi


# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS(stdlib.h string.h unistd.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T

# Checks for library functions.
#AC_FUNC_MALLOC
AC_FUNC_FORK
AC_CHECK_FUNCS([memset])


dnl =======================================
dnl check for libminig
hava_libminigui="no"
AC_CHECK_HEADERS(minigui/common.h, have_libminigui=yes, foo=bar)

dnl ======================================
dnl check for non-UNIX system
unix="yes"
AC_CHECK_DECLS(__NOUNIX__,unix="no",foo=bar)

dnl ========================================
dnl check for run-time mode of MiniGUI
procs_version="no"
AC_CHECK_DECLS(_MGRM_PROCESSES, procs_version="yes", foo=bar,[#include <minigui/common.h>])

AC_ARG_WITH(lang,
	[ --with-lang=[en/zhcn/zhtw]])
case "$with_lang" in
	zhcn)
		AC_DEFINE(_LANG_ZHCH, 1,
			[Use text strings for locale zh_CN in GB2312])
		;;
	zhtw)
		AC_DEFINE(_LANG_ZHTW, 1,
			[Use text strings for locale zh_TW in BIG5])
		;;
	*)
		AC_DEFINE(_LANG_EN, 1,
			[Use text strings for locale en_US in ISO8859-1])
		;;
esac

if test "$ac_cv_proc_gcc" = "yes"; then
	CFLAGS="$CFLAGS -Wall -Wstrict-prototypes -pipe"
fi

if test "$procs_version" = "yes"; then
	LIBS="$LIBS -lminigui_procs"
fi


AC_CHECK_DECLS(_HAVE_MATH_LIB, LIBS="$LIBS -lm", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGIMAGE_PNG, LIBS="$LIBS -lpng", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGIMAGE_JPG, LIBS="$LIBS -ljpeg", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGFONT_TTF, LIBS="$LIBS -lttf", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGFONT_FT2, LIBS="$LIBS -lfreetype", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGIAL_DLCUSTOM, LIBS="$LIBS -ldl", foo=bar, [#include <minigui/common.h>])

LIBS="$LIBS -lpthread"

AM_CONDITIONAL(NOUNIX, test "x$unix" = "xno")
AM_CONDITIONAL(MGRM_PROCESSES, test "x$procs_version" = "xyes")
AM_CONDITIONAL(USE_NEWGAL, test "x$use_newgal" = "xyes")

AC_OUTPUT(
Makefile
m4/Makefile
minigui/Makefile
animate/Makefile
animate/common_animates/Makefile
ime/Makefile
ime/libime/Makefile
ime/softkeyboard/Makefile
ime/softkeyboard/240-320/Makefile
ime/softkeyboard/320-240/Makefile
ime/softkeyboard/480-272/Makefile
include/Makefile
src/Makefile
src/agg/Makefile
src/res/Makefile
src/iphone-animates/Makefile
src/ebctrl/Makefile
src/ex_wifi/Makefile
src/memo/Makefile
src/memo/res/Makefile
src/calendar/Makefile
src/calculator/Makefile
src/picture/Makefile
src/picture/view/Makefile
src/watch/Makefile
src/others/Makefile
src/weatherforecast/Makefile
src/boxy/Makefile
src/snake/Makefile
src/scrnsaver/Makefile
src/systemsetting/Makefile
src/systemsetting/render/Makefile
src/systemsetting/date/Makefile
src/systemsetting/localnetwork/Makefile
src/systemsetting/screen_protect/Makefile
src/systemsetting/wifisetting/Makefile
src/systemsetting/res/Makefile
src/systemsetting/res/date/Makefile
src/systemsetting/res/animate/Makefile
src/systemsetting/res/localnetwork/Makefile
src/systemsetting/res/screen_protect/Makefile
src/systemsetting/res/wifisetting/Makefile
dnl src/configWifi/Makefile
dnl test/Makefile
dnl test/process/Makefile
dnl test/thread/Makefile
dnl src/agg/test/CoverFlow/Makefile
)

if test "x$have_libminigui" != "xyes"; then
    AC_MSG_WARN([
		MiniGUI is not properly installed on the system. You need 
        MiniGUI Ver 3.0.1 or later for building this package. 
        Please configure and install MiniGUI Ver 3.0.1 first.
    ])
fi

if test "x$procs_version" != "xyes"; then
	AC_MSG_ERROR([
		You must configure MiniGUI as MiniGUI-Processes mode.
	])
fi

