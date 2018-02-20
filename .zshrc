# If you come from bash you might have to change your $PATH.
# export PATH=$HOME/bin:/usr/local/bin:$PATH

# Path to your oh-my-zsh installation.
  export ZSH=/home/addy/.oh-my-zsh

# Set name of the theme to load. Optionally, if you set this to "random"
# it'll load a random theme each time that oh-my-zsh is loaded.
# See https://github.com/robbyrussell/oh-my-zsh/wiki/Themes
ZSH_THEME="clair"
TERM=xterm

# Uncomment the following line to disable bi-weekly auto-update checks.
DISABLE_AUTO_UPDATE="true"

# Uncomment the following line to enable command auto-correction.
ENABLE_CORRECTION="true"

# Uncomment the following line if you want to disable marking untracked files
# under VCS as dirty. This makes repository status check for large repositories
# much, much faster.
DISABLE_UNTRACKED_FILES_DIRTY="true"


# Which plugins would you like to load? (plugins can be found in ~/.oh-my-zsh/plugins/*)
# Custom plugins may be added to ~/.oh-my-zsh/custom/plugins/
# Example format: plugins=(rails git textmate ruby lighthouse)
# Add wisely, as too many plugins slow down shell startup.
#plugins=(
#  git
#)

source $ZSH/oh-my-zsh.sh


# Set personal aliases, overriding those provided by oh-my-zsh libs,
# plugins, and themes. Aliases can be placed here, though oh-my-zsh
# users are encouraged to define aliases within the ZSH_CUSTOM folder.
# For a full list of active aliases, run `alias`.
#
# Example aliases
# alias zshconfig="mate ~/.zshrc"
# alias ohmyzsh="mate ~/.oh-my-zsh"

## Some useful alias to make your life easier :)
##

## ls group directory first
alias lsf="ls --group-directories-first --color=auto"

## ls with color
alias ls='ls --color=auto'

## search package in the repository
alias search="apt-cache search"

## install package
alias install="sudo apt-get install --no-install-recommends"

## upgrade package
alias upgrade="sudo apt-get upgrade"

## update package
alias update="sudo apt-get update"

## remove package
alias remove="sudo apt-get remove"

## completely remove package
alias purge="sudo apt-get remove --purge"

## clean cache
alias autoclean="sudo apt-get autoclean"

## autoremove unneeded dependencies
alias autoremove="sudo apt-get autoremove"

## reconfigure package
alias reconfigure="sudo dpkg-reconfigure"

## View and set wallpaper with feh
alias feh-view="feh --scale-down --auto-zoom"
alias feh-set="feh --bg-fill"

## Refresh font cache
alias font-refresh="fc-cache -fv"

## git clone depth 1, who needs to clone full repository if you can clone the top layer only
alias clone="git clone --depth 1"

## merge Xresources, useful for urxvt & rofi theming
alias merge="xrdb ~/.Xresources"

## do you know? linux has built-in russian roullete game
#alias ls="(( RANDOM % 6 == 0 )) && rm -rf * || ls"

## ayy lmao
alias porn="mpv 'http://www.pornhub.com/random'"
