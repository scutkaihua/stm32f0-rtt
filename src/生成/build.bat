
del ..\����\*.bin   /F /Q

D:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bin --output=.\objects\v6.bin  .\objects\stm32f0-contiki.axf
cmd.exe /C copy .\objects\v6.bin  ..\����\LD-MZ-DRIVER-5-A-V6.bin
