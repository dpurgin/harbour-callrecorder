#!/bin/bash

CONFIGDIR=/home/nemo/.config/harbour-callrecorder
CONFIG="$CONFIGDIR/callrecorder.ini"
ENVFILE="$CONFIGDIR/environment"

LOCALE=$(awk -F '=' '/locale=/{print($2)}' "$CONFIG")

if [ -z "$LOCALE" ] || [ "$LOCALE" = "system" ]; then
        echo "" > "$ENVFILE"
else
        echo "LANG=$LOCALE" > "$ENVFILE"
fi
