#!/usr/bin/bash
# script : bar (lemonbar)
# author : mannahoc
# modifier : addy

# fetch colors from wal
. "${HOME}/.cache/wal/colors.sh"

# format an icon
icon() {
    printf "%s\\n" "%{F${color1}}${1}%{F-}"
}

# workspaces
get_workspaces() {
	desk=$(xdotool get_desktop)
    current_workspace=$(($desk + 1))
    case "$current_workspace" in
        1) printf "%s\\n" "$(icon )";;
        2) printf "%s\\n" "$(icon )";;
        3) printf "%s\\n" "$(icon )";;
        4) printf "%s\\n" "$(icon )";;
        5) printf "%s\\n" "$(icon )";;
        6) printf "%s\\n" "$(icon )";;
        7) printf "%s\\n" "$(icon )";;
        8) printf "%s\\n" "$(icon )";;
    esac
}

# get song
get_song() {

        printf "%.75s\\n" "$(icon ) $(now-playing)"
}

# get clock
get_clock() {
    printf "%s\\n" "$(icon ) $(date +'%a %H:%M')"
}

# get network
get_network() {
    network_id=$(/sbin/iwgetid -r)
    if [ -n "$network_id" ]
    then
        printf "%s\\n" "$(icon ) ${network_id}"
    else
        printf "%s\\n" "$(icon ) offline"
    fi
}

# get battery
get_battery() {
    battery_status=$(cat /sys/class/power_supply/BAT1/status)
    battery_percent=$(cat /sys/class/power_supply/BAT1/capacity)
    case "$battery_status" in
        Charging)
            printf "%s%%\\n" "$(icon ) ${battery_percent}"
            ;;
        Discharging)
            if [ "$battery_percent" -gt 80 ]
            then
                printf "%s%%\\n" "$(icon ) ${battery_percent}"
            elif [ "$battery_percent" -gt 30 ]
            then
                printf "%s%%\\n" "$(icon ) ${battery_percent}"
            else
                printf "%s%%\\n" "$(icon ) ${battery_percent}"
            fi
            ;;
        Unknown|Full) printf "%s%%\\n" "$(icon ) 100";;
    esac
}

# main
main() {
    # loop
    while :
    do
       printf "%s%s%s\\n" \
            "%{l}  $(get_workspaces)" \
            "%{c}$(get_song)" \
            "%{r}$(get_network)  $(get_battery) $(get_clock)  "
        sleep 1s
    done |\
    # lemonbar settings
    lemonbar -g 'x28' \
        -f 'Roboto:size=8' -f 'Siji:size=8' \
        -B "${color0}" -F "${color7}" \
        -n 'bar' \
        LVDS-1
}

main
