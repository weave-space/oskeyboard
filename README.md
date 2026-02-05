# A Simple On-Screen Keyboard
`osk` is an On-Screen Keyboard written in C, using GTK bindings, and is meant for use on Linux / Wayland / Hyprland setups and the Spaces ecosystem, more to come soon on that.

Currently, the default (and only) option is to toggle `osk` when `osk` is ran again. This keeps the keyboard in memory, keeping the launch nice and responsive.

## Setup
There are a few steps necessary in order to have this OSK work.
First, check that you have the `uinput` kernel module loaded:
```
lsmod | grep uinput

# If there is no output, try this:
sudo modprobe uinput
```

Second, open `/etc/udev/rules.d/99-uinput.rules` in your text editor of choice and add this line to the file: 
```
KERNEL=="uinput", GROUP="input", MODE="0660", OPTIONS+="static_node=uinput"
```

Then run the following command:
```
sudo usermod -aG input username
```

Log out and back in, and you'll be all set.

Alternatively, change the group to `uinput` in the `/etc/udev/rules.d/99-uinput.rules` file and in the `usermod` command, as the `input` group has access to other input devices, not just virtual input devices.

## Build from source
Run the following commands:
```
git clone https://github.com/weave-space/oskeyboard
cd oskeyboard
make
./osk
```

## Customization
There are two ways to customize `oskeyboard`: editing `keymap.h` and `styles.css`. Currently there is a limitation of custom layouts, as the number of rows and the number of keys per row are hardcoded in `main.c`. That will be remedied in the near future. 

Any changes to `keymap.h` or `styles.css` require you to recompile, simply run `make` to do so.

## TODO
- [ ] Responsive design sizes
- [ ] CLI flags
- [ ] Man page
- [ ] Documentation
- [ ] In-depth testing
- [ ] Easier customization & custom layouts

## Credits
- Google Gemini 3 (Explanation of docs and concepts.)
- https://docs.gtk.org/gtk4/index.html