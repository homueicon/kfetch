# kaboomfetch

A lightweight system information tool for Linux, written in C. Displays a distribution ASCII logo alongside system details directly in your terminal.


Example output:
```
                  -`                  eva@machine
                 .o+`                 ----------
                `ooo/                 OS: Arch Linux x86_64
               `+oooo:                Kernel: 6.9.3-arch1-1
              `+oooooo:               Shell: zsh
              -+oooooo+:              RAM: 3141MiB / 15926MiB
            `/:-:++oooo+:             Uptime: 5h 35m
           `/++++/+++++++:
          `/++++++++++++++:
         `/+++ooooooooooooo/`
        ./ooosssso++osssssso+`
       .oossssso-````/ossssss+`
      -osssssso.      :ssssssso.
     :osssssss/        osssso+++.
    /ossssssss/        +ssssooo/-
  `/ossssso+/:-        -:/+osssso+-
 `+sso+:-`                 `.-/+oso:
`++:.                           `-/+/
.`                                 `/
```

Supported Distributions

Arch, Artix, Alpine, Bedrock, CachyOS, Fedora, Debian, Ubuntu, Gentoo, openSUSE, openSUSE Leap, openSUSE Tumbleweed, Void, Windows (WSL), Devuan, Parabola, BlackArch, Linux From Scratch (LFS), Kaboom (custom bomb logo)


Requirements

- Linux or WSL
- GCC or any C99-compatible compiler
- libpng
- libjpeg


Building
```
  git clone https://code.chipmunk.land/homueicon/kaboomfetch.git
```
```
cd kaboomfetch
```
```sudo mv kaboomfetch /usr/local/bin/
 gcc -o kaboomfetch kaboomfetch.c -lpng -ljpeg
```
Install dependencies if needed:

  Fedora:
    sudo dnf install libpng-devel libjpeg-devel

  Arch / Artix / CachyOS:
    sudo pacman -S libpng libjpeg-turbo

  Debian / Ubuntu:
    sudo apt install libpng-dev libjpeg-dev

  Alpine:
    sudo apk add libpng-dev libjpeg-turbo-dev

  Gentoo:
    sudo emerge libpng jpeg

  openSUSE:
    sudo zypper install libpng16-devel libjpeg8-devel

  Void:
    sudo xbps-install libpng-devel libjpeg-turbo-devel

  Windows (WSL):
    winget install libpng-devel libjpeg-turbo-devel git


# Install
```
  sudo mv kaboomfetch /usr/local/bin/
```
or
```
  sudo mv kaboomfetch /usr/bin
```

Usage

  # Auto-detect distro and run
  kaboomfetch

  # Force a specific distro logo
  kaboomfetch --logo <distro>

  # Render a JPG or PNG as ASCII art logo
  kaboomfetch --logo ~/path/to/image.png

  # Set ASCII image size in columns x rows
  kaboomfetch --logo ~/path/to/image.png --size 40x20

  # Re-run the interactive setup wizard
  kaboomfetch --setup

  # Open the config file in $EDITOR
  kaboomfetch --config

  # Show help
  kaboomfetch --help

Available logo names:

  arch artix alpine bedrock cachyos fedora debian ubuntu gentoo opensuse opensuse-leap opensuse-tumbleweed void windows devuan parabola blackarch lfs kaboom


First Run

On first launch, kaboomfetch opens an interactive setup wizard to configure your username, hostname, and which fields to display. The wizard runs automatically when no config file is found. To reopen it manually, pass --setup.


Configuration

Settings are saved to ~/.config/kaboomfetch/kaboomfetch.conf after setup. The file can also be edited directly with kaboomfetch --config or any text editor.

Example:
```
  user=alice
  host=workstation
  show_os=1
  show_kernel=1
  show_shell=1
  show_ram=0
  show_uptime=0
  show_cpu=0
  show_disk=0
  show_packages=0
  show_de=0
  show_wm=0
  show_terminal=0
```
Set any show_* field to 1 to display it or 0 to hide it.


Adding a Distribution

1. Add an entry to the DistroID enum in kaboomfetch.c
2. Define a static const char *lines_<name>[] array with the ASCII art
3. Add a row to the distros[] table with line count and max width
4. Add a detection string in detect_distro()


License

MIT


Support

Contributions and pull requests are welcome.
