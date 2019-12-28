rem @echo off

PATH=c:\devkitadv\bin;%PATH%

cls

cd ..\music\data
modconv bladeraver.mod bladeraver.bin
modconv teknopekno.mod teknopekno.bin
bin2o bladeraver.bin bladeraver.o
bin2o teknopekno.bin teknopekno.o
del bladeraver.bin
del teknopekno.bin
cd ..\..\src

gcc -Wl,-Tlnkscript -nostartfiles -marm -mthumb-interwork -Wall -mcpu=arm7tdmi -ffreestanding -fomit-frame-pointer -fpeephole -O3 -o main.elf C:/devkitadv/lib/gcc-lib/arm-agb-elf/3.0.2/interwork/crtbegin.o C:/devkitadv/lib/gcc-lib/arm-agb-elf/3.0.2/interwork/crtend.o crt0.S gba/*.cpp object/*.cpp main.cpp ../music/data/bladeraver.o ../music/data/teknopekno.o ../music/GbaPlayerGCC.a -lm

objcopy -O binary main.elf main.gba
del main.elf
pause
