# NOTE :
# On Debian ~/.profile file is not executed by LightDM
# So, reanme this file to ~/.xsessionrc if You are using Debian

# Make ~/.bin folder executable
export PATH=$PATH:~/.bin

# My default editor is nano. I don't even able to find exit button in vim :D
export EDITOR='nano'

# Disable GTK CSD
# More explanation here
# https://github.com/PCMan/gtk3-nocsd
if [ -n "$STARTUP" ]; then
    BASESTARTUP=${STARTUP%% *}
    BASESTARTUP=${BASESTARTUP##*/}
    if [ "$BASESTARTUP" = x-session-manager ]; then
      BASESTARTUP=$(basename $(readlink /etc/alternatives/x-session-manager))
    fi
    if [ x"$BASESTARTUP" = x"${BASESTARTUP#gnome-session}" ] ; then
      export GTK_CSD=0
      STARTUP="env LD_PRELOAD=/path/to/libgtk3-nocsd.so.0 $STARTUP"
    fi
  fi

# Okay, finally after searching for a couple month I can exit vim. So, vim!
export EDITOR='vim'
