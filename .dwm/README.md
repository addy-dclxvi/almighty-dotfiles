## Introduction
Dwm compilable source with some customization. This is forked, ummm.. stolen.. from [Boris aka Appath](https://github.com/appath).
So, if You want to give a credit. Give to him instead :wink:

## Containing
- dwm

- dwmstatus

- dmenu

## Dependencies
freetype2, libx11, libxft, libxinerama, maybe there are some dependencies I forget. 
And remember, if You are on Debian. You have to install more packages due to Debian package splitting.

## Installation
- Launch a terminal in *dwm_custom* folder

- ```make```

- Failed? That means You don't have a complete dependencies. Likely You are on Debian :sweat_smile:.
So, use ```apt-file search```. For example, when You do ```make``` and fatal error happens. Read the error message.
It should tell You what file couldn't be found. If it tells You *xcb-xrm* couldn't be found, then do ```apt-file search xcb-xrm```.
*apt-file* will tell You that *xcb-xrm* is part of *libxcb-xrm-dev* package. So, install it ```sudo apt-get install libxcb-xrm-dev```.
Then, repeat the ```make``` command again.

- Compilation succes? Then it's ready to be installed ```sudo make clean install```

- Do the same in *dwmstatus_custom* and *dmenu_custom* folder

## Launching
The safest way to test is..

- Copy my example *.xinitrc* to your home (backup your current *~/.xinitrc* first before copying mine if You have one). 

- Read my *~/.xinitrc*, It contains some useful information. Like some needed startup items, and where I place the font.

- Open TTY2, hit Ctrl+Alt+F2 (You can go back to previous screen by hitting Ctrl+Alt+F1 then Alt+Left, maybe Alt+Left twice).

- Login

- ```startx```

- You should see this <br />
![alt text](https://raw.githubusercontent.com/addy-dclxvi/almighty-dotfiles/master/preview-dwm2.jpg) <br />
I open dmenu with dim effect by the way. Cool isn't it? :sunglasses:

- The iconic fonts is missing from the workspace indicator?? I see You are running Debian. Bitmap font is disabled by default on Debian.
You can enable it using ```sudo dpkg-reconfigure fontconfig-config``` :wink:

## Cheatsheet
- super + shift + p **=** open dmenu

- super + enter **=** launch urxvt

- super + shift + c **=** close window

- super + shift + q **=** quit dwm

- More cheatsheet read the *config.h*

## Now What?
Do some mess with *config.h*, recompile, then reinstall :wink:

## Uninstallation
No need, dwm is great window manager. Keep using it :laughing:





