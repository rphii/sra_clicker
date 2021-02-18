@echo off
if not exist ./prof mkdir ./prof
gcc -pg sra_clicker.c mouse.c locate.c keyboard.c procedures.c -lgdi32 -o prof/prof.exe
cd prof
prof.exe
gprof prof.exe gmon.out > prof.txt
prof.txt
