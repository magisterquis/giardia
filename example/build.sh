#!/bin/sh
#
# build.sh
# Builds the stub
# By J. Stuart McMurray
# Created 20170720
# Last Modified 20170720

set -e

cc -o example -I .. ../giardia.c example.c
