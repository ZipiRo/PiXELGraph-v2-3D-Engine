BUILD BAT
==================================================================
@echo off

cls

g++ -c main.cpp -IPiXELGraph-v2/Include -IInclude 
g++ main.o -o Build/main PiXELGraph-v2/Library/PiXELGraph-v2.a

del main.o

cd Build
start main
cd .. 
==================================================================

By ZipiRo