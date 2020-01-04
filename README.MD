# Blockout (Breakout clone) for GameBoy Advance

Note this is an early version that still has some debugging enabled (on-screen output) and is unfinished. I wrote this code in 2002 and just recently found it on a burned DVD at the end of 2019. I've started to clean things up a bit, but still need to primarily fix how the shooting works and add more levels.

### How to build:

I have included everything you should need to build it on Windows NT based operating systems.

After cloning the repo, unzip all files within \bin\devkitadv\ so they end up in c:\devkitadv\

Run \src\make.bat to build main.gba

Run main.gba in your emulator.


### Tools:

To get "GBA Map Editor Beta 4" working on newer versions of Windows:

Download comdlg32.ocx and copy it to C:\Windows\SysWOW64\ (for 64 bit Windows).

Run from elevated cmd: C:\Windows\SysWOW64\regsvr32 C:\Windows\SysWOW64\comdlg32.ocx
