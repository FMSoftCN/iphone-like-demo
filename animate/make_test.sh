#!/bin/sh

gcc -g -o test_new test_new.c libanimate.a -lminigui -lpthread -lpng -ljpeg
