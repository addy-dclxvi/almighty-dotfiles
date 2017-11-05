#
# ~/.bashrc
#

# If not running interactively, don't do anything
[[ $- != *i* ]] && return

## bash prompt
PS1='\e[1;31m\u\e[0;39m@\h \e[36m\W \e[39m'
#PS1='\n\e[31m\e[39m\e[31m \e[39m'


## Some useful alias to make your life easier :)
##

## ls group directory first
alias lsf="ls --group-directories-first --color=auto"

## ls with color
alias ls='ls --color=auto'

## View and set wallpaper with feh
alias feh-view="feh --scale-down --auto-zoom"
alias feh-set="feh --bg-fill"

## Refresh font cache
alias font-refresh="fc-cache -fv"

## ---- Not used anymore, I found that put this in ~/.bin is more effective ----
## ncmpcpp with album-art
#alias ncmpcpp-art='urxvt -hold -g 92x10 -e bash -c "ncmpcpp -c ~/.ncmpcpp/config-art" &'
## alternative way
#alias ncmpcpp-art="ncmpcpp -c .ncmpcpp/config-art"

## start termite with light config
#alias termite-light="termite -c ~/.config/termite/config-light &"
## -----------------------------------------------------------------------------

## download mp3 from youtube, ssst this is illegal actually
alias ytmp3="youtube-dl --extract-audio --audio-format mp3"

## git clone depth 1, who needs to clone full repository if you can clone the top layer only
alias clone="git clone --depth 1"

## list all packages that's installed by me. Without packages that only installed as dependency. The dependencies will be installed automatically afterall (if needed).
alias pkglist="pacman -Qqe"

## list all installed packages, every packages
alias pkglist-full="pacman -Qq"

## remove orphaned packages. if the result is "no target specified". you have a clean packages :)
alias pkglist-orphan="sudo pacman -Rns $(pacman -Qtdq)"

## merge Xresources, useful for urxvt & rofi theming
alias merge="xrdb -merge ~/.Xresources"
