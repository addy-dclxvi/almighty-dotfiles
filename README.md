<p align="center">
<a name="top" href="https://github.com/addy-dclxvi/almighty-dotfiles/"><img src="https://raw.githubusercontent.com/addy-dclxvi/almighty-dotfiles/master/header.png"></a>
</p>

## Introduction
A collection of configs to be placed in the users home directory usually prefixed with a period, hence the name dotfiles.
This repo is actually intended for my personal backup, but everyone are welcome to use these resources. <br />
Designed for **Arch Linux**, but can also works for another distro.

## Preview

## Openbox : Fallen Leaf
![alt text](https://raw.githubusercontent.com/addy-dclxvi/almighty-dotfiles/master/preview-openbox.jpg) <br />
**Openbox Theme** : Arroundie (included in this repo). To get the rounded corner, You need *openbox-patched* (AUR). <br />
**GTK Theme** : Lumiere, **Panel** : tint2, **Running Apps** : Feh, Thunar, & vim inside termite <br />

## i3-gaps : Scenes From a Memory
![alt text](https://raw.githubusercontent.com/addy-dclxvi/almighty-dotfiles/master/preview-i3.jpg) <br />
**Stuff in the screenshot** : Ncmpcpp inside Urxvt, dunst, eLinks, and Conky piped into i3bar <br />

## Bspwm : Eiffel
![alt text](https://raw.githubusercontent.com/addy-dclxvi/almighty-dotfiles/master/preview-bspwm.jpg) <br />
**Stuff in the screenshot** : Polybar, Weechat, and dunst <br />

## Fluxbox : Polygon
![alt text](https://raw.githubusercontent.com/addy-dclxvi/almighty-dotfiles/master/preview-fluxbox.jpg) <br />
**Stuff in the screenshot** : Vim inside termite, Fluxbox Toolbar, and dunst <br />

## Herbstluftwm : Winter
![alt text](https://raw.githubusercontent.com/addy-dclxvi/almighty-dotfiles/master/preview-herbstluftwm.jpg) <br />
**Stuff in the screenshot** : tint2 & ncmpcpp <br />

## xfwm-standalone : Take-Off
![alt text](https://raw.githubusercontent.com/addy-dclxvi/almighty-dotfiles/master/preview-xfwm.jpg) <br />
**Stuff in the screenshot** : Polybar, Plank, and rofi <br />

## Spectrwm : Hexagon
![alt text](https://raw.githubusercontent.com/addy-dclxvi/almighty-dotfiles/master/preview-spectrwm.jpg) <br />
**Stuff in the screenshot** : Conky piped into dzen2, vim inside termite, Thunar, mpdviz, and ncmpcpp <br />

## Fvwm : Abstract
![alt text](https://raw.githubusercontent.com/addy-dclxvi/almighty-dotfiles/master/preview-fvwm.jpg) <br />
**Stuff in the screenshot** : Fvwm Pager, Minimized to Thumbnail module (neofetch, ncmpcpp, and CAVA), Thunar, vim inside termite, and dunst. I haven't pushed this, too lazy to fix some problem. This WM really drain my patience (╯°□°）╯︵ ┻━┻ <br />


## Containing
- **Openbox** <br /> Some useful keybinds like snap window with useless gaps (super + arrows). MPC control using *play/pause, next, previous,* and *stop* keys. *Super + number* to switch workspace like in i3wm.
*Super + shift + number* to send window to another workspace (also like in i3wm), and more keybinds. Please read the *rc.xml* first before using it, just in case I forgot to remove my **self-destruct** keybind before I pushed it to this repo. <br /> <br />
And a well commented *autostart* file is included. Also including one bonus Openbox Theme. Want more themes? please check my [Openbox Theme Collections](https://github.com/addy-dclxvi/Openbox-Theme-Collections).

- **i3-gaps** <br /> Pretty stock simple config with useless gaps. My config is using Conkybar (I mean Conky piped into i3bar) as statusbar.

- **Another WM** <br /> Pretty stock simple setup of herbstluftwm, Bspwm, Fluxbox, Spectrwm, & [xfwm-standalone](https://github.com/addy-dclxvi/xfwm4-standalone). Maybe next I will also post my [compiz-standalone](https://github.com/addy-dclxvi/compiz-standalone), Dwm, and Fvwm setup. If I'm not lazy :p

- **obmenu-generator** <br /> My Openbox menu is generated using obmenu-generator pipemenu. To use my scheme simply copy my scheme & config, then install *obmenu-generator* from AUR and type ```obmenu-generator -p``` and reconfigure Openbox.
  
- **zshrc** <br /> Containing some useful alias, and simple [Oh-My-Zsh](https://aur.archlinux.org/packages/oh-my-zsh-git) config. Including my minimalist oh-my-zsh themes (in ~/.oh-my-zsh folder).
To use it, simply install oh-my-zsh package, copy my .zshrc & .oh-my-zsh folder to your home. Then switch shell using ```chsh``` command.
  
- **Compton** <br /> Eye candy shadow. Also some useful "exclude" to avoid broken compositing.

- **Conky** <br /> Just a single conkyrc. For my another Conky collections please check my other [repos](https://github.com/addy-dclxvi?tab=repositories).

- **Tint2** <br /> Just a single tint2rc. For my another Tint2 collections please check my [Tint2 Themes Collections](https://github.com/addy-dclxvi/Tint2-Theme-Collections). You need tint2 with executor support, and my executor scripts is placed inside *~/.bin*

- **termite** <br /> My custom color schemes for termite. To use white color schemes just launch ```termite-light``` command from *dmenu* (if you are using use my *~/.bin* & *.profile*). And Use *M+ 1mn* fonts.

- **Xresources** <br /> My custom color schemes & config for Urxvt, including copy paste support by [Muennich](https://github.com/muennich/urxvt-perls). Hit Alt+C to copy, and Alt+V to paste (*xsel* package is needed). Use *M+ 1mn* fonts.<br />
Also contains my stupidly minimalist *rofi* theme. The default color scheme is Nord dark. If You want to use light color scheme type this ```xrdb ~.Xresources-light```.

- **ncmpcpp** <br /> A simple ncmpcpp config, with album art support. The album art script is taken from [Marco](https://marcocheung.wordpress.com/). Slightly modified to get rid the transparency bug. <br />
To use ncmpcpp with album art, simply launch ```my-music``` from dmenu (if you are using my *~/.bin* & *.profile*). Urxvt with pixbuf enabled is needed. You can get it from AUR, the package name is *rxvt-unicode-pixbuf*. And Use *M+ 1mn* fonts, because of what? It just looks nice. <br />
Also integrated with *dunst* notification. Just copy my dunst config, install dunst, and do nothing. Dunst will launch automatically if triggered by *notify-send* event.

- **gtk.css** <br /> Add useless space wasting paddings (who don't love terminal paddings? it looks nice) for vte-based terminal, including termite.

- **bin** <br /> The most useful script here is gitsetup & gitpush by [Erik Dubois](http://erikdubois.be/quick-easy-way-set-personal-github-repository), and some script for Tint2 executor.
The rest are only toys from [Crunchbang Forum](https://crunchbang.org/forums/viewtopic.php?id=13645). Don't forget to make them executable. Also copy *.profile* file to your home to make them executable without writing full path.

## Additional Information
Some of these configs are hardcoded, that mean You need to edit the configs before it would work. Like the path and default apps.
- My username at my machine is *addy@freako*, so some of these configs path are pointing to ```/home/addy``` 

- My wlan adapter is *wlp2s0*, and my ethernet adapter is *enp1s0*. If you find these in the configs, replace them with your own. Use ```iwconfig``` to find yours. 

- My fonts are M+ 1mn (for terminal) and Roboto & Noto Sans (for GTK, openbox, panel, and other things that don't need a fixed space font). For iconic fonts, depends on what You see inside the config file. 

- My Web Browser is Chromium.

- My terminal is termite, and Urxvt as spare.

- My program launcher is dmenu & rofi.

- My text editor is Geany.

- My wallpaper handler is Feh.

- My screenshooter is Scrot. You will see that my keybind for printscreen is executing scrot.

- My music player is ncmpcpp & Audacious (only for double click mp3 file, I mainly using ncmpcpp), my media player is VLC.

- My image viewers are Viewnior.

- My Window Managers are Openbox, i3-gaps, Bspwm, Herbstluftwm, Xfwm (standalone), Compiz, Dwm, Fvwm, Fluxbox, and Spectrwm. Arch Linux turns me from *Distro Hopper* into *WM Hopper*.

- My panels are tint2 (with executor support) and polybar (with libmpdclient support). I gave up with lemonbar & dzen2 after slamming my head into wall twice!

- My favourite distros are Arch Linux, BunsenLabs, and ArchLabs.

- My favourite bands are Dream Theater, Metallica, and NOFX. (Not important :p)

- My favourite drinks are Red Velvet, Ice Cream Cone, Cola flavour Soda, Avocado Juice, Vanilla Milk, and Espresso (with less sugar, I'm already sweet :D). Just in case You want to buy me some.

## Package List
I can't remember one by one what packages need to be installed to make everything works.
But I have generated a [pkglist.txt](https://github.com/addy-dclxvi/almighty-dotfiles/blob/master/pkglist.txt) file. It contains a list of my installed packages, maybe it will help You to decide what packages need to be installed.
It is pretty barebones & not so bloated, only containing 772 packages. <br />
Or if You want to brutally replicate my setup, just install them all using ```yaourt -S pkglist.txt --needed```  <br />
It will install every packages that I have but You don't have. But I'm too afraid to test it by myself. Maybe You want to be a volunteer to test the command above, and post the results here. If your OS is broken, I will not use that command. <br /> <br />
EDIT : Day to day I have installed more and more packages, and some of my packages is not available in the repo. So, I don't update my package list.

## Comments
I have spend so much time writing comments in my config files for a purpose, to be read.

## Apologies
I'm sorry for this damned *readme* file. I'm trying to provide the best. And about **self-destruct** keybind, I was kidding. There is no such thing here. I just want to see people read the config files before using it.
Only copy & paste configs without learn anything is not a Linux way. <br />
Regards, Addy.

## Motivational Quotes
"The Linux philosophy is *'Laugh in the face of danger'*. Oops! Wrong One. *'Do it yourself'*. Yes, that's it." -- Linus Torvalds

## My Links
[Google+](https://plus.google.com/+AdhiPambudi)
[Deviant Art](http://addy-dclxvi.deviantart.com/)
