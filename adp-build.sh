#!/bin/bash

export PATH=/usr/local/arm/3.3.2/bin:$PATH

top_dir=`pwd`
#dir_to_tools="mgadp-tools-3.0.2"
#dir_to_minigui="libminigui-3.0.2"
#dir_to_minigui_res="minigui-res-3.0.2"
dir_to_demo="iphone-like-demo"

#path_to_tools="${top_dir}/${dir_to_tools}"
#path_to_minigui="${top_dir}/${dir_to_minigui}"
#path_to_minigui_res="${top_dir}/${dir_to_minigui_res}"
path_to_demo="${top_dir}/${dir_to_demo}"

#path_to_minigui_cfg="${top_dir}/${dir_to_demo}/minigui/"

if test $# -eq 0; then
    TARGET="pc"
else
    TARGET="$1"
fi

usage ()
{
    echo Please run this script in the parent of iphone-like-demo and
    echo "    make sure that you have the source of "
    echo "    'iphone-like-demo' in the working directory, "
    echo "    then run './iphone-like-demo/adp-build.sh [pc|ipaq|clean]'"
    exit 0
}

case "$TARGET" in
  pc)
    ;;

  ipaq)
    ;;

  clean)
#    (cd $path_to_minigui; make clean)
    (cd $path_to_demo; make clean)
    exit
    ;;
  *)
    usage
    ;;
esac

#if test ! -d ${dir_to_tools}; then
#    usage
#fi

#if test ! -d ${dir_to_minigui}; then
#    usage
#fi

#if test ! -d ${dir_to_minigui_res}; then
#    usage
#fi

if test ! -d ${dir_to_demo}; then
    usage
fi

# generate source files of libminigui incore resouce
#(cd $path_to_tools; CFLAGS=-g ./configure --disable-mgadpconftool && make)

#if test $? -eq 0; then
#    echo Generating the mgetc.c of MiniGUI...
#    ${path_to_tools}/mgcfg-trans/mgetc-trans < ${path_to_demo}/minigui/MiniGUI-${TARGET}.cfg > ${path_to_minigui}/src/sysres/mgetc.c
#    echo done
#    echo Generating the source files of incore fonts...
#    ${path_to_tools}/inner-res-trans/inner-res-trans -i ${path_to_minigui_res} -l ${path_to_demo}/minigui/res.list -o ${path_to_minigui}/src/sysres/font -p __mgir -n _font_inner_res
#    echo done
#fi

# configure libminigui and build minigui
#echo Configuring, building, and installing MiniGUI...
#(cd $path_to_minigui; ${path_to_minigui_cfg}/config-minigui.${TARGET} && make && sudo make install)
#echo done
echo Copy the config file to /usr/local/etc
cp  ${path_to_demo}/minigui/MiniGUI-${TARGET}.cfg  /usr/local/etc/MiniGUI.cfg -f
echo done

# configure demo 
echo Configuring iphone-like-demo...
(cd $path_to_demo; ${path_to_demo}/config-demo.${TARGET})
echo done

# build animation and ime libraries
echo Building and installing animation and ime libraries.
(cd ${path_to_demo}/animate; make && sudo make install)
(cd ${path_to_demo}/ime; make && sudo make install)
echo done

# build applications
echo Building and installing applications.
(cd ${path_to_demo}; make && sudo make install)
echo done

