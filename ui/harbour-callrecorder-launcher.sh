#!/bin/bash

CONFIG=/home/nemo/.config/harbour-callrecorder/callrecorder.ini

if [ -f $CONFIG ]; then
    LOCALE=$(awk -F '=' '/locale=/{print($2)}' "$CONFIG")
fi

if [ -z "$LOCALE" ] || [ "$LOCALE" = "system" ]; then
	harbour-callrecorder "$@"
else
	LANG=$LOCALE harbour-callrecorder "$@"
fi
