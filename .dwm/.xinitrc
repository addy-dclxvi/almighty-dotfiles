#!/bin/sh

#        ___                       ___                                   ___           ___     
#       /__/|        ___          /__/\        ___           ___        /  /\         /  /\    
#      |  |:|       /  /\         \  \:\      /  /\         /  /\      /  /::\       /  /:/    
#      |  |:|      /  /:/          \  \:\    /  /:/        /  /:/     /  /:/\:\     /  /:/     
#    __|__|:|     /__/::\      _____\__\:\  /__/::\       /  /:/     /  /:/~/:/    /  /:/  ___ 
#   /__/::::\____ \__\/\:\__  /__/::::::::\ \__\/\:\__   /  /::\    /__/:/ /:/___ /__/:/  /  /\
#      ~\~~\::::/    \  \:\/\ \  \:\~~\~~\/    \  \:\/\ /__/:/\:\   \  \:\/:::::/ \  \:\ /  /:/
#       |~~|:|~~      \__\::/  \  \:\  ~~~      \__\::/ \__\/  \:\   \  \::/~~~~   \  \:\  /:/ 
#       |  |:|        /__/:/    \  \:\          /__/:/       \  \:\   \  \:\        \  \:\/:/  
#       |  |:|        \__\/      \  \:\         \__\/         \__\/    \  \:\        \  \::/   
#       |__|/                     \__\/                                 \__\/         \__\/    

# I don't know what these lines below do
# I just find them in default .xinitrc
if [ -d /etc/X11/xinit/xinitrc.d ]; then
  for f in /etc/X11/xinit/xinitrc.d/*; do
    [ -x "$f" ] && . "$f"
  done
  unset f
fi

# Make ~/.toys folder executable
export PATH="${PATH}:$HOME/.toys"

# Make ~/.utility folder executable
export PATH="${PATH}:$HOME/.utility"

# Make ~/.notify folder executable
export PATH="${PATH}:$HOME/.notify"

# Make ~/.notify folder executable
export PATH="${PATH}:$HOME/.executor"

# Some necessary startup
xsetroot -cursor_name left_ptr
xset fp+ ~/.fonts/misc 
xrdb ~/.Xresources
feh --bg-fill ~/.dwm/wallpaper.jpg
compton -b
dwmstatus &
dunst &

# Then dwm is ready
exec dwm
