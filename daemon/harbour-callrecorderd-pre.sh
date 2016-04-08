#!/bin/bash

CONFIGDIR=/home/nemo/.config/harbour-callrecorder
CONFIG="$CONFIGDIR/callrecorder.ini"
ENVFILE="$CONFIGDIR/environment"

mkdir -p "$CONFIGDIR"

if [ -f "$CONFIG" ]; then
    LOCALE=$(awk -F '=' '/locale=/{print($2)}' "$CONFIG")
fi

if [ -z "$LOCALE" ] || [ "$LOCALE" = "system" ]; then
        echo "" > "$ENVFILE"
else
        echo "LANG=$LOCALE" > "$ENVFILE"
fi
