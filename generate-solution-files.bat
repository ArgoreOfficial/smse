@echo off
xmake f -y --vs=2022
xmake project -k vsxmake -y -m "Debug, Release" -a "x64" ./build