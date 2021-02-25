@echo off
if not exist ./prof mkdir ./prof
windres resource.rc resource.o
gcc -pg sra_clicker.c mouse.c locate.c keyboard.c procedures.c -lgdi32 -luser32 -o prof/prof.exe resource.o 
cd prof
prof.exe
gprof prof.exe gmon.out > prof.txt
prof.txt
cd ..

