#!/bin/bash

CONFIG=/home/nemo/.config/harbour-callrecorder/callrecorder.ini

LOCALE=`grep -F "locale=" "$CONFIG" | awk -F '=' '{print($2)}'`

if [ -z "$LOCALE" ] || [ "$LOCALE" = "system" ]; then
	harbour-callrecorder "$@"
else
	LANG=$LOCALE harbour-callrecorder "$@"
fi
