#!/bin/sh

XPM="$1"

XPM_FILE="${XPM}.xpm"
XPM_NAME="${XPM}_xpm"

if ! [ -f $XPM_FILE ]; then
    exit -1
fi

make CFLAGS="-std=c99 -DXPM_FILE=\"$XPM_FILE\" -DXPM_NAME=\"$XPM_NAME\"" xpm_sprite_converter > /dev/null &&
./xpm_sprite_converter

rm -f xpm_sprite_converter
