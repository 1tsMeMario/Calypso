<p align="center">
    <h3 align="center">Athena</h3>
    <p align="center"><i>CS2 EFI cheat with Arduino Support.</i></p>
</p>

## Setup
- Usb Drive
- [Optional]Arduino

(Any arduino that supports Mouse.h library)

## Features
#### Aimbot
- Customizable `fov`, `smooth` and `key`.
#### Triggerbot
- Customizable `delay` and `key`.
#### Config System
- Cheat uses self-sufficient config system. Dont have config file? Cheat will create it itself.
#### Offsets Updater
- Automatic fetching of latest offsets from [CS2 Dumper Repo](https://github.com/a2x/cs2-dumper/tree/main/output)
#### Arduino Support
- Cheat be used with or without Arduino.

## Default Key Binds

 **Aimbot**: `Mouse 4`
 
 **Triggerbot**: `Mouse 5`

## Usage

1. Format USB drive to FAT32

2. Open `build.bat` inside Athena folder

3. Copy all contents of the USB folder to your formated USB Drive

4. Copy `AthenaUM.exe` to usb drive

5. Start `AthenaUM.exe`


Check out [USAGE.md](.github/docs/USAGE.md) for detailed instruction.


## Showcase & Usage
Click on image below to watch it on YouTube

[![Click](https://i3.ytimg.com/vi/_rVH9mMZ--A/hqdefault.jpg)](https://www.youtube.com/watch?v=_rVH9mMZ--A)


## TODO
+ Add Raspberry Pi Pico W Support(?)
+ Add USB Host Shield support
+ Get module base address via EFI not usermode(?)
+ Add fullscreen esp(?)
+ Clean code and fix bugs

## Credits
+ [Original Code](https://github.com/3a1/Calypso)
+ [EFI Driver](https://github.com/TheCruZ/EFI_Driver_Access)
+ [Arduino Code](https://github.com/backpack-0x1337/CyberAim-Valorant/)
+ [EFI VS Compile](https://github.com/pbatard/uefi-simple)
