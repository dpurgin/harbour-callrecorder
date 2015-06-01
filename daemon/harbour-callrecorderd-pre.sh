#!/bin/bash

CONFIGDIR=/home/nemo/.config/harbour-callrecorder
CONFIG="$CONFIGDIR/callrecorder.ini"
ENVFILE="$CONFIGDIR/environment"

LOCALE=`grep -F "locale=" "$CONFIG" | awk -F '=' '{print($2)}'`

if [ -z "$LOCALE" ] || [ "$LOCALE" = "system" ]; then
        echo "" > "$ENVFILE"
else
        echo "LANG=$LOCALE" > "$ENVFILE"
fi
