#!/bin/bash

CONFIG=/home/nemo/.config/harbour-callrecorder/callrecorder.ini

LOCALE=$(awk -F '=' '/locale=/{print($2)}' "$CONFIG")

if [ -z "$LOCALE" ] || [ "$LOCALE" = "system" ]; then
	harbour-callrecorder "$@"
else
	LANG=$LOCALE harbour-callrecorder "$@"
fi
