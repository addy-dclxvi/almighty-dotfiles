#!/bin/bash

# I take this script from Anachron's i3blocks
# I only slightly modify this script to add an option to show icon, useful for my tint2 executor
# 'volume -i' = with icon, 'volume' = text only
# Cheers!
# Addy

statusLine=$(amixer get Master | tail -n 1)
status=$(echo "${statusLine}" | grep -wo "on")
volume=$(echo "${statusLine}" | awk -F ' ' '{print $5}' | tr -d '[]%')

if [[ "${status}" == "on" ]]; then
	if  [[ $1 = "-i" ]]; then
    echo "  ${volume}%"
	else
    echo "${volume}%"
	fi
else
	if  [[ $1 = "-i" ]]; then
	echo "  off"
	else
    echo "off"
	fi
fi
