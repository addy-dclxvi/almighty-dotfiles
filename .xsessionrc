# My default editor is nano. I don't even able to find exit button in vim :D
export EDITOR='nano'

# Make ~/.bar folder executable
export PATH="${PATH}:$HOME/.bar"

# Make ~/.toys folder executable
export PATH="${PATH}:$HOME/.toys"

# Make ~/.utility folder executable
export PATH="${PATH}:$HOME/.utility"

# Make ~/.notify folder executable
export PATH="${PATH}:$HOME/.notify"

# Make ~/.notify folder executable
export PATH="${PATH}:$HOME/.executor"

# Make Go apps executable
export GOPATH="$HOME/.go"
export GOBIN="$HOME/.go/bin"
export PATH="${PATH}:$HOME/.go/bin"

RANGER_LOAD_DEFAULT_RC=FALSE

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




