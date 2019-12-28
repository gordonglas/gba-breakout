02/10/2001 - GBA Module Player
Copyright (c) Sergej Kravcenko - sergej@codewaves.com

1. COPYRIGHT NOTICE

All libraries and source files inside this SDK are Copyright (C) 2001
by Sergej Kravcenko

This GBA Module Player can be used freely only if *NO MONEY* is involved. Also
my permission is required if you want to use this module player in your
game/demo/tool/other project. Just send me an email with request. Also
you are requested to add my name and email address to project credits list.

YOU CANNOT USE THIS PLAYER IN COMMERCIAL/SHAREWARE PROJECT. PLEASE CONTACT
PLAYER AUTHOR BEFORE ADDING THIS PLAYER IN SUCH PROJECT.  I THINK WE WILL
FIND GOOD DECISION FOR BOTH SIDES. 

2. FEATURES

4 channel module playback (.mod format)
2 channel sfx engine
20%-30% CPU usage
22Khz mixing rate

3. SDK

GBA Module Player SDK contains all required files for complete player integration
in your projects:
1. Module converter
2. Player Libraries and headers (currently for ARM compiler only)
3. 2 demo projects. (Thumb and Arm)
You can find function description inside include files (gbaplayer.h). Demo
projects also show basic module control:
Volume Up/Down - Left/Right
Pause/Resume   - A/B
Sfx Effect     - Up
Module change  - Select

Press select to start module playback

4. Sound effects

Sound effect files conatains RAW sample data (8-bit, signed, mono, up to 21600Hz). You will
need WAV->RAW file converter in order to creat such files. Select outbut to 8-bit mono
RAW output.

5. Basic integration steps

a. Convert module to internal binary format using modconv.exe utility.
b. Add MP_InHandler to your interrupt table (DMA1) and enable CPU interrupts.
c. Call MP_InitXXX function.
d. Every frame call MP_UpdateXXX function.
e. Start module playback using MP_PlayModuleXXX.

6. CONTACTS

If you have any suggestions or questions use this e-mail address:
sergej@codewaves.com 

Best regards,
Sergej Kravcenko 
